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
#include "opc/frl_opc_group.h"

namespace frl
{
namespace opc
{

OPCServer::OPCServer()
	:	refCount( 0 )
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

	updateThread = boost::thread(boost::bind( &OPCServer::updateGroups, this ) );

	registerInterface( IID_IOPCShutdown );
	factory.LockServer( TRUE );
}

OPCServer::~OPCServer()
{
	stopUpdate.signal();
	updateThread.join();

	factory.LockServer( FALSE );
}

void OPCServer::addAsyncReadRequest( AsyncRequestListElem &request )
{
	request_manager.addReadRequest( request );
}

void OPCServer::addAsyncWriteRequest( AsyncRequestListElem &request )
{
	request_manager.addWriteRequest( request );
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
	if( phServerGroup == NULL || pRevisedUpdateRate == NULL || ppUnk == NULL )
		return E_INVALIDARG;

	*phServerGroup = 0;
	*pRevisedUpdateRate = 0;
	*ppUnk = NULL;

	if( pPercentDeadband )
	{
		if( *pPercentDeadband < 0 || *pPercentDeadband > 100 )
		{
			return E_INVALIDARG;
		}
	}

	GroupElem new_group;
	String name;
	boost::mutex::scoped_lock guard( scopeGuard );
	if( szName == NULL || wcslen( szName ) == 0 )
	{
		name = util::getUniqueName();
	}
	else
	{
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			name = szName;
		#else	
			name = wstring2string( szName );
		#endif
	}

	try
	{
		new_group = group_manager.addGroup( name );
	}
	catch( GroupManager::IsExistGroup& )
	{
		return OPC_E_DUPLICATENAME;
	}

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

	new_group->setServerPtr( this );
	HRESULT res = new_group->SetState(
		&dwRequestedUpdateRate,
		pRevisedUpdateRate,
		&bActive,
		&lTimeBias,
		pPercentDeadband,
		&dwLCID,
		&hClientGroup );

	if( FAILED(res) )
	{
		group_manager.removeGroup( name );
		return res;
	}
	HRESULT queryResult = new_group->QueryInterface( riid, (void**)ppUnk );
	if( FAILED( queryResult ) || *ppUnk == NULL)
	{
		group_manager.removeGroup( name );		
		return E_NOINTERFACE;
	}
	
	if( phServerGroup )
	{
		*phServerGroup = new_group->getServerHandle();
	}
	return res;
}

HRESULT STDMETHODCALLTYPE OPCServer::GetErrorString( /* [in] */ HRESULT dwError, /* [in] */ LCID dwLocale, /* [string][out] */ LPWSTR *ppString )
{
	if( ppString == NULL )
	{
		return E_INVALIDARG;
	}
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

	GroupElem group;
	try
	{
		group = group_manager.getGroup( name );
	}
	catch( GroupManager::NotExistGroup& )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = group->QueryInterface( riid, (void**)ppUnk );
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
	stat->dwGroupCount = (DWORD) group_manager.getGroupCount();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE OPCServer::RemoveGroup( /* [in] */ OPCHANDLE hServerGroup, /* [in] */ BOOL bForce )
{
	boost::mutex::scoped_lock guard( scopeGuard );
	try
	{
		request_manager.removeGroupFromRequest( hServerGroup );
		if( ! group_manager.removeGroup( hServerGroup ) && ! bForce )
		{
			return OPC_S_INUSE;
		}
	}
	catch( GroupManager::NotExistGroup& )
	{
		return E_FAIL;
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
			try
			{
				unkn = group_manager.getGroupEnum();
			}
			catch( GroupManager::NotExistGroup& )
			{
			}
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
			try
			{
				nameList = group_manager.getNamesEnum();
			}
			catch( GroupManager::NotExistGroup& )
			{			
			}
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
	try
	{
		group_manager.renameGroup( oldName, newName );
	}
	catch( GroupManager::IsExistGroup )
	{
		return OPC_E_DUPLICATENAME;
	}
	catch( GroupManager::NotExistGroup )
	{
		return E_INVALIDARG;
	}
	return S_OK;
}

frl::opc::GroupElem OPCServer::cloneGroup( String &name , String &to_name )
{
	return group_manager.cloneGroup( name, to_name );
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

frl::Bool OPCServer::asyncRequestCancel( DWORD id )
{	
	return ( request_manager.cancelRequest( id ) );
}

void OPCServer::removeItemFromRequestList( OPCHANDLE item_handle )
{
	request_manager.removeItemFromRequest( item_handle );
}

void OPCServer::removeGroupFromRequestList( OPCHANDLE group_handle )
{
	request_manager.removeGroupFromRequest( group_handle );
}

void OPCServer::updateGroups()
{
	while( ! stopUpdate.timedWait( 50 ) )
	{
		group_manager.updateGroups();
	}
}

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
