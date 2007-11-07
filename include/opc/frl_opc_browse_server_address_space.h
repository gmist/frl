#ifndef frl_opc_browse_server_address_space_h_
#define frl_opc_browse_server_address_space_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_enum_string.h"
#include "opc/frl_opc_flat_data_cache.h"

namespace frl
{
	namespace opc
	{
		template< class T >
		class BrowseServerAddressSpace
			:	public IOPCBrowseServerAddressSpace
		{
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
				
				EnumString *pEnum = new EnumString();
				if( pEnum == NULL )
					return E_OUTOFMEMORY;
				std::vector<String> items;
				flatDataCache.Browse( items );
				pEnum->init( items );
				*ppIEnumString = pEnum;
				return S_OK;
			}

			HRESULT STDMETHODCALLTYPE GetItemID( 
				/* [in] */ LPWSTR szItemDataID,
				/* [string][out] */ LPWSTR *szItemID )
			{
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
