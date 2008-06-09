#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include <algorithm>
#include "sys/frl_sys_util.h"
#include "opc/frl_opc_server.h"
#include "opc/frl_opc_enum_group.h"
#include "frl_exception.h"
#include "opc/frl_opc_server_factory.h"
#include "frl_util_functors.h"
#include "os/win32/com/frl_os_win32_com_uuidof.h"
#include <boost/lambda/lambda.hpp>

namespace frl
{
namespace opc
{

namespace private_
{
inline
void clearAyncRequestList( const OPCHANDLE &handle, AsyncRequestList &requestList )
{
for( AsyncRequestList::iterator iter = requestList.begin(), remIt;
	iter != requestList.end(); 
	iter = remIt )
{
	remIt = iter;
	++remIt;
	(*iter)->removeHandle( handle );
	if( (*iter)->getCounts() == 0 )
		requestList.erase( iter );
}
}
}

OPCServer::OPCServer()
	: refCount( 0 )
{
	// call opcAddressSpace::getInstance().finalConstruct first !
	if( ! opcAddressSpace::getInstance().isInit() )
		FRL_THROW_S_CLASS( address_space::AddressSpace::NotFinalConstruct );

	os::win32::com::zeroMemory<OPCSERVERSTATUS>( &serverStatus );
	CoFileTimeNow( &serverStatus.ftStartTime );
	serverStatus.szVendorInfo = L"Serg Baburin";
	serverStatus.dwServerState = OPC_STATUS_NOCONFIG;
	serverStatus.dwBandWidth = 0xFFFFFFFF;
	serverStatus.wMajorVersion = 2;
	registerInterface(IID_IOPCShutdown);
	factory.LockServer( TRUE );
	
	timerRead.init( this, &OPCServer::onReadTimer );
	timerWrite.init( this, &OPCServer::onWriteTimer );
	timerRead.setTimer( 1 );
	timerWrite.setTimer( 1 );
	timerRead.start();
	timerWrite.start();
}

OPCServer::~OPCServer()
{
	timerRead.tryStop();
	asyncReadSignal();
	timerRead.stop();

	timerWrite.tryStop();
	asyncWriteSignal();
	timerWrite.stop();

	factory.LockServer( FALSE );
}

void OPCServer::onReadTimer()
{
	boost::mutex::scoped_lock lock( readMtx );
	while( asyncReadList.empty() && ! timerRead.isStop() )
		readCnd.wait( lock );

	if( timerRead.isStop() )
		return;

	AsyncRequestList tmp;
	{
		boost::mutex::scoped_lock guard( readGuard );
		asyncReadList.swap( tmp );
	}

	IOPCDataCallback* ipCallback = NULL;
	HRESULT hResult;
	
	boost::mutex::scoped_lock guard( scopeGuard );
	GroupElemMap::iterator group;
	GroupElemMap::iterator groupEnd = groupItem.end();
	
	AsyncRequestList::iterator end = tmp.end();
	for( AsyncRequestList::iterator it = tmp.begin(); it != end; ++it )
	{
		group = groupItem.find( (*it)->getGroupHandle() );
		if( group == groupEnd )
			continue;
		hResult = (*group).second->getCallback( IID_IOPCDataCallback, (IUnknown**)&ipCallback );
		if( FAILED( hResult ) )
			continue;

		if( (*it)->isCancelled() )
		{
			ipCallback->OnCancelComplete( (*it)->getTransactionID(), (*group).second->getClientHandle() );
		}
		else
		{
			if( (*it)->getCounts() != 0 )
				(*group).second->doAsyncRead( ipCallback, (*it) );
		}
		ipCallback->Release();
	}
}

void OPCServer::onWriteTimer()
{	
	boost::mutex::scoped_lock lock( writeMtx );
	while( asyncWriteList.empty() && ! timerWrite.isStop() )
		writeCnd.wait( lock );

	if( timerWrite.isStop() )
		return;

	AsyncRequestList tmp;
	{
		boost::mutex::scoped_lock guard( writeGuard );
		asyncWriteList.swap( tmp );
	}

	IOPCDataCallback* ipCallback = NULL;
	HRESULT hResult;

	boost::mutex::scoped_lock guard( scopeGuard );
	GroupElemMap::iterator group;
	GroupElemMap::iterator groupEnd = groupItem.end();

	AsyncRequestList::iterator end = tmp.end();
	for( AsyncRequestList::iterator it = tmp.begin(); it != end; ++it )
	{
		group = groupItem.find( (*it)->getGroupHandle() );
		if( group == groupEnd )
			continue;
		hResult = (*group).second->getCallback( IID_IOPCDataCallback, (IUnknown**)&ipCallback );
		if( FAILED( hResult ) )
			continue;

		if( (*it)->isCancelled() )
		{
			ipCallback->OnCancelComplete( (*it)->getTransactionID(), (*group).second->getClientHandle() );
		}
		else
		{
			if( (*it)->getCounts() != 0 )
				(*group).second->doAsyncWrite( ipCallback, (*it) );
		}
		ipCallback->Release();
	}
}

void OPCServer::addAsyncReadRequest( const AsyncRequestListElem &request )
{
	boost::mutex::scoped_lock guard( readGuard );
	asyncReadList.push_back( request );
}

void OPCServer::addAsyncWriteRequest( const AsyncRequestListElem &request )
{
	boost::mutex::scoped_lock guard( writeGuard );
	asyncWriteList.push_back( request );
}

STDMETHODIMP OPCServer::QueryInterface( REFIID iid, LPVOID* ppInterface )
{
	if( ppInterface == NULL )
		return E_POINTER;

	*ppInterface = NULL;

	if( iid == __uuidof( IOPCCommon ) || iid == IID_IUnknown )
	{
		*ppInterface = (dynamic_cast< IOPCCommon* >( this ) );
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCServer ) )
	{
		*ppInterface = (dynamic_cast< IOPCServer* >( this ) );
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCItemProperties ) )
	{
		*ppInterface = (dynamic_cast< IOPCItemProperties* >( this ) );
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCBrowseServerAddressSpace ) )
	{
		*ppInterface = (dynamic_cast< IOPCBrowseServerAddressSpace* >( this ) );
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IConnectionPointContainer ) )
	{
		*ppInterface = (dynamic_cast< IConnectionPointContainer* >( this ) );
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCBrowse ) )
	{
		*ppInterface = (dynamic_cast< IOPCBrowse* >( this ) );
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCItemIO ) )
	{
		*ppInterface = (dynamic_cast< IOPCItemIO* >( this ) );
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG OPCServer::AddRef( void )
{
	return ::InterlockedIncrement( &refCount );
}

ULONG OPCServer::Release( void )
{
	LONG tmp = ::InterlockedDecrement( &refCount );
	if( tmp == 0 )
		delete this;
	return tmp;
}

HRESULT STDMETHODCALLTYPE OPCServer::AddGroup(	/* [string][in] */ LPCWSTR szName,
																					/* [in] */ BOOL bActive,
																					/* [in] */ DWORD dwRequestedUpdateRate,
																					/* [in] */ OPCHANDLE hClientGroup,
																					/* [in][unique] */ LONG *pTimeBias,
																					/* [in][unique] */ FLOAT *pPercentDeadband,
																					/* [in] */ DWORD dwLCID,
																					/* [out] */ OPCHANDLE *phServerGroup,
																					/* [out] */ DWORD *pRevisedUpdateRate,
																					/* [in] */ REFIID riid,
																					/* [iid_is][out] */ LPUNKNOWN *ppUnk )
{
	if (phServerGroup == NULL || pRevisedUpdateRate == NULL || ppUnk == NULL)
		return E_INVALIDARG;

	*phServerGroup = 0;
	*pRevisedUpdateRate = 0;
	*ppUnk = NULL;

	String name;

	boost::mutex::scoped_lock guard( scopeGuard );
	if( szName == NULL || wcslen( szName ) == 0 )
	{
		name = util::getUniqueName();
	}
	else
	{
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			if( groupItemIndex.find( szName ) != groupItemIndex.end() )
				return OPC_E_DUPLICATENAME;
			name = szName;
		#else
		if( groupItemIndex.find( wstring2string( szName ) ) != groupItemIndex.end() )
			return OPC_E_DUPLICATENAME;
		name = wstring2string( szName );
		#endif
	}

	if( pPercentDeadband )
		if( *pPercentDeadband < 0 || *pPercentDeadband > 100 )
			return E_INVALIDARG;
	Group *newGroup = new Group( name );

	if( newGroup == NULL )
		return E_OUTOFMEMORY;

	((IOPCItemMgt*)(newGroup))->AddRef();

	LONG lTimeBias = 0;
	if (pTimeBias == NULL)
	{
		TIME_ZONE_INFORMATION cZoneInfo;
		GetTimeZoneInformation(&cZoneInfo);
		lTimeBias = cZoneInfo.Bias;
	}
	else
	{
		lTimeBias = *pTimeBias;
	}

	HRESULT res = newGroup->SetState
		( &dwRequestedUpdateRate, pRevisedUpdateRate, &bActive,
		&lTimeBias, pPercentDeadband, &dwLCID, &hClientGroup);
	if( FAILED(res) )
	{
		delete newGroup;
		return res;
	}
	HRESULT queryResult = newGroup->QueryInterface( riid, (void**)ppUnk );
	if( FAILED( queryResult ) || *ppUnk == NULL)
	{
		delete newGroup;
		return E_NOINTERFACE;
	}

	newGroup->setServerPtr( this );
	groupItemIndex.insert( std::pair< String, Group* >( newGroup->getName(), newGroup ) );
	groupItem.insert( std::pair< OPCHANDLE, Group* >( newGroup->getServerHandle(), newGroup ) );
	if( phServerGroup )
		*phServerGroup = newGroup->getServerHandle();
	return res;
}

HRESULT STDMETHODCALLTYPE OPCServer::GetErrorString( /* [in] */ HRESULT dwError, /* [in] */ LCID dwLocale, /* [string][out] */ LPWSTR *ppString )
{
	if (ppString == NULL)
		return E_INVALIDARG;

	*ppString = NULL;
	return util::getErrorString( dwError, dwLocale, &ppString );
}

HRESULT STDMETHODCALLTYPE OPCServer::GetGroupByName( /* [string][in] */ LPCWSTR szName, /* [in] */ REFIID riid, /* [iid_is][out] */ LPUNKNOWN *ppUnk )
{
	if( ppUnk == NULL )
		return E_INVALIDARG;
	*ppUnk = NULL;

	if( szName == NULL || wcslen( szName ) == 0  )
		return E_INVALIDARG;

	boost::mutex::scoped_lock guard( scopeGuard );

	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
	String name = szName;
	#else
	String name = wstring2string( szName );
	#endif

	GroupElemIndexMap::iterator it = groupItemIndex.find( name );
	if(it == groupItemIndex.end() )
		return E_INVALIDARG;

	HRESULT hr = (*it).second->QueryInterface( riid, (void**)ppUnk );
	if( FAILED(hr) || *ppUnk == NULL)
	{
		*ppUnk = NULL;
		return E_NOINTERFACE;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE OPCServer::GetStatus( /* [out] */ OPCSERVERSTATUS **ppServerStatus )
{
	if(ppServerStatus == NULL)
		return E_INVALIDARG;

	boost::mutex::scoped_lock guard( scopeGuard );
	OPCSERVERSTATUS *stat = os::win32::com::allocMemory<OPCSERVERSTATUS>();
	memcpy( stat, &serverStatus, sizeof(OPCSERVERSTATUS) );
	CoFileTimeNow(&( stat->ftCurrentTime));

	*ppServerStatus = stat;
	stat->szVendorInfo = os::win32::com::allocMemory<WCHAR>( (wcslen(serverStatus.szVendorInfo)+1) * sizeof(WCHAR) );
	memcpy( stat->szVendorInfo, serverStatus.szVendorInfo, (wcslen( serverStatus.szVendorInfo) + 1) * sizeof(WCHAR) );
	stat->dwGroupCount = (DWORD) groupItemIndex.size();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE OPCServer::RemoveGroup( /* [in] */ OPCHANDLE hServerGroup, /* [in] */ BOOL bForce )
{
	boost::mutex::scoped_lock guard( scopeGuard );

	GroupElemMap::iterator it = groupItem.find( hServerGroup );
	if( it == groupItem.end() )
		return E_FAIL;

	Group* grpItem = (*it).second.get();
	groupItemIndex.erase( grpItem->getName() );
	groupItem.erase( it );
	grpItem->isDeleted( True );
	if( grpItem->Release() != 0 && ! bForce )
	{
		return OPC_S_INUSE;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE OPCServer::CreateGroupEnumerator( /* [in] */ OPCENUMSCOPE dwScope, /* [in] */ REFIID riid, /* [iid_is][out] */ LPUNKNOWN *ppUnk )
{
	boost::mutex::scoped_lock guard( scopeGuard );

	if( riid == IID_IEnumUnknown )
	{
		std::vector< GroupElem > unkn;
		if( dwScope != OPC_ENUM_PUBLIC && dwScope != OPC_ENUM_PUBLIC_CONNECTIONS )
		{
			unkn.reserve( groupItem.size() );
			for( GroupElemMap::iterator it = groupItem.begin(); it != groupItem.end(); ++it )
				unkn.push_back( (*it).second );
		}

		EnumGroup *enumGroup;
		if( unkn.size() )
		{
			enumGroup = new EnumGroup( unkn );
		}
		else
		{
			enumGroup = new EnumGroup();
		}
		HRESULT result = enumGroup->QueryInterface( riid, (void**)ppUnk );
		if( FAILED( result ) )
			delete enumGroup;
		return unkn.size() ? S_OK : S_FALSE;
	}
	if( riid == IID_IEnumString )
	{
		std::vector< String > nameList;
		if( dwScope != OPC_ENUM_PUBLIC && dwScope != OPC_ENUM_PUBLIC_CONNECTIONS )
		{
			nameList.reserve( groupItem.size() );
			for( GroupElemMap::iterator it = groupItem.begin(); it != groupItem.end(); ++it )
				nameList.push_back( (*it).second->getName() );
		}

		EnumString *enumString = new EnumString();
		if( nameList.size() )
		{
			enumString->init( nameList );
		}
		HRESULT result = enumString->QueryInterface( riid, (void**)ppUnk );
		if( FAILED( result ) )
			delete enumString;
		return nameList.size() ? S_OK : S_FALSE;
	}
	return E_NOINTERFACE;
}

HRESULT OPCServer::setGroupName( const String &oldName, const String &newName )
{
	boost::mutex::scoped_lock guard( scopeGuard );

	GroupElemIndexMap::iterator it =  groupItemIndex.find( newName );
	if( it != groupItemIndex.end() )
		return OPC_E_DUPLICATENAME;

	it = groupItemIndex.find( oldName );
	if( it == groupItemIndex.end() )
		return E_INVALIDARG;

	GroupElem tmpGroup = (*it).second;
	groupItemIndex.erase( it );
	groupItemIndex.insert( std::pair< String, GroupElem >( newName, tmpGroup) );
	return S_OK;
}

HRESULT OPCServer::cloneGroup( const String &name, const String &cloneName, Group **ppClone )
{
	if ( ppClone == NULL )
		return E_INVALIDARG;

	boost::mutex::scoped_lock guard( scopeGuard );

	GroupElemIndexMap::iterator it = groupItemIndex.find( cloneName );
	if( it != groupItemIndex.end() )
		return OPC_E_DUPLICATENAME;

	it = groupItemIndex.find( name );
	if( it == groupItemIndex.end() )
		return E_INVALIDARG;

	*ppClone = (*it).second->clone();
	if ( ppClone == NULL )
		return E_OUTOFMEMORY;
	((IOPCItemMgt*)(*ppClone))->AddRef();
	(*ppClone)->setName( cloneName );
	return addNewGroup( ppClone );
}

HRESULT OPCServer::addNewGroup( Group **group )
{
	if ( group == NULL )
		FRL_THROW( FRL_STR("Invalid agrument") );

	if( (*group)->getName().empty() )
		(*group)->setName( util::getUniqueName() );

	GroupElem newGroup( *group );
	groupItemIndex.insert( std::pair< String, GroupElem >( (*group)->getName(), newGroup ) );
	groupItem.insert( std::pair< OPCHANDLE, GroupElem >( (*group)->getServerHandle(), newGroup ) );
	return S_OK;
}

void OPCServer::setServerState( OPCSERVERSTATE newState )
{
	if( newState < OPC_STATUS_RUNNING || newState > OPC_STATUS_TEST )
		FRL_THROW_S_CLASS( OPCServer::InvalidServerState );
	serverStatus.dwServerState = newState;
	CoFileTimeNow( &serverStatus.ftCurrentTime );
}

OPCSERVERSTATE OPCServer::getServerState()
{
	return serverStatus.dwServerState;
}

void OPCServer::asyncReadSignal()
{
	boost::mutex::scoped_lock lock( readMtx );
	readCnd.notify_one();
}

void OPCServer::asyncWriteSignal()
{
	boost::mutex::scoped_lock lock( writeMtx );
	writeCnd.notify_one();
}

frl::Bool OPCServer::asyncRequestCancel( DWORD id )
{
	Bool isExistRead = False;
	Bool isExistWrite = False;
	AsyncRequestList::iterator it;
	AsyncRequestList::iterator end;
	{
		boost::mutex::scoped_lock lock( readGuard );
		end = asyncReadList.end();
		for( it = asyncReadList.begin(); it != end; ++it )
		{
			if( (*it)->getCancelID() == id )
			{
				isExistRead = True;
				break;
			}
		}
		if( isExistRead )
			(*it)->isCancelled( True );		
	}

	{
		boost::mutex::scoped_lock lock( writeGuard );
		end =  asyncWriteList.end();
		for( it = asyncWriteList.begin(); it != end; ++it )
		{
			if( (*it)->getCancelID() == id )
			{
				isExistWrite = True;
				break;
			}
		}
		if( isExistWrite )
			(*it)->isCancelled( True );	
	}
	
	return ( isExistRead || isExistWrite );
}

void OPCServer::removeItemFromAsyncReadRequestList( OPCHANDLE handle_ )
{
	boost::mutex::scoped_lock guar( readGuard );
	private_::clearAyncRequestList( handle_, asyncReadList );
}

void OPCServer::removeItemFromAsyncWriteRequestList( OPCHANDLE handle_ )
{
	boost::mutex::scoped_lock guar( writeGuard );
	private_::clearAyncRequestList( handle_, asyncWriteList );
}

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
