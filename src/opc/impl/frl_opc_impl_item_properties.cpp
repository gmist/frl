#include "opc/impl/frl_opc_impl_item_properties.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "opc/address_space/frl_opc_address_space.h"
#include "opc/frl_opc_util.h"
#include "os/win32/com/frl_os_win32_com_variant.h"

namespace frl { namespace opc { namespace impl {

/*! Dtor */
OPCItemProperties::~OPCItemProperties()
{
}

/*!
	Return a list of ID codes and descriptions for the available properties for this ITEMID. 
	This list may differ for different ItemIDs. This list is expected to be relatively stable for a particular ItemID. 
	That is, it could be affected from time to time by changes to the underlying system's configuration. 

	\param[in] szItemID
		The ItemID for which the caller wants to know the available properties.

	\param[out] pdwCount
		The number of properties returned.

	\param[out] ppPropertyIDs
		DWORD IDs for the returned properties. 
		These IDs can be passed to GetItemProperties or LookupItemIDs

	\param[out] ppDescriptions
		A brief vendor supplied text description of each property. 
		NOTE LocaleID does not apply to Descriptions. They are from the tables above.

	\param[out] ppvtDataTypes
		The data type which will be returned for this property by GetItemProperties.

	\retval S_OK
		The function was successful.

	\retval OPC_E_UNKNOWNITEMID
		The ItemID is not in the server address space.

	\retval OPC_E_INVALIDITEMID
		The ItemID is not syntactically valid.

	\retval E_OUTOFMEMORY
		Not enough Memory.

	\retval E_INVALIDARG
		An invalid argument was passed.

	\retval E_FAIL
		The function failed.

	\section OPCItemProperties_QueryAvailableProperties_Comment Comment
		The ItemID is passed to this function because servers 
		are allowed to return different sets of properties for different ItemIDs.
*/
STDMETHODIMP OPCItemProperties::QueryAvailableProperties(
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

	for( DWORD i = 0; i < *pdwCount; ++i )
	{
		(*ppPropertyIDs)[i] = propArray[i];
		(*ppDescriptions)[i] = util::duplicateString( util::getPropertyDesc( propArray[i] ) );
		(*ppvtDataTypes)[i] = util::getPropertyType( propArray[i] ); 	
	}

	return S_OK;
}

/*!
	Return a list of the current data values for the passed ID codes.

	\param[in] szItemID
		The ItemID for which the caller wants to read the list of properties.

	\param[in] dwCount
		The number of properties passed

	\param[in] pdwPropertyIDs
		DWORD IDs for the requested properties. 
		These IDs were returned by QueryAvailableProperties 
		or obtained from the fixed list described earlier.

	\param[out] ppvData
		An array of count VARIANTS returned by the server 
		which contain the current values of the requested properties.

	\param[out] ppErrors
		Error array indicating whether each property was returned.

	\retval S_OK
		The function was successful.

	\retval S_FALSE
		The operation completed with one or more errors. 
		Refer to individual error returns for failure analysis 
		(see \ref OPCItemProperties_GetItemProperties_ppErrors).

	\retval OPC_E_UNKNOWNITEMID
		The ItemID is not in the server address space.

	\retval OPC_E_INVALIDITEMID
		The ItemID is not syntactically valid.

	\retval E_OUTOFMEMORY
		Not enough Memory.

	\retval E_INVALIDARG
		An invalid argument was passed.

	\retval E_FAIL
		The function failed.

	\section OPCItemProperties_GetItemProperties_ppErrors ppErrors codes
		\arg \b S_OK - 
			The corresponding PropertyID was read.
		\arg \b OPC_E_INVALID_PID - 
			The passed Property ID is not defined for this item.
		\arg \b E_xxx - 
			The passed Property ID could not be read. 
			The server can return a server specific error code to provide 
			a detailed explanation as to why this property could not be read. 
			This error code can be passed to GetErrorMessage(). 
			In general this will be the same set of errors as is returned by the OPC Read function.

	\section OPCItemProperties_GetItemProperies_Comments Comments
		\arg
			The caller must Free the returned Variants and Errors array. 
			The client must first do a VariantClear() on each of the returned Variants.
		\arg
			Clients should not use this interface to obtain large amounts of data. 
			Clearly each server vendor will provide the best performance possible however as 
			a practical matter it is expected that the design of this interface will 
			make it difficult for the server to optimize performance. See LookupItemIDs.
*/
STDMETHODIMP OPCItemProperties::GetItemProperties(
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

/*!
	Return a list of ITEMIDs (if available) for each of the passed ID codes. 
	These indicate the ITEMID which could be added to an OPCGroup 
	and used for more efficient access to the data corresponding to the Item Properties.

	\param[in] szItemID
		The ItemID for which the caller wants to lookup the list of properties

	\param[in] dwCount
		The number of properties passed

	\param[in] pdwPropertyIDs
		DWORDIDs for the requested properties. 
		These IDs were returned by QueryAvailableProperties

	\param[out] ppszNewItemIDs
		The returned list of ItemIDs.

	\param[out] ppErrors
		Error array indicating whether each New ItemID was returned.

	\retval S_OK
		The function was successful.

	\retval S_FALSE
		The operation completed with one or more errors. 
		Refer to individual error returns for failure analysis 
		( see \ref OPCItemProperties_LookupItemIDs_ppErrors ).

	\retval OPC_E_UNKNOWNITEMID
		The ItemID is not in the server address space.

	\retval OPC_E_INVALIDITEMID
		The ItemID is not syntactically valid.

	\retval E_OUTOFMEMORY
		Not enough Memory.

	\retval E_INVALIDARG
		An invalid argument was passed.

	\retval E_FAIL
		The function was not successful.

	\section OPCItemProperties_LookupItemIDs_ppErrors ppErrors codes
		\arg \b S_OK
			The corresponding Property ID was translated into an ItemID.
		\arg \b OPC_E_INVALID_PID
			The passed Property ID is not defined for this item.
		\arg \b E_FAIL
			The passed Property ID could not be translated into an ItemID.

	\section OPCItemProperties_LookupItemIDs_Comments Comments
		\arg
			It is expected and recommended that servers will allow most or all item properties to be translated into specific ItemIDs.
		\arg
			The caller must Free the returned NewItemIDs and Errors array.


*/
STDMETHODIMP OPCItemProperties::LookupItemIDs(
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

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
