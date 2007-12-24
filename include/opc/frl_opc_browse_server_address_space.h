#ifndef frl_opc_browse_server_address_space_h_
#define frl_opc_browse_server_address_space_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_enum_string.h"
#include "opc/address_space/frl_opc_address_space.h"
#include "frl_lock.h"
#include "frl_string.h"

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
				if( pNameSpaceType == NULL )
					return E_INVALIDARG;
				*pNameSpaceType = OPC_NS_HIERARCHIAL;
				return S_OK;
			}

			HRESULT STDMETHODCALLTYPE ChangeBrowsePosition( 
				/* [in] */ OPCBROWSEDIRECTION dwBrowseDirection,
				/* [string][in] */ LPCWSTR szString )
			{
				frl::lock::Mutex::ScopeGuard guard( scopeGuard );
				switch( dwBrowseDirection )
				{
					case OPC_BROWSE_UP:
					{
						if( ! opcAddressSpace.goUp() )
							return E_FAIL;
						return S_OK;
					}

					case OPC_BROWSE_DOWN:
					{
						if( szString == NULL || stringLength( szString ) == 0 )
							return E_INVALIDARG;
						try
						{
							opcAddressSpace.goDown( szString );
						}
						catch( ... )
						{
							return E_INVALIDARG;
						}
						return S_OK;
					}

					case OPC_BROWSE_TO:
					{
						if( szString == NULL || stringLength( szString ) == 0 )
						{
							opcAddressSpace.goToRoot();
							return S_OK;
						}
						try
						{
							opcAddressSpace.goTo( szString );
						}
						catch( ... )
						{
							return E_INVALIDARG;
						}
						return S_OK;
					}
				}
				return E_INVALIDARG;
			}

			HRESULT STDMETHODCALLTYPE BrowseOPCItemIDs( 
				/* [in] */ OPCBROWSETYPE dwBrowseFilterType,
				/* [string][in] */ LPCWSTR szFilterCriteria,
				/* [in] */ VARTYPE vtDataTypeFilter,
				/* [in] */ DWORD dwAccessRightsFilter,
				/* [out] */ LPENUMSTRING *ppIEnumString )
			{
				// validate browse filters.
				if( dwBrowseFilterType < OPC_BRANCH || dwBrowseFilterType > OPC_FLAT )
					return E_INVALIDARG;

				// validate access rights.
				if( (dwAccessRightsFilter & 0xFFFFFFFC) != 0 )
					return E_INVALIDARG;
				
				if( ppIEnumString == NULL )
					return E_POINTER;
				*ppIEnumString = NULL;

				frl::lock::Mutex::ScopeGuard guard( scopeGuard );
				EnumString *pEnum = new EnumString();
				if( pEnum == NULL )
					return E_OUTOFMEMORY;
				std::vector<String> items;

				switch( dwBrowseFilterType )
				{
					case OPC_LEAF:
						opcAddressSpace.browseLeafs( items, dwAccessRightsFilter );
					break;

					case OPC_BRANCH:
						opcAddressSpace.browseBranches( items );
					break;

					case OPC_FLAT:
						opcAddressSpace.browseLeafs( items, dwAccessRightsFilter );
					break;
				}

				// filtration by name
				if( szFilterCriteria != NULL && wcslen( szFilterCriteria ) != 0 )
				{
					std::vector< String > filtredItems;
					for( std::vector< String >::iterator it = items.begin(); it != items.end(); ++it )
					{
						if( util::matchStringPattern( (*it), szFilterCriteria ) )
							filtredItems.push_back( (*it) );
					}
					items.erase( items.begin(), items.end() );
					items = filtredItems;
				}
				
				if( items.size() )
					pEnum->init( items );

				HRESULT hResult = pEnum->QueryInterface( IID_IEnumString, (void**) ppIEnumString );
				if( FAILED( hResult ) )
				{
					delete pEnum;
					return hResult;
				}
				return items.size() ? S_OK : S_FALSE;
			}

			HRESULT STDMETHODCALLTYPE GetItemID( 
				/* [in] */ LPWSTR szItemDataID,
				/* [string][out] */ LPWSTR *szItemID )
			{
				if( szItemDataID == NULL || szItemID == NULL )
					return E_INVALIDARG;
				*szItemID = NULL;

				// is root item
				if( stringLength( szItemDataID ) == 0 )
				{
					*szItemID = szItemDataID;
					return S_OK;
				}

				frl::lock::Mutex::ScopeGuard guard( scopeGuard );
				address_space::Tag *tag;
				if( opcAddressSpace.getCurPosPath().size() )
				{
					if( ! opcAddressSpace.isExistTag( opcAddressSpace.getCurPosPath() + opcAddressSpace.getDelimiter() + szItemDataID) )
						return E_INVALIDARG;
					tag = opcAddressSpace.getTag( opcAddressSpace.getCurPosPath() + opcAddressSpace.getDelimiter() + szItemDataID );
				}
				else
				{
					if( ! opcAddressSpace.isExistTag( szItemDataID) )
						return E_INVALIDARG;
					tag = opcAddressSpace.getTag( szItemDataID );
				}
				*szItemID = util::duplicateString( tag->getID() );
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
