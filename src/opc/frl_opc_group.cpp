#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <algorithm>
#include "opc/frl_opc_group.h"
#include "opc/frl_opc_util.h"
#include "frl_util_functors.h"
#include "os/win32/com/frl_os_win32_com_uuidof.h"

using namespace frl::opc::address_space;

namespace frl
{
namespace opc
{
HRESULT STDMETHODCALLTYPE Group::QueryInterface( /* [in] */ REFIID iid, /* [iid_is][out] */ void** ppInterface )
{
	if( ppInterface == NULL )
		return E_POINTER;

	*ppInterface = NULL;

	if( iid == __uuidof( IOPCGroupStateMgt ) || iid == IID_IUnknown )
	{
		*ppInterface = (dynamic_cast<IOPCGroupStateMgt*>(this));
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCItemMgt ) )
	{
		*ppInterface = (dynamic_cast<IOPCItemMgt*>(this));
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCSyncIO ) )
	{
		*ppInterface = (dynamic_cast<IOPCSyncIO*>(this));
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCSyncIO2 ) )
	{
		*ppInterface = (dynamic_cast<IOPCSyncIO2*>(this));
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCAsyncIO2 ) )
	{
		*ppInterface = (dynamic_cast<IOPCAsyncIO2*>(this));
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCAsyncIO3 ) )
	{
		*ppInterface = (dynamic_cast<IOPCAsyncIO3*>(this));
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IConnectionPointContainer ) )
	{
		*ppInterface = (dynamic_cast< IConnectionPointContainer* >(this));
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCGroupStateMgt2 ) )
	{
		*ppInterface = (dynamic_cast< IOPCGroupStateMgt2* >(this));
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCItemDeadbandMgt ) )
	{
		*ppInterface = (dynamic_cast< IOPCItemDeadbandMgt* >(this));
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE Group::AddRef( void )
{
	return ::InterlockedIncrement( &refCount );
}

ULONG STDMETHODCALLTYPE Group::Release( void )
{
	LONG tmp = ::InterlockedDecrement( &refCount );
	if( tmp == 0 )
		delete this;
	return tmp;
}

HRESULT STDMETHODCALLTYPE Group::CreateInstance( IUnknown** ippUnknown, const CLSID* pClsid )
{
	if( ippUnknown == NULL)
		return E_POINTER;
	
	*ippUnknown = NULL;
	Group* pObject = new Group();
	pObject->clsid = pClsid;
	HRESULT hResult = pObject->QueryInterface(IID_IUnknown, (void**)ippUnknown);
	pObject->Release();
	return hResult;
}

Group::Group()
	:	refCount( 0 )
{
	Init();
}

Group::Group( const String &groupName )
	:	refCount( 0 ),
		name( groupName )
{
	Init();
}

Group::~Group()
{
	groupGuard.lock();
	timerUpdate.stop();
	unregisterInterface(IID_IOPCDataCallback);
	groupGuard.unLock();
}

void Group::Init()
{
	clientHandle = 0;

	actived = False;
	enabled = True;
	deleted = False;
	updateRate = 500;
	timeBias = 0;
	deadband = 0;
	localeID = LOCALE_NEUTRAL;
	keepAlive = 0;
	::GetSystemTimeAsFileTime( &lastUpdate );
	tickOffset = -1;
	registerInterface(IID_IOPCDataCallback);
	
	timerUpdate.init( this, &Group::onUpdateTimer );
	timerUpdate.setTimer( updateRate );
}

void Group::setServerPtr( OPCServer *serverPtr )
{
	server = serverPtr;
}

const String Group::getName()
{
	return name;
}

LONG Group::getRefCount()
{
	return refCount;
}

frl::Bool Group::isDeleted()
{
	return deleted;
}

void Group::isDeleted( Bool deleteFlag )
{
	deleted = deleteFlag;
}

void Group::setName( const String &newName )
{
	name = newName;
}

void Group::onUpdateTimer()
{
	if( timerUpdate.isStop() || ! actived )
		return;

	lock::ScopeGuard guard( groupGuard );

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
		GetSystemTimeAsFileTime( &lastUpdate );
		AsyncRequestListElem request( new AsyncRequest( getServerHandle(), handles) );
		request->setTransactionID( 0 );
		doAsyncRefresh( request );	
	}

	if( keepAlive == 0 ) // keep alive not used
	{
		return;
	}
	
	FILETIME curTime;
	GetSystemTimeAsFileTime( &curTime );
	ULONGLONG lastUpdateCount = *(ULONGLONG*)( &lastUpdate );
	ULONGLONG curTimeCount = *(ULONGLONG*)( &curTime );
	if( ( curTimeCount - lastUpdateCount ) >= keepAlive * 10000000 )
	{
		GetSystemTimeAsFileTime( &lastUpdate );
		AsyncRequestListElem request( new AsyncRequest( getServerHandle(), handles) );
		request->setTransactionID( 0 );
		doAsyncRefresh( request );	
	}
}

void Group::doAsyncRead( IOPCDataCallback* callBack, const AsyncRequestListElem &request )
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
	for( std::list< ItemHVQT >::const_iterator it = handles->begin(); it != end; ++it, ++i )
	{
		GroupItemElemList::iterator iter = itemList.find( (*it).getHandle() );
		if( iter == itemList.end() )
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

void Group::doAsyncRefresh( const AsyncRequestListElem &request )
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
	for( std::list< ItemHVQT >::const_iterator it = handles->begin(); it != end; ++it, ++i )
	{
		GroupItemElemList::iterator iter = itemList.find( (*it).getHandle() );
		if( iter == itemList.end() )
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

void Group::doAsyncWrite( IOPCDataCallback* callBack, const AsyncRequestListElem &request )
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
	for( std::list< ItemHVQT >::const_iterator it = handles->begin(); it != end; ++it, ++i )
	{
		GroupItemElemList::iterator iter = itemList.find( (*it).getHandle() );
		if( iter == itemList.end() )
		{
			masterError = S_FALSE;
			pErrors[i] = OPC_E_INVALIDHANDLE;
			continue;
		}
		pHandles[i] = (*iter).second->getClientHandle();

		if(  ! ( (*iter).second->getAccessRights() & OPC_WRITEABLE ) )
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

Group* Group::clone()
{
	Group *newGroup = new Group();
	newGroup->registerInterface(IID_IOPCDataCallback);

	lock::ScopeGuard guard( groupGuard );

	newGroup->name = name;
	newGroup->actived = False;
	newGroup->clsid = clsid;
	newGroup->deadband = deadband;
	newGroup->enabled = enabled;
	newGroup->keepAlive = keepAlive;
	newGroup->lastUpdate = lastUpdate;
	newGroup->localeID = localeID;
	newGroup->server = server;
	newGroup->tickOffset = tickOffset;
	newGroup->timeBias = timeBias;
	newGroup->updateRate = updateRate;
	newGroup->clientHandle = clientHandle;
	newGroup->deleted = deleted;

	GroupItemElemList::const_iterator end = itemList.end();
	for( GroupItemElemList::const_iterator it = itemList.begin(); it != itemList.end(); ++it )
	{
		GroupItemElem item( (*it).second->clone() );
		newGroup->itemList.insert( std::pair< OPCHANDLE, GroupItemElem > ( item->getServerHandle(), item ) );
	}
	newGroup->timerUpdate.setTimer( newGroup->updateRate );
	return newGroup;
}

OPCHANDLE Group::getClientHandle()
{
	return clientHandle;
}
} //namespace opc
} //namespace FatRat Library

#endif // FRL_PLATFORM_WIN32
