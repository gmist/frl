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
	frl::lock::Mutex bsaScopeGuard;
public:

	virtual ~BrowseServerAddressSpace(){}

	virtual HRESULT STDMETHODCALLTYPE QueryOrganization( 
		/* [out] */ OPCNAMESPACETYPE *pNameSpaceType )
	{
		if( pNameSpaceType == NULL )
			return E_INVALIDARG;
		*pNameSpaceType = OPC_NS_HIERARCHIAL;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE ChangeBrowsePosition( 
		/* [in] */ OPCBROWSEDIRECTION dwBrowseDirection,
		/* [string][in] */ LPCWSTR szString )
	{
		frl::lock::ScopeGuard guard( bsaScopeGuard );
		T* pT = static_cast<T*> (this);
		switch( dwBrowseDirection )
		{
			case OPC_BROWSE_UP:
			{
				if( ! pT->crawler.goUp() )
					return E_FAIL;
				return S_OK;
			}

			case OPC_BROWSE_DOWN:
			{
				if( szString == NULL )
					return E_INVALIDARG;
				
				#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
				String inString = szString;
				#else
				String inString = wstring2string( szString );
				#endif
				
				if( inString.empty() )
					return E_INVALIDARG;

				try
				{
					pT->crawler.goDown( inString );
				}
				catch( frl::Exception& )
				{
					return E_INVALIDARG;
				}
				return S_OK;
			}

			case OPC_BROWSE_TO:
			{
				if( szString == NULL )
				{
					pT->crawler.goToRoot();
					return S_OK;
				}
					
				#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
				String inString = szString;
				#else
				String inString = wstring2string( szString );
				#endif

				if( inString.empty() )
				{
					pT->crawler.goToRoot();
					return S_OK;
				}
				try
				{
					pT->crawler.goTo( inString );
				}
				catch( frl::Exception& )
				{
					return E_INVALIDARG;
				}
				return S_OK;
			}
		}
		return E_INVALIDARG;
	}

	virtual HRESULT STDMETHODCALLTYPE BrowseOPCItemIDs( 
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

		frl::lock::ScopeGuard guard( bsaScopeGuard );
		T* pT = static_cast<T*> (this);
		EnumString *pEnum = new EnumString();
		if( pEnum == NULL )
			return E_OUTOFMEMORY;
		std::vector<String> items;

		switch( dwBrowseFilterType )
		{
			case OPC_LEAF:
				pT->crawler.browseLeafs( items, dwAccessRightsFilter );
			break;

			case OPC_BRANCH:
				pT->crawler.browseBranches( items );
			break;

			case OPC_FLAT:
				// If OPC_FLAT we must returns all leafs from address space.
				opcAddressSpace::getInstance().getAllLeafs( items, dwAccessRightsFilter );
			break;
		}

		// filtration by name
		if( szFilterCriteria != NULL && wcslen( szFilterCriteria ) != 0 )
		{
			std::vector< String > filtredItems;
			filtredItems.reserve( items.size() );
			std::vector< String >::iterator end = items.end();
			
			#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			String filter = szFilterCriteria;
			#else
			String filter = wstring2string( szFilterCriteria );
			#endif
			for( std::vector< String >::iterator it = items.begin(); it != end; ++it )
			{
				if( util::matchStringPattern( (*it), filter ) )
					filtredItems.push_back( (*it) );
			}
			items.swap( filtredItems );
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

	virtual HRESULT STDMETHODCALLTYPE GetItemID( 
		/* [in] */ LPWSTR szItemDataID,
		/* [string][out] */ LPWSTR *szItemID )
	{
		if( szItemDataID == NULL || szItemID == NULL )
			return E_INVALIDARG;

		*szItemID = NULL;

		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		String itemDataID = szItemDataID;
		#else
		String itemDataID = wstring2string( szItemDataID );
		#endif

		frl::lock::ScopeGuard guard( bsaScopeGuard );
		T* pT = static_cast<T*> (this);

		if( itemDataID.empty() )
		{
			String tmp = pT->crawler.getCurPosPath();
			if( ! tmp.size() ) // is root item
			{
				*szItemID = szItemDataID;
			}
			else
			{
				#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
				*szItemID = util::duplicateString( tmp ); // return current position
				#else
				*szItemID = util::duplicateString( string2wstring( tmp ) ); // return current position
				#endif
			}
			return S_OK;
		}

		address_space::Tag *tag;

		if( pT->crawler.getCurPosPath().size() )
		{
			if( ! opcAddressSpace::getInstance().isExistTag( pT->crawler.getCurPosPath() + opcAddressSpace::getInstance().getDelimiter() + itemDataID) )
				return E_INVALIDARG;
			tag = opcAddressSpace::getInstance().getTag( pT->crawler.getCurPosPath() + opcAddressSpace::getInstance().getDelimiter() + itemDataID );
		}
		else
		{
			if( ! opcAddressSpace::getInstance().isExistTag( itemDataID) )
				return E_INVALIDARG;
			tag = opcAddressSpace::getInstance().getTag( itemDataID );
		}
		
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		*szItemID = util::duplicateString( tag->getID() );
		#else
		*szItemID = util::duplicateString( string2wstring(tag->getID()) );
		#endif

		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE BrowseAccessPaths( 
		/* [string][in] */ LPCWSTR szItemID,
		/* [out] */ LPENUMSTRING *ppIEnumString )
	{
		return E_NOTIMPL;
	}
};
}
}

#endif // FRL_PLATFORM_WIN32
#endif /* frl_opc_browse_server_address_space_h_ */
