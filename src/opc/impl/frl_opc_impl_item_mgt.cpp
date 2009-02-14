#include "opc/impl/frl_opc_impl_item_mgt.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "opc/impl/frl_opc_enum_item_attributes.h"
#include "opc/address_space/frl_opc_address_space.h"
#include "opc/frl_opc_async_request.h"
#include "opc/frl_opc_group_base.h"
#include "opc/frl_opc_server.h"

namespace frl { namespace opc { namespace impl {

/*! Dtor */
ItemMgt::~ItemMgt()
{
}

/*!
	Add one or more items to a group.
	
	\param dwCount
		The number of items to be added.

	\param pItemArray
		Array of OPCITEMDEFs. 
		These tell the server everything it needs to know about the item
		 including the access path, definition and requested data type.

	\param ppAddResults
		Array of OPCITEMRESULTs. 
		This tells the client additional information about the item including 
		the server assigned item handle and the canonical data type.

	\param ppErrors
		Array of HRESULTs. This tells the client which of the items was 
		successfully added. For any item  which failed it provides a reason.

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory.

	\retval E_INVALIDARG
		An argument to the function was invalid
		(e.g. dwCount = 0 )

	\retval S_OK
		The operation succeeded.

	\retval S_FALSE
		The operation comleted with one or more errors.
		Refer to individual errors returns for failure analysis.

	\retval OPC_E_PUBLIC
		Cannot add items to a public group.

	\section ppErrorsCodes
		\arg \b S_OK - The function was successful for this item.
		\arg \b OPC_E_INVALIDITEMID - The ItemID is not syntactically valid.
		\arg \b OPC_E_UNKNOWNITEMID - The ItemID is not in the server address space.
		\arg \b OPC_E_BADTYPE - The requested data type cannot be returned for this item 
		(See \ref ItemMgt_AddItems_Comments "Comments" section).
		\arg \b E_FAIL - The function was unsuccessful.
		\arg \b OPC_E_UNKNOWNPATH - The item's access path is not known to the server.

	\section ItemMgt_AddItems_Comments Comments
		\arg It is acceptable to add the same item to the group more than once. 
		This will generate a 2nd item with a unique ServerHandle.

		\arg
		Any FAILED code in ppErrors indicates that the corresponding item was NOT added to the group
		 and that the corresponding OPCITEMRESULT will not contain useful information.

		\arg
		As an alternative to OPC_E_BADTPYE it is acceptable for the server to return 
		any FAILED error returned by VariantChangeType or VariantChangeTypeEx.

		\arg
		The server provided item handle will be unique within the group, but may not be unique across groups. 
		The server is allowed to 'reuse' the handles of deleted items.

		\arg
		Items cannot be added to public groups.

		\arg
		The client needs to free all of the memory associated with the OPCITEMRESULTs including the BLOB.

		\arg
		If the server supports the BLOB it will return an updated BLOB in the OPCITEMRESULTs. 
		This BLOB may differ in both content and size from the one passed by the client in OPCITEMDEF.

		\arg
		Note that if an Advise is active, the client will begin receiving callbacks for active items. 
		This can occur very quickly, perhaps even before the client has time to process the returned results. 
		The client must be designed to deal with this. 
		One simple solution is for the client to clear the Active state of the group while doing AddItems 
		and to restore it after the AddItems is completed and the results are processed.
*/
STDMETHODIMP ItemMgt::AddItems(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCITEMDEF *pItemArray,
	/* [size_is][size_is][out] */ OPCITEMRESULT **ppAddResults,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if( pItemArray == NULL || ppAddResults == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*ppAddResults = NULL;
	*ppErrors = NULL;

	if( dwCount == 0 ) 
		return E_INVALIDARG;

	*ppAddResults = os::win32::com::allocMemory<OPCITEMRESULT>( dwCount );
	if( *ppAddResults == NULL )
		return E_OUTOFMEMORY;
	os::win32::com::zeroMemory<OPCITEMRESULT>( *ppAddResults, dwCount );

	*ppErrors = os::win32::com::allocMemory<HRESULT>( dwCount );
	if( *ppErrors == NULL ) 
	{ 
		os::win32::com::freeMemory(*ppAddResults);
		return E_OUTOFMEMORY;
	}

	HRESULT res = S_OK;
	boost::mutex::scoped_lock guard( groupGuard );
	for( DWORD i = 0; i < dwCount; ++i )
	{
		if( pItemArray[i].szItemID == NULL || wcslen( pItemArray[i].szItemID ) == 0 )
		{
			(*ppErrors)[i] = OPC_E_INVALIDITEMID;
			res = S_FALSE;
			continue;
		}

		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			String itemID = pItemArray[i].szItemID;
		#else
			String itemID = wstring2string( pItemArray[i].szItemID );
		#endif

		if( ! opcAddressSpace::getInstance().isExistLeaf( itemID ) )
		{
			(*ppErrors)[i] = OPC_E_UNKNOWNITEMID;
			res = S_FALSE;
			continue;
		}

		if( ! os::win32::com::isValidType( pItemArray[i].vtRequestedDataType ) )
		{
			(*ppErrors)[i] = OPC_E_BADTYPE;
			res = S_FALSE;
			continue;
		}

		GroupItemElem item( new GroupItem() );
		item->Init( pItemArray[i] );
		(*ppAddResults)[i].hServer = item->getServerHandle();

		address_space::Tag *tag = opcAddressSpace::getInstance().getTag( itemID );
		(*ppAddResults)[i].vtCanonicalDataType = tag->getCanonicalDataType();
		(*ppAddResults)[i].dwAccessRights = tag->getAccessRights();
		(*ppAddResults)[i].dwBlobSize = 0;
		(*ppAddResults)[i].pBlob = NULL;
		itemList.insert( std::pair< OPCHANDLE, GroupItemElem > ( item->getServerHandle(), item ));
		(*ppErrors)[i] = S_OK;
	}
	return res;
}

/*!
	Determines if an item is valid (could it be added without error). 
	Also returns information about the item such as canonical data type. 
	Does not affect the group in any way.

	\param dwCount
		The number of items to be validated.

	\param pItemArray
		Array of OPCITEMDEFs. These tell the server everything it needs to know 
		about the item including the access path, definition and requested data type.

	\param bBlobUpdate
		If non-zero (and the server supports Blobs) the server should return 
		updated Blobs in OPCITEMRESULTs. 
		If zero (False) the server will not return Blobs in OPCITEMRESULTs.

	\param ppValidationResults
		Array of OPCITEMRESULTs. This tells the client additional information 
		about the item including the canonical data type.

	\param ppErrors
		Array of HRESULTs. This tells the client which of the items was 
		successfully validated. For any item  which failed it provides a reason.

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory.

	\retval E_INVALIDARG
		An argument to the function was invalid 
		(e.g. dwCount = 0 )

	\retval S_OK
		The operation succeeded.

	\retval S_FALSE
		The operation completed with one or more errors.
		Refer to individual error returns for failure analysis.

	\section ppErrorsCodes
		\arg \b S_OK - The function was successful for this item.
		\arg \b OPC_E_INVALIDITEMID - The ItemID is not syntactically valid.
		\arg \b OPC_E_UNKNOWNITEMID - The ItemID is not in the server address space.
		\arg \b OPC_E_BADTYPE - The requested data type cannot be returned for this item (See \ref ItemMgt_ValidateItems "Comments" section)
		\arg \b E_FAIL - The function was unsuccessful for this item. 
		\arg \b OPC_E_UNKNOWNPATH - The item's access path is not known to the server.

	\section ItemMgt_ValidateItems Comments
		\arg
		The client needs to free all of the memory associated with the OPCITEMRESULTs including the BLOB.

		\arg
		As an alternative to OPC_E_BADTPYE it is acceptable for the server
		 to return any FAILED error returned by VariantChangeType or VariantChangeTypeEx
*/
STDMETHODIMP ItemMgt::ValidateItems(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCITEMDEF *pItemArray,
	/* [in] */ BOOL bBlobUpdate,
	/* [size_is][size_is][out] */ OPCITEMRESULT **ppValidationResults,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;
	if (pItemArray == NULL || ppValidationResults == NULL || ppErrors == NULL )
		return E_INVALIDARG;
	*ppValidationResults = NULL;
	*ppErrors = NULL;

	if( dwCount == 0 )
		return E_INVALIDARG;

	*ppValidationResults = os::win32::com::allocMemory< OPCITEMRESULT >( dwCount );
	if( *ppValidationResults == NULL )
		return E_OUTOFMEMORY;
	os::win32::com::zeroMemory< OPCITEMRESULT >( *ppValidationResults, dwCount );

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	if( *ppErrors == NULL )
	{
		os::win32::com::freeMemory( *ppValidationResults );
		return E_OUTOFMEMORY;
	}
	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT res = S_OK;
	boost::mutex::scoped_lock guard( groupGuard );
	String itemID;
	for( DWORD i = 0; i < dwCount; ++i )
	{
		if( pItemArray[i].szItemID == NULL || wcslen(pItemArray[i].szItemID) == 0 )
		{
			(*ppErrors)[i] = OPC_E_INVALIDITEMID;
			res = S_FALSE;
			continue;
		}

		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			itemID = pItemArray[i].szItemID;
		#else
			itemID = wstring2string( pItemArray[i].szItemID );
		#endif

		if( ! opcAddressSpace::getInstance().isExistLeaf( itemID ) )
		{
			(*ppErrors)[i] = OPC_E_UNKNOWNITEMID;
			res = S_FALSE;
			continue;
		}

		if (! os::win32::com::isValidType(pItemArray[i].vtRequestedDataType ) )
		{
			(*ppErrors)[i] = OPC_E_BADTYPE;
			res = S_FALSE;
			continue;
		}

		address_space::Tag *item = opcAddressSpace::getInstance().getTag(itemID );
		ppValidationResults[0][i].vtCanonicalDataType = item->getCanonicalDataType();
		ppValidationResults[0][i].dwAccessRights = item->getAccessRights();
		ppValidationResults[0][i].dwBlobSize = 0;
		ppValidationResults[0][i].pBlob = NULL;
		ppValidationResults[0][i].hServer = 	util::getUniqueServerHandle();
	}
	return res;
}

/*!
	Removes (deletes) items from a group. Basically this is the reverse of AddItems.

	\param dwCount
		Number of items to be removed.

	\param phServer
		Array of server items handles. These were returned from AddItem.

	\param ppErrors
		Array of HRESULTs. Indicates which items were successfully removed.

	\retval S_OK
		The operation succeeded.

	\retval S_FALSE
		The function completed with one or more errors. 
		See the \ref ItemMgt_RemoveItems_ppErrors "ppErrors" to determine what  happened

	\retval E_FAIL
		The operation failed.

	\retval E_INVALIDARG
		An argument to the function was invalid ( e.g dwCount = 0).

	\retval OPC_E_PUBLIC
		Cannot remove items from a public group

	\section ItemMgt_RemoveItems_ppErrors ppErrors codes
		\arg \b S_OK - The corresponding item was removed.
		\arg \b OPC_E_INVALIDHANDLE - The corresponding Item handle was invalid.

	\section ItemMgt_RemoveItems_Comments Comments
		\arg
		Adding and removing items from a group does not affect the address space 
		of the server or physical device. It simply indicates whether 
		or not the client is interested in those particular items.
		\arg
		Items are not really objects in the custom interface (do not have interfaces), 
		and there is no concept of a reference count for items. 
		The client should insure that no further references are made to deleted items.
		\arg
		Items cannot be removed from a public group.
*/
STDMETHODIMP ItemMgt::RemoveItems(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if( phServer == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*ppErrors = NULL;
	if( dwCount == 0 )
		return E_INVALIDARG;

	*ppErrors = os::win32::com::allocMemory<HRESULT>( dwCount );
	if( *ppErrors == NULL )
		return E_OUTOFMEMORY;
	os::win32::com::zeroMemory<HRESULT>( *ppErrors, dwCount );

	HRESULT res = S_OK;
	GroupItemElemList::iterator it;
	boost::mutex::scoped_lock guard( groupGuard );
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		it = itemList.find( phServer[i] );
		if( it == end ) 
		{
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			res = S_FALSE;
			continue;
		}
		// and disconnected from all async requests
		server->removeItemFromRequestList( phServer[i] );
		itemList.erase( it );
	}
	return res;
}

/*!
	Sets one or more items in a group to active or inactive. 
	This controls whether or not valid data can be obtained from Read CACHE 
	for those items and whether or not they are included in the IAdvise subscription to the group.

	\param dwCount
		The number of items to be affected.

	\param phServer
		Array of Server items handles.

	\param bActive
		TRUE if items are to be activated. 
		FALSE if items are to be deactivated.

	\param ppErrors
		Array of HRESULTs. Indicates which 
		items were successfully affected.

	\retval S_OK
		The operation succeeded.

	\retval S_FALSE
		The function completed with one or more errors. 
		See the \ref ItemMgt_SetActiveState_ppErrors "ppErrors" to determine what  happened

	\retval E_INVALIDARG
		An argument to the function was invalid ( e.g dwCount = 0).

	\retval E_FAIL
		The operation failed.

	\section ItemMgt_SetActiveState_ppErrors ppErrors codes
		\arg \b S_OK - The function was successful.
		\arg \b OPC_E_INVALIDHANDLE - The corresponding Item handle was invalid

	\section ItemMgt_SetActiveState_Comments Comments
		\arg
		Deactivating items will not result in a callback 
		(since by definition callbacks do not occur for inactive items). 
		Activating items will generally result in an IAdvise callback at the next UpdateRate period.
*/
STDMETHODIMP ItemMgt::SetActiveState(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [in] */ BOOL bActive,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;
	if (phServer == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*ppErrors = NULL;
	if( dwCount == 0 )
		return E_INVALIDARG;

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	if( *ppErrors == NULL ) 
		return E_OUTOFMEMORY;
	os::win32::com::zeroMemory<HRESULT>( *ppErrors, dwCount );

	HRESULT res = S_OK;
	GroupItemElemList::iterator it;
	boost::mutex::scoped_lock guard( groupGuard );
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		it = itemList.find( phServer[i] );
		if ( it == end )
		{
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			res = S_FALSE;
			continue;
		}
		(*it).second->isActived( ( bActive == VARIANT_TRUE ) || ( bActive == TRUE ) );
		(*ppErrors)[i] = S_OK;
	}
	return res;
}

/*!
	Changes the client handle for one or more items in a group.

	\param dwCount
	The number of items to be affected

	\param phServer
	Array of Server items handles.

	\param phClient
	Array of new Client item handles to be stored. 
	The Client handles do not need to be unique.

	\param ppErrors
	Array of HRESULTs. Indicates which items were successfully affected.

	\retval S_OK
	The function was successful.

	\retval S_FALSE
	The function completed with one or more errors. 
	See the \ref ItemMgt_SetClientHandles_ppErrors "ppErrors" to determine what  happened

	\retval E_INVALIDARG
	An argument to the function was invalid (e.g dwCount = 0).

	\retval E_FAIL
	The function was unsuccessful.

	\section ItemMgt_SetClientHandles_ppErrors ppErrors codes
		\arg \b S_OK - The function was successful.
		\arg \b OPC_E_INVALIDHANDLE - 
			The corresponding Item handle was invalid.

	\section ItemMgt_SetClientHandles_Comments Comments
		\arg
		In general, it is expected that clients will set the client handle when the item 
		is added and not change it later. This function is most useful for setting 
		the client handles for items in a public group to which the client has connected.
*/
STDMETHODIMP ItemMgt::SetClientHandles(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][in] */ OPCHANDLE *phClient,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if ( phServer == NULL || phClient == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*ppErrors = NULL;

	if( dwCount == 0 )
		return E_INVALIDARG;

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	if( *ppErrors == NULL )
		return E_OUTOFMEMORY;

	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT res = S_OK;
	GroupItemElemList::iterator it;
	boost::mutex::scoped_lock guard( groupGuard );
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		it = itemList.find( phServer[i] );
		if( it == end )
		{
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			res = S_FALSE;
		}
		else
			(*it).second->setClientHandle( phClient[i] );
	}
	return res;
}

/*!
	Changes the requested data type for one or more items in a group.

	\param dwCount
		The number of items to be affected.

	\param phServer
		Array of Server items handles.

	\param pRequestedDatatypes
		Array of new Requested Datatypes to be stored.

	\param ppErrors
		Array of HRESULT's. Indicates which items were successfully affected.

	\retval S_OK
		The function was successful.

	\retval S_FALSE
		The function completed with one or more errors. 
		See the \ref ItemMgt_SetDatatypes_ppErrors "ppErrors" to determine what  happened

	\retval E_INVALIDARG
		An argument to the function was invalid ( e.g dwCount = 0).

	\retval E_FAIL
		The function was unsuccessful. 

	\section ItemMgt_SetDatatypes_ppErrors ppErrors codes
		\arg
		\b S_OK - 
			The function was successful.
		\arg
		\b OPC_E_INVALIDHANDLE - 
			The corresponding Item handle was invalid.
		\arg
		\b OPC_E_BADTYPE - 
			The requested datatype cannot be supported for this item. 
			(See \ref ItemMgt_SetDatatypes_Comments "Comments"). 
			The previous requested type is left unchanged.

	\section ItemMgt_SetDatatypes_Comments Comments
		\arg
		In general, it is expected that clients will set the requested datatype 
		when the item is added and not change it later. 
		This function is most useful for setting the datatype 
		for items in a public group to which the client has connected.
		\arg
		As an alternative to OPC_E_BADTPYE it is acceptable for the server to return
		 any FAILED error returned by VariantChangeType() or VariantChangeTypeEx().
*/
STDMETHODIMP ItemMgt::SetDatatypes( /* [in] */ DWORD dwCount, /* [size_is][in] */ OPCHANDLE *phServer, /* [size_is][in] */ VARTYPE *pRequestedDatatypes, /* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if( phServer == NULL || pRequestedDatatypes == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*ppErrors = NULL;
	if( dwCount == 0 )
		return E_INVALIDARG;

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	if( *ppErrors == NULL )
		return E_OUTOFMEMORY;

	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT res = S_OK;
	GroupItemElemList::iterator it;
	boost::mutex::scoped_lock guard( groupGuard );
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		it = itemList.find( phServer[i] );
		if( it == end )
		{
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			res = S_FALSE;
			continue;
		}
		if( ! os::win32::com::isValidType( pRequestedDatatypes[i] ) )
		{
			(*ppErrors)[i] = OPC_E_BADTYPE;
			res = S_FALSE;
			continue;
		}
		(*it).second->setRequestDataType( pRequestedDatatypes[i] );
	}
	return res;
}

/*!
	Create an enumerator for the items in the group.

	\param riid
		The interface requested.  At this time the only supported OPC interface 
		is ID_IEnumOPCItemAttributes although vendors can add their own extensions to this.

	\param ppUnk
		Where to return the interface. NULL is returned for any HRESULT other than S_OK

	\retval S_OK
		The function was successful.

	\retval S_FALSE
		There is nothing to enumerate (There are no items in the group).

	\retval E_OUTOFMEMORY
		Not enough memory.

	\retval E_INVALIDARG
		An argument to the function was invalid (e.g. a bad riid parameter was passed).

	\retval E_FAIL
		The function was unsuccessful.

	\section ItemMgt_CreateEnumerator_Comments Comments
		\arg The client must release the returned interface pointer when it is done with it.
*/
STDMETHODIMP ItemMgt::CreateEnumerator( /* [in] */ REFIID riid, /* [iid_is][out] */ LPUNKNOWN *ppUnk )
{
	if( deleted )
		return E_FAIL;

	boost::mutex::scoped_lock guard( groupGuard );
	if (ppUnk == NULL)
		return E_INVALIDARG;

	if ( riid == IID_IEnumOPCItemAttributes)
	{
		if( itemList.empty() )
			return S_FALSE;

		EnumOPCItemAttributes *temp = new EnumOPCItemAttributes();
		if (temp == NULL)
			return E_OUTOFMEMORY;

		std::for_each( itemList.begin(), itemList.end(), boost::bind( &EnumOPCItemAttributes::addItem, temp, _1 ) );
		return temp->QueryInterface(riid,(void**)ppUnk);
	}
	return E_INVALIDARG;
}

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
