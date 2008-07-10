#include "opc/impl/frl_opc_impl_server.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "opc/frl_opc_enum_string.h"
#include "opc/frl_opc_server.h"
#include "opc/frl_opc_group.h"
#include "opc/frl_opc_enum_group.h"

namespace frl { namespace opc { namespace impl {

OPCServer::~OPCServer()
{
}

void OPCServer::setServerState( OPCSERVERSTATE newState )
{
	if( newState < OPC_STATUS_RUNNING || newState > OPC_STATUS_TEST )
		FRL_THROW_S_CLASS( InvalidServerState );
	serverStatus.dwServerState = newState;
	CoFileTimeNow( &serverStatus.ftCurrentTime );
}

OPCSERVERSTATE OPCServer::getServerState()
{
	return serverStatus.dwServerState;
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

	new_group->setServerPtr( dynamic_cast< opc::OPCServer*>( this ) );
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

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
