#ifndef frl_opc_browse_server_address_space_h_
#define frl_opc_browse_server_address_space_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_enum_string.h"
#include "opc/frl_opc_flat_data_cache.h"
#include "frl_lock.h"

namespace frl
{
	namespace opc
	{
		template< class T >
		class BrowseServerAddressSpace
			:	public IOPCBrowseServerAddressSpace
		{
		private:
			frl::lock::Mutex scopeGuard;
		public:
			HRESULT STDMETHODCALLTYPE QueryOrganization( 
				/* [out] */ OPCNAMESPACETYPE *pNameSpaceType )
			{
				*pNameSpaceType = OPC_NS_FLAT;
				return S_OK;
			}

			HRESULT STDMETHODCALLTYPE ChangeBrowsePosition( 
				/* [in] */ OPCBROWSEDIRECTION dwBrowseDirection,
				/* [string][in] */ LPCWSTR szString )
			{
				return E_FAIL;
			}

			HRESULT STDMETHODCALLTYPE BrowseOPCItemIDs( 
				/* [in] */ OPCBROWSETYPE dwBrowseFilterType,
				/* [string][in] */ LPCWSTR szFilterCriteria,
				/* [in] */ VARTYPE vtDataTypeFilter,
				/* [in] */ DWORD dwAccessRightsFilter,
				/* [out] */ LPENUMSTRING *ppIEnumString )
			{
				// validate browse filters.
				if (dwBrowseFilterType < OPC_BRANCH || dwBrowseFilterType > OPC_FLAT)
				{
					return E_INVALIDARG;
				}

				// validate access rights.
				if ((dwAccessRightsFilter & 0xFFFFFFFC) != 0)
				{
					return E_INVALIDARG;
				}
				
				if( ppIEnumString == NULL )
					return E_POINTER;
				*ppIEnumString = NULL;

				frl::lock::Mutex::ScopeGuard guard( scopeGuard );
				EnumString *pEnum = new EnumString();
				if( pEnum == NULL )
					return E_OUTOFMEMORY;
				std::vector<String> items;
				flatDataCache.Browse( items );
				pEnum->init( items );
				HRESULT hResult = pEnum->QueryInterface( IID_IEnumString, (void**) ppIEnumString );
				if( FAILED( hResult ) )
					delete pEnum;
				return hResult;
			}

			HRESULT STDMETHODCALLTYPE GetItemID( 
				/* [in] */ LPWSTR szItemDataID,
				/* [string][out] */ LPWSTR *szItemID )
			{
				if( szItemDataID == NULL || szItemID == NULL )
					return E_INVALIDARG;
				*szItemID = NULL;
				frl::lock::Mutex::ScopeGuard guard( scopeGuard );
				if( ! opc::flatDataCache.isExistItem( szItemDataID ) )
					return E_INVALIDARG;
				*szItemID = szItemDataID;
				return S_OK;
			}

			HRESULT STDMETHODCALLTYPE BrowseAccessPaths( 
				/* [string][in] */ LPCWSTR szItemID,
				/* [out] */ LPENUMSTRING *ppIEnumString )
			{
				return E_NOTIMPL;
			}
		};
	}
}

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_browse_server_address_space_h_ */
