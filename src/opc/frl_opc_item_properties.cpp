#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_item_properties.h"

namespace frl
{
namespace opc
{
HRESULT STDMETHODCALLTYPE ItemProperties::QueryAvailableProperties(
	/* [in] */ LPWSTR szItemID,
	/* [out] */ DWORD *pdwCount,
	/* [size_is][size_is][out] */ DWORD **ppPropertyIDs,
	/* [size_is][size_is][out] */ LPWSTR **ppDescriptions,
	/* [size_is][size_is][out] */ VARTYPE **ppvtDataTypes )
{
	if (
		szItemID == NULL ||
		pdwCount == NULL ||
		ppPropertyIDs == NULL ||
		ppDescriptions == NULL ||
		ppvtDataTypes == NULL
		)
	{
		return E_INVALIDARG;
	}

	*pdwCount = 0;
	*ppPropertyIDs = NULL;
	*ppDescriptions = NULL;
	*ppvtDataTypes = NULL;

	if( wcslen( szItemID ) == 0 )
		return OPC_E_INVALIDITEMID;

	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
	String itemID = szItemID;
	#else
	String itemID = wstring2string( szItemID );
	#endif

	if( opcAddressSpace::getInstance().isExistBranch( itemID ) )
		return S_OK;

	address_space::Tag *item = NULL;
	try
	{
		item = opcAddressSpace::getInstance().getTag( itemID );
	}
	catch( address_space::Tag::NotExistTag& )
	{
		return OPC_E_UNKNOWNITEMID;
	}	

	std::vector< DWORD > propArray = item->getAvailableProperties();
	*pdwCount = (DWORD) propArray.size();

	*ppPropertyIDs = os::win32::com::allocMemory<DWORD>( *pdwCount );
	if( *ppPropertyIDs == NULL )
		return E_OUTOFMEMORY;
	os::win32::com::zeroMemory<DWORD>( *ppPropertyIDs, *pdwCount );

	*ppDescriptions = os::win32::com::allocMemory< LPWSTR >( *pdwCount );
	if( *ppDescriptions == NULL )
	{
		os::win32::com::freeMemory( *ppPropertyIDs );
		return E_OUTOFMEMORY;
	}
	os::win32::com::zeroMemory< LPWSTR >( *ppDescriptions, *pdwCount );

	*ppvtDataTypes = os::win32::com::allocMemory< VARTYPE >( *pdwCount );
	if( *ppvtDataTypes == NULL )
	{
		os::win32::com::freeMemory( *ppPropertyIDs );
		os::win32::com::freeMemory( *ppDescriptions );
		return E_OUTOFMEMORY;
	}
	os::win32::com::zeroMemory< VARTYPE >( *ppvtDataTypes, *pdwCount );

	size_t arrSize = propArray.size();
	for( size_t i = 0; i < arrSize; ++i )
	{
		(*ppPropertyIDs)[i] = propArray[i];
		(*ppDescriptions)[i] = util::duplicateString( util::getPropertyDesc( propArray[i] ) );
		(*ppvtDataTypes)[i] = util::getPropertyType( propArray[i] ); 	
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE ItemProperties::GetItemProperties(
	/* [in] */ LPWSTR szItemID,
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ DWORD *pdwPropertyIDs,
	/* [size_is][size_is][out] */ VARIANT **ppvData,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if ( szItemID == NULL || ppvData == NULL || ppErrors  == NULL )
		return E_INVALIDARG;

	*ppvData  = NULL;
	*ppErrors = NULL;

	if( dwCount == 0 )
		return E_INVALIDARG;

	if( wcslen( szItemID ) == 0 )
		return OPC_E_INVALIDITEMID;

	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		String itemID = szItemID;
	#else
		String itemID = wstring2string( szItemID );
	#endif

	address_space::Tag *item = NULL;
	try
	{
		item = opcAddressSpace::getInstance().getLeaf( itemID );
	}
	catch( frl::opc::address_space::Tag::NotExistTag& )
	{
		return OPC_E_UNKNOWNITEMID;	
	}

	*ppvData = os::win32::com::allocMemory< VARIANT >( dwCount );
	if( *ppvData == NULL )
		return E_OUTOFMEMORY;
	os::win32::com::zeroMemory< VARIANT >( *ppvData, dwCount );

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	if( *ppErrors == NULL )
		return E_OUTOFMEMORY;
	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT res = S_OK;
	for( DWORD i = 0; i < dwCount; ++i )
	{
		(*ppErrors)[i] = item->getPropertyValue( pdwPropertyIDs[i], (*ppvData)[i] );
		if( FAILED( (*ppErrors)[i] ) )
			res = S_FALSE;
	}
	return res;
}

HRESULT STDMETHODCALLTYPE ItemProperties::LookupItemIDs(
	/* [in] */ LPWSTR szItemID,
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ DWORD *pdwPropertyIDs,
	/* [size_is][size_is][string][out] */ LPWSTR **ppszNewItemIDs,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if ( szItemID == NULL || ppszNewItemIDs == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*ppszNewItemIDs = NULL;
	*ppErrors = NULL;

	if (dwCount == 0)
		return E_INVALIDARG;

	if( wcslen( szItemID ) == 0 )
		return OPC_E_INVALIDITEMID;

	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		String itemID = szItemID;
	#else
		String itemID = wstring2string( szItemID );
	#endif

	address_space::Tag *item = NULL;
	try
	{
		item = opcAddressSpace::getInstance().getLeaf( itemID );
	}
	catch( address_space::Tag::NotExistTag& )
	{
		return OPC_E_UNKNOWNITEMID;
	}	

	*ppszNewItemIDs = os::win32::com::allocMemory< LPWSTR >( dwCount );
	if( ppszNewItemIDs == NULL )
		return E_OUTOFMEMORY;
	os::win32::com::zeroMemory< LPWSTR>( *ppszNewItemIDs, dwCount );

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	if( ppErrors == NULL )
		return E_OUTOFMEMORY;
	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT ret = S_OK;
	for( DWORD i = 0; i < dwCount; ++i )
	{
		if( item->isValidProperties( pdwPropertyIDs[i] ) )
		{
			(*ppszNewItemIDs)[i] = util::duplicateString( util::getPropertyDesc( pdwPropertyIDs[i] ) );
			
		}
		else
		{
			(*ppErrors)[i] = OPC_E_INVALID_PID;
			ret = S_FALSE;
		}

		if( pdwPropertyIDs[i] <= OPC_PROPERTY_EU_INFO)
		{
			(*ppErrors)[i] = OPC_E_INVALID_PID;
			ret = S_FALSE;
		}
		else
		{
			(*ppErrors)[i] = S_OK;
		}
	}
	return ret;
}
} // namespace opc
} // FatRat Library

#endif
