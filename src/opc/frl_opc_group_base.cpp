#include "opc/frl_opc_group_base.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "opc/address_space/frl_opc_tag.h"
#include "opc/frl_opc_group.h"

using namespace frl::opc::address_space;

namespace frl
{
namespace opc
{

GroupBase::GroupBase()
{
	Init();
}

GroupBase::GroupBase( const String &groupName )
	:	name( groupName )
{
	Init();
}

GroupBase::~GroupBase()
{
}

void GroupBase::Init()
{
	boost::mutex::scoped_lock guard( groupGuard );
	clientHandle = 0;
	actived = False;
	enabled = True;
	deleted = False;
	updateRate = 500;
	timeBias = 0;
	deadband = 0;
	localeID = LOCALE_NEUTRAL;
	keepAlive = 0;
	registerInterface(IID_IOPCDataCallback);
	renewUpdateRate();
}

Bool GroupBase::isDeleted()
{
	return deleted;
}

void GroupBase::isDeleted( Bool deleteFlag )
{
	deleted = deleteFlag;
}

void GroupBase::onUpdateTimer()
{
	if( ! actived )
		return;

	boost::mutex::scoped_lock guard( groupGuard );

	if( ! isConnected( IID_IOPCDataCallback ) )
		return;

	std::list< OPCHANDLE > handles;
	GroupItemElemList::iterator end = itemList.end();
	for( GroupItemElemList::iterator it = itemList.begin(); it != end; ++it )
	{
		if( (*it).second->isChange() && (*it).second->isActived() )
			handles.push_back( (*it).first );
	}

	if( ! handles.empty() )
	{
		renewUpdateRate();
		GroupElem tmp = GroupElem( dynamic_cast< Group* >( this ) );
		AsyncRequestListElem request( new AsyncRequest( tmp, async_request::UPDATE, handles) );
		request->setTransactionID( 0 );
		doAsyncRefresh( request );	
	}

	if( keepAlive == 0 ) // keep alive not used
	{
		return;
	}

	FILETIME curTime;
	::GetSystemTimeAsFileTime( &curTime );
	ULONGLONG curTimeCount = *reinterpret_cast< ULONGLONG* >( &curTime );
	if( ( curTimeCount - getLastUpdateTick() ) >= keepAlive * 10000000 )
	{
		renewUpdateRate();
		GroupElem tmp = GroupElem( dynamic_cast< Group* >( this ) );
		AsyncRequestListElem request( new AsyncRequest( tmp, async_request::UPDATE, handles) );
		request->setTransactionID( 0 );
		doAsyncRefresh( request );	
	}
}

void GroupBase::doAsyncRead( IOPCDataCallback* callBack, const AsyncRequestListElem &request )
{
	size_t counts = request->getCounts();

	OPCHANDLE *pHandles = NULL;
	VARIANT *pValue = NULL;
	WORD *pQuality = NULL;
	FILETIME *pTimeStamp = NULL;
	HRESULT *pErrors = NULL;

	pHandles = os::win32::com::allocMemory< OPCHANDLE >( counts );
	if( pHandles == NULL )
		return;

	pValue = os::win32::com::allocMemory< VARIANT >( counts );
	if( pValue == NULL )
	{
		os::win32::com::freeMemory( pHandles );
		return;
	}
	os::win32::com::zeroMemory( pValue, counts );

	pQuality = os::win32::com::allocMemory< WORD >( counts );
	if( pQuality == NULL )
	{
		os::win32::com::freeMemory( pHandles );
		os::win32::com::freeMemory( pValue );
		return;
	}

	pTimeStamp = os::win32::com::allocMemory< FILETIME >( counts );
	if( pTimeStamp == NULL )
	{
		os::win32::com::freeMemory( pHandles );
		os::win32::com::freeMemory( pValue );
		os::win32::com::freeMemory( pQuality );
		return;
	}

	pErrors = os::win32::com::allocMemory< HRESULT >( counts );
	if( pErrors == NULL )
	{
		os::win32::com::freeMemory( pHandles );
		os::win32::com::freeMemory( pValue );
		os::win32::com::freeMemory( pQuality );
		os::win32::com::freeMemory( pTimeStamp );
		return;
	}

	HRESULT masterError = S_OK;
	const std::list< ItemHVQT >  *handles = &request->getItemHVQTList();
	size_t i = 0;
	std::list< ItemHVQT >::const_iterator end = handles->end();
	GroupItemElemList::iterator iter;
	GroupItemElemList::iterator groupIterEnd = itemList.end();
	for( std::list< ItemHVQT >::const_iterator it = handles->begin(); it != end; ++it, ++i )
	{
		iter = itemList.find( (*it).getHandle() );
		if( iter == groupIterEnd )
		{
			masterError = S_FALSE;
			pErrors[i] = OPC_E_INVALIDHANDLE;
			continue;
		}

		pHandles[i] = (*iter).second->getClientHandle();

		try
		{
			pErrors[i] = ((*iter).second->readValue()).copyTo( pValue[i] );
		}
		catch( Tag::NotExistTag& )
		{
			pErrors[i] = OPC_E_INVALIDHANDLE;
		}

		if( FAILED( pErrors[i] ) )
		{
			masterError = S_FALSE;
			continue;
		}

		pQuality[i] = (*iter).second->getQuality();
		pTimeStamp[i] = (*iter).second->getTimeStamp();
	}

	callBack->OnReadComplete(	request->getTransactionID(),
		clientHandle,
		S_OK,
		masterError,
		(DWORD)counts,
		pHandles,
		pValue,
		pQuality,
		pTimeStamp,
		pErrors );
	os::win32::com::freeMemory( pHandles );
	os::win32::com::freeMemory( pValue );
	os::win32::com::freeMemory( pQuality );
	os::win32::com::freeMemory( pTimeStamp );
	os::win32::com::freeMemory( pErrors );
}

void GroupBase::doAsyncRefresh( const AsyncRequestListElem &request )
{
	IOPCDataCallback* callBack = NULL;
	HRESULT hResult = getCallback( IID_IOPCDataCallback, (IUnknown**)&callBack );

	if( FAILED( hResult ) )
		return;

	size_t counts = request->getCounts();

	if( counts == 0 ) // keep alive
	{
		OPCHANDLE pHandles = 0;
		VARIANT pValue;
		VariantInit( &pValue );
		WORD pQuality = 0;
		FILETIME  pTimeStamp;
		memset( &pTimeStamp, 0, sizeof( pTimeStamp ) );
		HRESULT pErrors = S_OK;

		callBack->OnDataChange(	request->getTransactionID(),
			clientHandle,
			S_OK,
			S_OK,
			0,
			&pHandles,
			&pValue,
			&pQuality,
			&pTimeStamp,
			&pErrors );
		callBack->Release();
		return;
	}

	OPCHANDLE *pHandles = NULL;
	VARIANT *pValue = NULL;
	WORD *pQuality = NULL;
	FILETIME *pTimeStamp = NULL;
	HRESULT *pErrors = NULL;

	pHandles = os::win32::com::allocMemory< OPCHANDLE >( counts );
	if( pHandles == NULL )
		return;

	pValue = os::win32::com::allocMemory< VARIANT >( counts );
	if( pValue == NULL )
	{
		os::win32::com::freeMemory( pHandles );
		return;
	}
	os::win32::com::zeroMemory( pValue, counts );

	pQuality = os::win32::com::allocMemory< WORD >( counts );
	if( pQuality == NULL )
	{
		os::win32::com::freeMemory( pHandles );
		os::win32::com::freeMemory( pValue );
		return;
	}

	pTimeStamp = os::win32::com::allocMemory< FILETIME >( counts );
	if( pTimeStamp == NULL )
	{
		os::win32::com::freeMemory( pHandles );
		os::win32::com::freeMemory( pValue );
		os::win32::com::freeMemory( pQuality );
		return;
	}

	pErrors = os::win32::com::allocMemory< HRESULT >( counts );
	if( pErrors == NULL )
	{
		os::win32::com::freeMemory( pHandles );
		os::win32::com::freeMemory( pValue );
		os::win32::com::freeMemory( pQuality );
		os::win32::com::freeMemory( pTimeStamp );
		return;
	}

	const std::list< ItemHVQT >  *handles = &request->getItemHVQTList();
	size_t i = 0;
	std::list< ItemHVQT >::const_iterator end = handles->end();
	GroupItemElemList::iterator iter;
	GroupItemElemList::iterator groupIterEnd = itemList.end();
	for( std::list< ItemHVQT >::const_iterator it = handles->begin(); it != end; ++it, ++i )
	{
		iter = itemList.find( (*it).getHandle() );
		if( iter == groupIterEnd )
		{
			pErrors[i] = OPC_E_INVALIDHANDLE;
			continue;
		}
		pHandles[i] = (*iter).second->getClientHandle();
		try
		{
			if( request->getSource() == OPC_DS_CACHE )
				pErrors[i] = ((*iter).second->getCachedValue()).copyTo( pValue[i] );
			else
				pErrors[i] = ((*iter).second->readValue()).copyTo( pValue[i] );
		}
		catch( Tag::NotExistTag& )
		{
			pErrors[i] = OPC_E_INVALIDHANDLE;
		}

		if( FAILED( pErrors[i] ) )
			continue;

		pQuality[i] = (*iter).second->getQuality();
		pTimeStamp[i] = (*iter).second->getTimeStamp();
	}

	callBack->OnDataChange(	request->getTransactionID(),
		clientHandle,
		S_OK,
		S_OK,
		(DWORD)counts,
		pHandles,
		pValue,
		pQuality,
		pTimeStamp,
		pErrors );
	callBack->Release();
	os::win32::com::freeMemory( pHandles );
	os::win32::com::freeMemory( pValue );
	os::win32::com::freeMemory( pQuality );
	os::win32::com::freeMemory( pTimeStamp );
	os::win32::com::freeMemory( pErrors );
}

void GroupBase::doAsyncWrite( IOPCDataCallback* callBack, const AsyncRequestListElem &request )
{

	size_t counts = request->getCounts();

	OPCHANDLE *pHandles = NULL;
	HRESULT *pErrors = NULL;

	pHandles = os::win32::com::allocMemory< OPCHANDLE >( counts );
	if( pHandles == NULL )
		return;

	pErrors = os::win32::com::allocMemory< HRESULT >( counts );
	if( pErrors == NULL )
	{
		os::win32::com::freeMemory( pHandles );
		return;
	}

	HRESULT masterError = S_OK;

	const std::list< ItemHVQT >  *handles = &request->getItemHVQTList();
	size_t i = 0;
	std::list< ItemHVQT >::const_iterator end = handles->end();
	GroupItemElemList::iterator iter;
	GroupItemElemList::iterator groupIterEnd = itemList.end();
	for( std::list< ItemHVQT >::const_iterator it = handles->begin(); it != end; ++it, ++i )
	{
		iter = itemList.find( (*it).getHandle() );
		if( iter == groupIterEnd )
		{
			masterError = S_FALSE;
			pErrors[i] = OPC_E_INVALIDHANDLE;
			continue;
		}
		pHandles[i] = (*iter).second->getClientHandle();

		if( !(*iter).second->isWritable() )
		{
			masterError = S_FALSE;
			pErrors[i] = OPC_E_BADRIGHTS;
			continue;
		}

		if( (*it).getValue().getType() == VT_EMPTY )
		{
			masterError = S_FALSE;
			pErrors[i] = OPC_E_BADTYPE;
			continue;
		}

		pErrors[i] = (*iter).second->writeValue( (*it).getValue() );

		if( FAILED( pErrors[i] ) )
		{
			masterError = S_FALSE;
			continue;
		}

		if( (*it).isQualitySpecified() )
		{
			(*iter).second->setQuality( (*it).getQuality() );
		}

		if( (*it).isTimeStampSpecified() )
		{
			(*iter).second->setTimeStamp( (*it).getTimeStamp() );
		}
	}

	callBack->OnWriteComplete(	request->getTransactionID(),
		clientHandle,
		masterError,
		( DWORD )counts,
		pHandles,
		pErrors );
	os::win32::com::freeMemory( pHandles );
	os::win32::com::freeMemory( pErrors );
}

void GroupBase::setServerPtr( OPCServer *serverPtr )
{
	server = serverPtr;
}

const String GroupBase::getName()
{
	return name;
}


void GroupBase::setName( const String &newName )
{
	name = newName;
}

OPCHANDLE GroupBase::getClientHandle()
{
	return clientHandle;
}

DWORD GroupBase::getUpdateRate()
{
	return updateRate;
}

FILETIME GroupBase::getLastUpdate()
{
	return lastUpdate;
}

ULONGLONG GroupBase::getLastUpdateTick()
{
	return *( reinterpret_cast< ULONGLONG* >( &lastUpdate ) );
}

void GroupBase::renewUpdateRate()
{
	::GetSystemTimeAsFileTime( &lastUpdate );
}

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
