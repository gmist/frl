#include "frl_platform.h"
#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include "sys/frl_sys_util.h"
#include "opc/frl_opc_server.h"
#include "opc/frl_opc_enum_group.h"

namespace frl
{
	namespace opc
	{	
		OPCServer::OPCServer()
		{
			util::zeroMemory<OPCSERVERSTATUS>( &m_ServerStatus );
			CoFileTimeNow( &m_ServerStatus.ftStartTime );
			m_ServerStatus.szVendorInfo = L"Serg Baburin";
			m_ServerStatus.dwServerState = OPC_STATUS_NOCONFIG;
			m_ServerStatus.dwBandWidth = 0xFFFFFFFF;
			m_ServerStatus.wMajorVersion = 2;
		}

		HRESULT STDMETHODCALLTYPE OPCServer::AddGroup( /* [string][in] */ LPCWSTR szName, /* [in] */ BOOL bActive, /* [in] */ DWORD dwRequestedUpdateRate, /* [in] */ OPCHANDLE hClientGroup, /* [in][unique] */ LONG *pTimeBias, /* [in][unique] */ FLOAT *pPercentDeadband, /* [in] */ DWORD dwLCID, /* [out] */ OPCHANDLE *phServerGroup, /* [out] */ DWORD *pRevisedUpdateRate, /* [in] */ REFIID riid, /* [iid_is][out] */ LPUNKNOWN *ppUnk )
		{

			if (phServerGroup == NULL || pRevisedUpdateRate == NULL || ppUnk == NULL)
				return E_INVALIDARG;

			*phServerGroup = NULL;
			*pRevisedUpdateRate = 0;
			*ppUnk = NULL;

			lock::Mutex::ScopeGuard guard( scopeGuard );
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
			res = newGroup->QueryInterface( riid, (void**)ppUnk );
			if( FAILED( res ) || *ppUnk == NULL) 
			{
				delete newGroup;
				return E_NOINTERFACE;
			}
			newGroup->setServerPtr( this );
			newGroup->setServerHandle( util::getUniqueServerHandle() );
			groupItemIndex.insert( std::pair< String, OPCHANDLE >( groupName, newGroup->getServerHandle() ) );
			groupItem.insert( std::pair< OPCHANDLE, Group* >(newGroup->getServerHandle(), newGroup ) );
			if(phServerGroup)
				*phServerGroup = newGroup->getServerHandle();
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE OPCServer::GetErrorString( /* [in] */ HRESULT dwError, /* [in] */ LCID dwLocale, /* [string][out] */ LPWSTR *ppString )
		{
			if (ppString == NULL)
				return E_INVALIDARG;

			*ppString = NULL;
			LANGID langID;
			switch (dwLocale)
			{
			case LOCALE_SYSTEM_DEFAULT:
				{
					langID = GetSystemDefaultLangID();
					break;
				}
			case LOCALE_USER_DEFAULT:
				{
					langID = GetUserDefaultLangID();
					break;
				}
			case LOCALE_INVARIANT:
				{
					langID = LANGIDFROMLCID(LOCALE_NEUTRAL);
					break;
				}
			default:
				langID = LANGIDFROMLCID(LOCALE_NEUTRAL);
			}

			String cMsg = sys::util::getCodeErrorDescription( langID, dwError );
			if( cMsg.size() == 0 )
				return E_INVALIDARG;

			size_t size = ( cMsg.size() + 1 ) * sizeof(WCHAR);
			*ppString = (LPWSTR)CoTaskMemAlloc( size );
			if( ppString == NULL )
				return E_OUTOFMEMORY;
			wcscpy_s( *ppString, size/sizeof(WCHAR), cMsg.c_str() );
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE OPCServer::GetGroupByName( /* [string][in] */ LPCWSTR szName, /* [in] */ REFIID riid, /* [iid_is][out] */ LPUNKNOWN *ppUnk )
		{
			if( ppUnk == NULL )
				return E_INVALIDARG;
			*ppUnk = NULL;

			if( szName == NULL || String(szName).empty() )
				return E_INVALIDARG;
			
			lock::Mutex::ScopeGuard guard( scopeGuard );

			std::map< String, OPCHANDLE >::iterator it = groupItemIndex.find( szName );
			if(it == groupItemIndex.end() )
				return E_FAIL;

			OPCHANDLE handle = (*it).second;
			if( groupItem.find( handle ) == groupItem.end())
				return E_FAIL;

			Group *grpItem = groupItem[ handle ];
			if(grpItem == NULL)
				return E_FAIL;

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

			lock::Mutex::ScopeGuard guard( scopeGuard );			
			OPCSERVERSTATUS *stat = util::allocMemory<OPCSERVERSTATUS>();
			memcpy( stat, &m_ServerStatus, sizeof(OPCSERVERSTATUS) );
			CoFileTimeNow(&( stat->ftCurrentTime));

			*ppServerStatus = stat;
			stat->szVendorInfo = util::allocMemory<WCHAR>( (wcslen(m_ServerStatus.szVendorInfo)+1) * sizeof(WCHAR) );
			memcpy( stat->szVendorInfo, m_ServerStatus.szVendorInfo, (wcslen( m_ServerStatus.szVendorInfo) + 1) * sizeof(WCHAR) );
			stat->dwGroupCount = (DWORD) groupItemIndex.size();

			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE OPCServer::RemoveGroup( /* [in] */ OPCHANDLE hServerGroup, /* [in] */ BOOL bForce )
		{
			lock::Mutex::ScopeGuard guard( scopeGuard );

			if( groupItem.find( hServerGroup ) == groupItem.end())
				return E_FAIL;

			HRESULT res = S_OK;
			Group *grpItem = groupItem[ hServerGroup ];
			if( grpItem->getRefCount() > 1 && ! bForce )
				res = OPC_S_INUSE;

			groupItemIndex.erase( grpItem->getName() );
			groupItem.erase( hServerGroup );
			grpItem->isDeleted( True );
			delete grpItem;
			return res;
		}
		HRESULT STDMETHODCALLTYPE OPCServer::CreateGroupEnumerator( /* [in] */ OPCENUMSCOPE dwScope, /* [in] */ REFIID riid, /* [iid_is][out] */ LPUNKNOWN *ppUnk )
		{
			lock::Mutex::ScopeGuard guard( scopeGuard );

			if( riid == IID_IEnumUnknown )
			{
				std::vector< Group*> unkn( groupItem.size() );
				std::map< OPCHANDLE, frl::opc::Group* >::iterator it;
				for( it = groupItem.begin(); it!= groupItem.end(); ++it)
					unkn.push_back( (*it).second );

				if( unkn.size() )
				{
					unkn.push_back( NULL );
					EnumGroup *enumGroup = new EnumGroup( unkn );
					HRESULT result = enumGroup->QueryInterface( riid, (void**)ppUnk );
					if( FAILED(result) )
						delete enumGroup;
					return result;
				}
				return S_FALSE;
			}
			if( riid == IID_IEnumString )
			{
			}
			else
				return E_INVALIDARG;
			return S_OK;
		}

	} // namespace opc
} // namespace FatRat Library
#endif /* FRL_PLATFORM_WIN32 */
