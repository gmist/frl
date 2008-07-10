#include "opc/impl/frl_opc_impl_browse_server_address_space.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "frl_string.h"
#include "opc/frl_opc_enum_string.h"
#include "opc/address_space/frl_opc_address_space.h"

namespace frl { namespace opc { namespace impl {

BrowseServerAddressSpace::~BrowseServerAddressSpace()
{
}

HRESULT STDMETHODCALLTYPE
	BrowseServerAddressSpace::QueryOrganization(
	/* [out] */ OPCNAMESPACETYPE *pNameSpaceType )
{
	if( pNameSpaceType == NULL )
		return E_INVALIDARG;
	*pNameSpaceType = OPC_NS_HIERARCHIAL;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE
	BrowseServerAddressSpace::ChangeBrowsePosition(
	/* [in] */ OPCBROWSEDIRECTION dwBrowseDirection,
	/* [string][in] */ LPCWSTR szString )
{
	boost::mutex::scoped_lock guard( bsaScopeGuard );
	switch( dwBrowseDirection )
	{
	case OPC_BROWSE_UP:
		{
			if( ! crawler.goUp() )
				return E_FAIL;
			return S_OK;
		}

	case OPC_BROWSE_DOWN:
		{
			if( szString == NULL || wcslen( szString ) == 0 )
				return E_INVALIDARG;

#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			String inString = szString;
#else
			String inString = wstring2string( szString );
#endif

			try
			{
				crawler.goDown( inString );
			}
			catch( frl::Exception& )
			{
				return E_INVALIDARG;
			}
			return S_OK;
		}

	case OPC_BROWSE_TO:
		{
			if( szString == NULL || wcslen( szString ) == 0 )
			{
				crawler.goToRoot();
				return S_OK;
			}

#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			String inString = szString;
#else
			String inString = wstring2string( szString );
#endif

			try
			{
				crawler.goTo( inString );
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

HRESULT STDMETHODCALLTYPE
	BrowseServerAddressSpace::BrowseOPCItemIDs(
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

	boost::mutex::scoped_lock guard( bsaScopeGuard );
	EnumString *pEnum = new EnumString();
	if( pEnum == NULL )
		return E_OUTOFMEMORY;
	std::vector<String> items;

	switch( dwBrowseFilterType )
	{
	case OPC_LEAF:
		crawler.browseLeafs( items, dwAccessRightsFilter );
		break;

	case OPC_BRANCH:
		crawler.browseBranches( items );
		break;

	case OPC_FLAT:
		// If OPC_FLAT we must returns all leafs from address space.
		opcAddressSpace::getInstance().getAllLeafs( items, dwAccessRightsFilter );
		break;
	}

	// filtration by name
	if( szFilterCriteria != NULL && wcslen( szFilterCriteria ) != 0 )
	{			
#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		String filter = szFilterCriteria;
#else
		String filter = wstring2string( szFilterCriteria );
#endif
		std::vector< String > filtredItems;
		filtredItems.reserve( items.size() );
		std::vector< String >::iterator end = items.end();
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

HRESULT STDMETHODCALLTYPE
	BrowseServerAddressSpace::GetItemID(
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

	boost::mutex::scoped_lock guard( bsaScopeGuard );

	if( itemDataID.empty() )
	{
		String tmp = crawler.getCurPosPath();
		if( tmp.empty() ) // is root item
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

	if( crawler.getCurPosPath().size() )
	{
		if( ! opcAddressSpace::getInstance().isExistTag( crawler.getCurPosPath() + opcAddressSpace::getInstance().getDelimiter() + itemDataID) )
			return E_INVALIDARG;
		tag = opcAddressSpace::getInstance().getTag( crawler.getCurPosPath() + opcAddressSpace::getInstance().getDelimiter() + itemDataID );
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

HRESULT STDMETHODCALLTYPE 
	BrowseServerAddressSpace::BrowseAccessPaths(
	/* [string][in] */ LPCWSTR szItemID,
	/* [out] */ LPENUMSTRING *ppIEnumString )
{
	return E_NOTIMPL;
}

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
