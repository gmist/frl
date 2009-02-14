#include "opc/impl/frl_opc_impl_browse_server_address_space.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <boost/foreach.hpp>
#include "frl_string.h"
#include "opc/frl_opc_enum_string.h"
#include "opc/address_space/frl_opc_address_space.h"

namespace frl { namespace opc { namespace impl {

/*! Dtor */
BrowseServerAddressSpace::~BrowseServerAddressSpace()
{
}

/*!
	Provides a way to determine if the underlying system is inherently flat or hierarchical 
	and how the server may represent the information of the address space to the client.

	\param[out] pNameSpaceType
		Place to put OPCNAMESPACE result which will be OPC_NS_HIERARCHIAL or OPC_NS_FLAT

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory

	\retval E_INVALIDARG
		An argument to the function was invalid.

	\retval S_OK
		The operation succeeded.

	\section BrowseServerAddressSpace_QueryOrganization_Comments Comments
		FLAT and HIERARCHICAL spaces behave somewhat different. 
		If the result is "FLAT" then the client knows that there is no need to pass 
		the BRANCH or LEAF flags to BrowseOPCItemIDs or to call ChangeBrowsePosition 
*/
STDMETHODIMP BrowseServerAddressSpace::QueryOrganization(
	/* [out] */ OPCNAMESPACETYPE *pNameSpaceType )
{
	if( pNameSpaceType == NULL )
		return E_INVALIDARG;
	*pNameSpaceType = OPC_NS_HIERARCHIAL;
	return S_OK;
}

/*!
	Provides a way to move "up" or "down" or "to" in a hierarchical space.

	\param[in] dwBrowseDirection
		OPC_BROWSE_UP or OPC_BROWSE_DOWN or OPC_BROWSE_TO.

	\param[out] szString
		For DOWN, the name of the branch to move into. 
		This would be one of the strings returned from BrowseOPCItemIDs. E.g. REACTOR10\n
		For UP this parameter is ignored and should point to a NUL string.\n
		For TO a fully qualified name (e.g. as returned from GetItemID) or a pointer to a NUL string to go to the "root".
		E.g. AREA1.REACTOR10.TIC1001

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory

	\retval E_INVALIDARG
		An argument to the function was invalid.

	\retval S_OK
		The operation succeeded.

	\section BrowseServerAddressSpace_ChangeBrowsePosition_Comments Comments
		\arg
			The function will return E_FAIL if called for a FLAT space.
		\arg
			An error is returned if the passed string does not represent a "branch".
		\arg
			Moving UP from the "root" will return E_FAIL.
		\arg
			Note OPC_BROWSE_TO is new for version 2.0. 
			Clients should be prepared to handle E_INVALIDARG if they pass this to a 1.0 server.
*/
STDMETHODIMP BrowseServerAddressSpace::ChangeBrowsePosition(
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

/*!
	Returns an IENUMString for a list of ItemIDs as determined by the passed parameters. 
	The position from the which the browse is done can be set via ChangeBrowsePosition.

	\param[in] dwBrowseFilterType
		\arg OPC_BRANCH - returns only items that have children
		\arg OPC_LEAF - returns only items that don't have children
		\arg OPC_FLAT - returns everything at and below this level including all children of children - 
								basically "pretends" that the address space in actually FLAT.
		\arg This parameter is ignored for FLAT address space.

	\param[in] szFilterCriteria
		A server specific filter string. 
		This is entirely free format and may be entered by the user via an EDIT field. 
		Although the valid criteria are vendor specific, source code for a recommended filter 
		function is included in an Apppendix at the end of this document. 
		This particular filter function is commonly used by OPC interfaces and is very similar in functionality to the LIKE function in visual basic. A pointer to a NUL string indicates no filtering.

	\param[in] vtDataTypeFilter
		Filter the returned list based in the available data types 
		(those that would succeed if passed to AddItem). 
		VT_EMPTY indicates no filtering.

	\param[in] dwAccessRightsFilter
		Filter based on the AccessRights bit mask (OPC_READABLE or OPC_WRITEABLE). 
		The bits passed in the bitmask are "ANDed" with the bits that would be returned 
		for this Item by AddItem, ValidateItem or EnumOPCItemAttributes. 
		If the result is non-zero then the item is returned. 
		A 0 value in the bitmask indicates that the AccessRights bits should be ignored during the filtering process..

	\param[out] ppIEnumString
		Where to save the returned interface pointer.  NULL if the HRESULT is other than S_OK or S_FALSE

	\retval S_OK
		The operation succeeded.

	\retval S_FALSE
		There is nothing to enumerate. However an empty Enumerator is still returned and must be released. 
		Note: In previous versions of the spec there has been some ambiguity 
		about the behavior in the case of S_FALSE. 
		For this reason, it is recommended that when S_FALSE is returned by the server, 
		clients test the returned interface pointer for NULL prior to calling Release on it.

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory.

	\retval E_INVALIDARG
		An argument to the function was invalid.

	\retval OPC_E_INVALIDFILTER
		The filter string was not valid.

	\section BrowseServerAddressSpace_BrowseOPCItemIDs_Comments Comments
		\arg
			The returned enumerator may have nothing to enumerate if no ItemIDs satisfied the filter constraints. 
			The strings returned by the enumerator represent the BRANCHs and LEAFS contained in the current level. 
			They do NOT include any delimiters or "parent" names. (See GetItemID()).
		\arg
			Whenever possible the server should return strings which can be passed directly to AddItems. 
			However, it is allowed for the Server to return a "hint" string rather than an actual legal Item ID. 
			For example a PLC with 32000 registers could return a single string of "0 to 31999" rather 
			than return 32,000 individual strings from the enumerator. 
			For this reason (as well as the fact that browser support is optional) 
			clients should always be prepared to allow manual entry of ITEM ID strings. 
			In the case of "hint" strings, there is no indication given as to whether 
			the returned string will be acceptable by AddItem or ValidateItem
		\arg
			Clients are allowed to get and hold Enumerators for more than one "browse position" at a time.
		\arg
			Changing the browse position will not affect any String Enumerator the client already has.
		\arg
			The client must Release each Enumerator when he is done with it.
*/
STDMETHODIMP BrowseServerAddressSpace::BrowseOPCItemIDs(
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
		BOOST_FOREACH( String& el, items )
		{
			if( util::matchStringPattern( el, filter ) )
				filtredItems.push_back( el );
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

/*!
	Provides a way to assemble a "fully qualified" ITEM ID in a hierarchical space. 
	This is required since the browsing functions return only the components or tokens 
	which make up an ITEMID and do NOT return the delimiters used to separate those tokens. 
	Also, at each point one is browsing just the names "below" the current node (e.g. the "units" in a "cell").

	\param[in] szItemDataID
		The name of a BRANCH or LEAF at the current level. or a pointer to a NUL string. 
		Passing in a NUL string results in a return string which represents the current position in the hierarchy.

	\param[out] szItemID
		Where to return the resulting ItemID.

	\retval E_FAIL
		The function failed.

	\retval E_INVALIDARG
		An argument to the function was invalid 
		(e.g. the passed ItemDataID was invalid).

	\retval E_OUTOFMEMORY
		Not enough memory.

	\retval S_OK
		The function was successful.

	\section BrowseServerAddressSpace_GetItemID_Comments Comments
		\arg
			A client would browse down from AREA1 to REACTOR10 to TIC1001 to CURRENT_VALUE. 
			As noted earlier the client sees only the components, not the 
			delimiters which are likely to be very server specific. 
			The function rebuilds the fully qualified name including the vendor specific 
			delimiters for use by ADDITEMs. An extreme example might be a server that returns:
				\\AREA1:REACTOR10.TIC1001[CURRENT_VALUE] 
		\arg
			It is also possible that a server could support hierarchical browsing of an address space that contains globally unique tags. 
			For example in the case above, the tag TIC1001.CURRENT_VALUE might still be globally 
			unique and might therefore be acceptable to AddItem. 
			However the expected behavior is that (a) GetItemID will always return the 
			fully qualified name (AREA1.REACTOR10.TIC1001.CURRENT_VALUE) 
			and that (b) that the server will always accept the fully qualified name in AddItems (even if it does not require it).
		\arg
			This function does not need to be called for a FLAT space. 
			If it is called, then the server must return the same string that was passed in.
		\arg
			It is valid to form an ItemID that represents a BRANCH (e.g. AREA1.REACTOR10). 
			This could happen if you pass a BRANCH (AREA1) rather than a LEAF (CURRENT_VALUE). 
			The resulting string might fail if passed to AddItem but could be passed to ChangeBrowsePosition using OPC_BROWSE_TO.
		\arg
			The client must free the returned string.
		\arg
			ItemID is the unique "key" to the data, it is considered the "what" or "where" that allows the server to connect to the data source.
*/
STDMETHODIMP BrowseServerAddressSpace::GetItemID(
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

/*!
	Provides a way to browse the available AccessPaths for an ITEM ID.

	\param[in] szItemID
		Fully Qualified ItemID

	\param[out] ppIEnumString
		Where to save the returned string enumerator. 
		NULL if the HRESULT is other than S_OK or S_FALSE.

	\retval E_FAIL
		The function failed 

	\retval E_INVALIDARG
		An argument to the function was invalid.

	\retval S_FALSE
		There is nothing to enumerate. However an empty Enumerator is still returned and must be released. Note: In previous versions of the spec there has been some ambiguity about the behavior in the case of S_FALSE. For this reason, it is recommended that when S_FALSE is returned by the server, clients test the returned interface pointer for NULL prior to calling Release on it.

	\retval E_OUTOFMEMORY
		Not enough memory

	\retval lE_NOTIMPL
		The server does not require or support access paths.

	\retval S_OK
		The function was successful

	\section BrowseServerAddressSpace_BrowseAccessPaths_Comments Comments
		\arg
			Clients are allowed to get Access Path Enumerators for more than one item at a time.
		\arg
			Changing the browse position will not affect any enumerator the client already has.
		\arg
			The client must Release each Enumerator when he is done with it.
		\arg
			AccessPath is the "how" for the server to get the data specified by the itemID (the what). 
			The client uses this function to identify the possible access paths for the specified itemID.
*/
STDMETHODIMP BrowseServerAddressSpace::BrowseAccessPaths(
	/* [string][in] */ LPCWSTR szItemID,
	/* [out] */ LPENUMSTRING *ppIEnumString )
{
	return E_NOTIMPL;
}

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
