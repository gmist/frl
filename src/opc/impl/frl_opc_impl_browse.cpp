#include "opc/impl/frl_opc_impl_browse.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "opc/address_space/frl_opc_address_space.h"

namespace frl { namespace opc { namespace impl {

BrowseImpl::~BrowseImpl()
{
}

HRESULT STDMETHODCALLTYPE BrowseImpl::GetProperties(
	/* [in] */ DWORD dwItemCount,
	/* [size_is][string][in] */ LPWSTR *pszItemIDs,
	/* [in] */ BOOL bReturnPropertyValues,
	/* [in] */ DWORD dwPropertyCount,
	/* [size_is][in] */ DWORD *pdwPropertyIDs,
	/* [size_is][size_is][out] */ OPCITEMPROPERTIES **ppItemProperties )
{
	if( pszItemIDs == NULL || ppItemProperties == NULL )
		return E_INVALIDARG;

	*ppItemProperties = NULL;

	if( dwItemCount == 0 )
		return E_INVALIDARG;

	*ppItemProperties = os::win32::com::allocMemory< OPCITEMPROPERTIES >( dwItemCount );
	if( *ppItemProperties == NULL )
		return E_OUTOFMEMORY;
	os::win32::com::zeroMemory< OPCITEMPROPERTIES >( *ppItemProperties, dwItemCount );

	HRESULT ret = S_OK;
	address_space::Tag *item = NULL;
	for( DWORD i = 0; i < dwItemCount; ++i )
	{
		if( pszItemIDs[i] == NULL || wcslen( pszItemIDs[i] ) == 0 )
		{
			(*ppItemProperties)[i].hrErrorID = OPC_E_INVALIDITEMID;
			ret = S_FALSE;
			continue;
		}

		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			String itemID = pszItemIDs[i];
		#else
			String itemID = wstring2string( pszItemIDs[i] );
		#endif

		try
		{
			item = opcAddressSpace::getInstance().getTag( itemID );
		}
		catch( address_space::Tag::NotExistTag& )
		{
			(*ppItemProperties)[i].hrErrorID = OPC_E_UNKNOWNITEMID;
			ret = S_FALSE;
			continue;
		}

		if( dwPropertyCount == 0 )
		{
			std::vector< DWORD > propArray = item->getAvailableProperties();
			size_t arrSize = propArray.size();

			(*ppItemProperties)[i].pItemProperties = os::win32::com::allocMemory<OPCITEMPROPERTY>( arrSize );
			os::win32::com::zeroMemory<OPCITEMPROPERTY>( (*ppItemProperties)[i].pItemProperties, arrSize );

			(*ppItemProperties)[i].dwNumProperties = (DWORD)arrSize;

			for( size_t j = 0; j < arrSize; ++j )
			{
				(*ppItemProperties)[i].pItemProperties[j].dwPropertyID = propArray[j];
				(*ppItemProperties)[i].pItemProperties[j].szDescription = util::duplicateString( util::getPropertyDesc( propArray[j] ) );
				(*ppItemProperties)[i].pItemProperties[j].vtDataType = util::getPropertyType( propArray[j] );
				(*ppItemProperties)[i].pItemProperties[j].szItemID = util::duplicateString( pszItemIDs[i] );
				(*ppItemProperties)[i].pItemProperties[j].hrErrorID = S_OK;
				if( bReturnPropertyValues == TRUE )
				{
					item->getPropertyValue( propArray[j], (*ppItemProperties)[i].pItemProperties[j].vValue );
				}
			}				
		} // if
		else
		{
			(*ppItemProperties)[i].pItemProperties = os::win32::com::allocMemory<OPCITEMPROPERTY>( dwPropertyCount );
			os::win32::com::zeroMemory<OPCITEMPROPERTY>( (*ppItemProperties)[i].pItemProperties, dwPropertyCount );
			(*ppItemProperties)[i].dwNumProperties = dwPropertyCount;
			for( DWORD j = 0; j < dwPropertyCount; ++j )
			{
				if( item->isValidProperties( pdwPropertyIDs[j] ) )
				{
					(*ppItemProperties)[i].pItemProperties[j].dwPropertyID = pdwPropertyIDs[j];
					(*ppItemProperties)[i].pItemProperties[j].szDescription = util::duplicateString( util::getPropertyDesc( pdwPropertyIDs[j] ) );
					(*ppItemProperties)[i].pItemProperties[j].vtDataType = util::getPropertyType( pdwPropertyIDs[j] );
					(*ppItemProperties)[i].pItemProperties[j].szItemID = util::duplicateString( pszItemIDs[i] );
					(*ppItemProperties)[i].pItemProperties[j].hrErrorID = S_OK;
					if( bReturnPropertyValues == TRUE )
					{
						item->getPropertyValue( pdwPropertyIDs[j] , (*ppItemProperties)[i].pItemProperties[j].vValue );
					}
				}
				else
				{
					(*ppItemProperties)[i].pItemProperties[j].hrErrorID = OPC_E_INVALID_PID;
					(*ppItemProperties)[i].hrErrorID = S_FALSE;
					ret = S_FALSE;
				}
			}
		} //else
	} // for

	return ret;
}

HRESULT STDMETHODCALLTYPE BrowseImpl::Browse(
	/* [string][in] */ LPWSTR szItemID,
	/* [string][out][in] */ LPWSTR *pszContinuationPoint,
	/* [in] */ DWORD dwMaxElementsReturned,
	/* [in] */ OPCBROWSEFILTER dwBrowseFilter,
	/* [string][in] */ LPWSTR szElementNameFilter,
	/* [string][in] */ LPWSTR szVendorFilter,
	/* [in] */ BOOL bReturnAllProperties,
	/* [in] */ BOOL bReturnPropertyValues,
	/* [in] */ DWORD dwPropertyCount,
	/* [size_is][in] */ DWORD *pdwPropertyIDs,
	/* [out] */ BOOL *pbMoreElements,
	/* [out] */ DWORD *pdwCount,
	/* [size_is][size_is][out] */ OPCBROWSEELEMENT **ppBrowseElements )
{
	if( pbMoreElements   == NULL || pdwCount == NULL || ppBrowseElements == NULL )
		return E_INVALIDARG;

	if( dwBrowseFilter < OPC_BROWSE_FILTER_ALL || dwBrowseFilter > OPC_BROWSE_FILTER_ITEMS )
		return E_INVALIDARG;

	*pbMoreElements = FALSE; // we supported continuation point
	*pdwCount = 0;
	*ppBrowseElements = NULL;

	if( szItemID == NULL || wcslen( szItemID ) == 0 )
	{
		crawler.goToRoot();
	}
	else
	{
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			String itemID = szItemID;
		#else
			String itemID = wstring2string( szItemID );
		#endif

		try
		{
			crawler.goTo( itemID );
		}
		catch( frl::Exception& )
		{
			return OPC_E_UNKNOWNITEMID;
		}
	}

	String cp;
	if( *pszContinuationPoint != NULL && wcslen( *pszContinuationPoint ) != 0 )
	{
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			cp = *pszContinuationPoint;
		#else
			cp = wstring2string( *pszContinuationPoint );
		#endif
		if( ! opcAddressSpace::getInstance().isExistTag( cp ) )
			return OPC_E_INVALIDCONTINUATIONPOINT;
	}

	std::vector< address_space::TagBrowseInfo > itemsList;
	switch( dwBrowseFilter )
	{
	case OPC_BROWSE_FILTER_BRANCHES:
		crawler.browseBranches( itemsList );
		break;

	case OPC_BROWSE_FILTER_ITEMS:
		crawler.browseLeafs( itemsList );
		break;

	case OPC_BROWSE_FILTER_ALL:
		crawler.browse( itemsList );
		break;
	} // switch

	if( itemsList.empty() )
		return S_OK;

	if( ! cp.empty() )
	{
		Bool flag = False;
		std::vector< address_space::TagBrowseInfo > tmp;
		for( size_t i = 0; i < itemsList.size(); ++i )
		{
			if( itemsList[i].fullID == cp )
				flag = True;
			if( flag )
				tmp.push_back( itemsList[i] );
		}
		if( ! flag )
			return OPC_E_INVALIDCONTINUATIONPOINT;
		itemsList.swap( tmp );
	}

	if( szElementNameFilter != NULL && wcslen( szElementNameFilter ) != 0 )
	{
		std::vector< address_space::TagBrowseInfo > filtredItems;
		filtredItems.reserve( itemsList.size() );
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			String filter = szElementNameFilter;
		#else
			String filter = wstring2string( szElementNameFilter );
		#endif
		std::vector< address_space::TagBrowseInfo >::iterator end = itemsList.end();
		for( std::vector< address_space::TagBrowseInfo >::iterator it = itemsList.begin(); it != end; ++it )
		{
			if( util::matchStringPattern( (*it).shortID, filter ) )
				filtredItems.push_back( (*it) );
		}
		itemsList.swap( filtredItems );
	}

	if( itemsList.empty() )
		return S_FALSE;

	if( dwMaxElementsReturned != 0
		&& ( dwMaxElementsReturned < (DWORD)itemsList.size() ) )
	{
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			*pszContinuationPoint = util::duplicateString( itemsList[dwMaxElementsReturned].fullID );
		#else
			*pszContinuationPoint = util::duplicateString( string2wstring( itemsList[dwMaxElementsReturned].fullID ) );
		#endif

		std::vector< address_space::TagBrowseInfo > tmp( dwMaxElementsReturned );
		tmp.assign( itemsList.begin(), itemsList.begin() + dwMaxElementsReturned );
		itemsList.swap( tmp );			
	}
	else
	{
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			*pszContinuationPoint = util::duplicateString( FRL_STR("") );
		#else
			*pszContinuationPoint = util::duplicateString( string2wstring( FRL_STR("") ) );
		#endif
	}

	size_t size = itemsList.size();
	*pdwCount = (DWORD)size;
	*ppBrowseElements = os::win32::com::allocMemory<OPCBROWSEELEMENT> ( size );
	os::win32::com::zeroMemory<OPCBROWSEELEMENT>( *ppBrowseElements, size );

	for( size_t i = 0; i < size; ++i )
	{
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			(*ppBrowseElements)[i].szName = util::duplicateString( itemsList[i].shortID );
			(*ppBrowseElements)[i].szItemID = util::duplicateString( itemsList[i].fullID );
		#else
			(*ppBrowseElements)[i].szName = util::duplicateString( string2wstring( itemsList[i].shortID ) );
			(*ppBrowseElements)[i].szItemID = util::duplicateString( string2wstring( itemsList[i].fullID ) );
		#endif

		if( itemsList[i].isLeaf )
			(*ppBrowseElements)[i].dwFlagValue = OPC_BROWSE_ISITEM;
		else
			(*ppBrowseElements)[i].dwFlagValue = OPC_BROWSE_HASCHILDREN;
	}

	HRESULT ret = S_OK;
	if( bReturnAllProperties || dwPropertyCount > 0 )
	{
		LPWSTR* szItemIDs = os::win32::com::allocMemory< LPWSTR >( size );
		for( size_t i = 0; i < size; ++i )
		{
			szItemIDs[i] = (*ppBrowseElements)[i].szItemID;
		}

		OPCITEMPROPERTIES* pProperties = NULL;
		HRESULT prop_result = GetProperties(
			(DWORD)size,
			szItemIDs,
			bReturnPropertyValues,
			dwPropertyCount,
			pdwPropertyIDs,
			&pProperties
			);

		os::win32::com::freeMemory( szItemIDs );

		for( size_t i = 0; i < size; ++i )
		{
			if( ((*ppBrowseElements)[i].dwFlagValue & OPC_BROWSE_ISITEM) == 0 )
				continue;

			if( FAILED( prop_result ) )
			{
				(*ppBrowseElements)[i].ItemProperties.hrErrorID = prop_result;
				ret = S_FALSE;
				continue;
			}

			(*ppBrowseElements)[i].ItemProperties = pProperties[i];
			if( pProperties[i].hrErrorID == S_FALSE || FAILED( pProperties[i].hrErrorID ) )
				ret = S_FALSE;
		}
		os::win32::com::freeMemory(pProperties);
	}
	return ret;
}

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
