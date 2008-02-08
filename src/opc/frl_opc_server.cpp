#include "frl_platform.h"
#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include "sys/frl_sys_util.h"
#include "opc/frl_opc_server.h"
#include "opc/frl_opc_enum_group.h"
#include "frl_exception.h"
#include "opc/frl_opc_server_factory.h"

namespace frl
{
	namespace opc
	{	
		OPCServer::OPCServer()
			: refCount( 0 )
		{
			// call opcAddressSpace.finalConstruct first !
			if( ! opcAddressSpace.isInit() )
				FRL_THROW_S_CLASS( address_space::AddressSpace::NotFinalConstruct );

			os::win32::com::zeroMemory<OPCSERVERSTATUS>( &serverStatus );
			CoFileTimeNow( &serverStatus.ftStartTime );
			serverStatus.szVendorInfo = L"Serg Baburin";
			serverStatus.dwServerState = OPC_STATUS_NOCONFIG;
			serverStatus.dwBandWidth = 0xFFFFFFFF;
			serverStatus.wMajorVersion = 2;
			registerInterface(IID_IOPCShutdown);
			factory.LockServer( TRUE );
			factory.usageServer();
		}

		OPCServer::~OPCServer()
		{
			for( std::map< OPCHANDLE, frl::opc::Group* >::iterator it = groupItem.begin(); it != groupItem.end(); ++it )
				delete (*it).second;
			factory.LockServer( FALSE );
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

			*phServerGroup = NULL;
			*pRevisedUpdateRate = 0;
			*ppUnk = NULL;

			lock::ScopeGuard guard( scopeGuard );
			if( groupItemIndex.find( szName ) != groupItemIndex.end() )
				return OPC_E_DUPLICATENAME;
			if( pPercentDeadband )
				if( *pPercentDeadband < 0 || *pPercentDeadband > 100 ) 
					return E_INVALIDARG;
			String groupName;
			if( szName == NULL || ((String)szName).empty() )
			{
				do
				{
					groupName = util::getUniqueName();
				} while ( groupItemIndex.find( groupName ) != groupItemIndex.end() );
			}
			else
				groupName = szName;
			Group *newGroup = new Group( groupName );
			
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
			groupItemIndex.insert( std::pair< String, OPCHANDLE >( groupName, newGroup->getServerHandle() ) );
			groupItem.insert( std::pair< OPCHANDLE, Group* >(newGroup->getServerHandle(), newGroup ) );
			if(phServerGroup)
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

			if( szName == NULL || String(szName).empty() )
				return E_INVALIDARG;
			
			lock::ScopeGuard guard( scopeGuard );

			std::map< String, OPCHANDLE >::iterator it = groupItemIndex.find( szName );
			if(it == groupItemIndex.end() )
				return E_INVALIDARG;

			OPCHANDLE handle = (*it).second;
			std::map< OPCHANDLE, Group*>::iterator it1 = groupItem.find( handle );
			if( it1 == groupItem.end())
				return E_FAIL;

			Group *grpItem = (*it1).second;

			HRESULT hr = grpItem->QueryInterface( riid, (void**)ppUnk );
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

			lock::ScopeGuard guard( scopeGuard );
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
			lock::ScopeGuard guard( scopeGuard );

			std::map< OPCHANDLE, Group*>::iterator it = groupItem.find( hServerGroup );
			if( it == groupItem.end() )
				return E_FAIL;

			Group *grpItem = (*it).second;
			groupItemIndex.erase( grpItem->getName() );
			groupItem.erase( hServerGroup );
			grpItem->isDeleted( True );
			if( grpItem->Release() != 0 && ! bForce )
			{
				return OPC_S_INUSE;
			}
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE OPCServer::CreateGroupEnumerator( /* [in] */ OPCENUMSCOPE dwScope, /* [in] */ REFIID riid, /* [iid_is][out] */ LPUNKNOWN *ppUnk )
		{
			lock::ScopeGuard guard( scopeGuard );

			if( riid == IID_IEnumUnknown )
			{
				std::vector< Group* > unkn;
				if( dwScope != OPC_ENUM_PUBLIC && dwScope != OPC_ENUM_PUBLIC_CONNECTIONS )
				{
					unkn.reserve( groupItem.size() );
					for( std::map< OPCHANDLE, frl::opc::Group* >::iterator it = groupItem.begin(); it != groupItem.end(); ++it )
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
					for( std::map< OPCHANDLE, frl::opc::Group* >::iterator it = groupItem.begin(); it != groupItem.end(); ++it )
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

		frl::Bool OPCServer::setGroupName( const String &oldName, const String &newName )
		{
			lock::ScopeGuard guard( scopeGuard );
			std::map< String, OPCHANDLE >::iterator it = groupItemIndex.find( oldName );
			if( it == groupItemIndex.end() )
				return False;
			OPCHANDLE tmpHandle = (*it).second;
			groupItemIndex.erase( it );
			groupItemIndex.insert( std::pair< String, OPCHANDLE>( newName, tmpHandle) );
			return True;
		}

		HRESULT OPCServer::cloneGroup( const String &name, const String &cloneName, Group **ppClone )
		{
			if ( ppClone == NULL )
				return E_INVALIDARG;

			lock::ScopeGuard guard( scopeGuard );

			std::map< String, OPCHANDLE >::iterator itInd = groupItemIndex.find( cloneName );
			if( itInd != groupItemIndex.end() )
				return OPC_E_DUPLICATENAME;
			
			itInd = groupItemIndex.find( name );
			if( itInd == groupItemIndex.end() )
				return E_INVALIDARG;

			std::map< OPCHANDLE, Group* >::iterator it = groupItem.find( (*itInd).second );
			if( it == groupItem.end() )
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

			OPCHANDLE handle = (*group)->getServerHandle();
			groupItemIndex.insert( std::pair< String, OPCHANDLE >( (*group)->getName(), handle ) );
			groupItem.insert( std::pair< OPCHANDLE, Group* >( handle, (*group ) ) );
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
	} // namespace opc
} // namespace FatRat Library
#endif // FRL_PLATFORM_WIN32
