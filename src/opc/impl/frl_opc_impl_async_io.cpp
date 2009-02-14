#include "opc/impl/frl_opc_impl_async_io.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <OleCtl.h>
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "opc/frl_opc_async_request.h"
#include "opc/frl_opc_server.h"
#include "opc/frl_opc_group.h"

namespace frl { namespace opc { namespace impl {

/*! Dtor */
AsyncIO::~AsyncIO()
{
}

/*!
	Read one or more items in a group. 
	The results are returned via the client's IOPCDataCallback connection 
	established through the server's IConnectionPointContainer.
	Reads are from "DEVICE" and are not affected by the ACTIVE state of the group or item.

	\param[in] dwCount
		Number of items to be read.

	\param[in] phServer
		Array of server item handles of the items to be read

	\param[in] dwTransactionID
		The Client generated transaction ID. 
		This is included in the "completion" information provided to the OnReadComplete.

	\param[out] pdwCancelID
		Place to return a Server generated ID to be used in case the operation needs to be canceled.

	\param[out] ppErrors
		Array of errors for each item - returned by the server. See below.

	\retval S_OK
		The operation succeeded. The read was successfully initiated.

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory.

	\retval E_INVALIDARG
		An argument to the function was invalid ( e.g dwCount = 0).

	\retval S_FALSE
		One or more of the passed items could not be read. 
		The ppError array indicates which items in phServer could not be read 
		(see \ref AsyncIO_Read_ppErrors "ppErrors codes").
		Any items which do not return errors (E) here will be read and results will be returned to OnReadComplete. 
		Items which do return errors here will not be returned in the callback.

	\retval CONNECT_E_NOCONNECTION
		The client has not registered a callback through IConnectionPoint::Advise.

	\section AsyncIO_Read_ppErrors ppErrors codes
		\arg \b S_OK - 
			The corresponding Item handle was valid and the item information will be returned on OnReadComplete.
		\arg \b E_FAIL - 
			The Read failed for this item
		\arg \b OPC_E_BADRIGHTS- 
			The item is not readable
		\arg \b OPC_E_INVALIDHANDLE - 
			The passed item handle was invalid. 
		\arg \b OPC_E_UNKNOWNITEMID - 
			The item is no longer available in the server address space. 
		\arg \b E_xxx - 
			Vendor specific errors may also be returned. Descriptive information 
			for such errors can be obtained from GetErrorString().
		\arg \b S_xxx - 
			Vendor specific errors may also be returned. Descriptive information 
			for such errors can be obtained from GetErrorString().

	\section AsyncIO_Red_Comments Comments
		\arg
		Some servers will be "smarter" at read time and return "early" errors, 
		others may simply queue the request with minimal checking and return "late" errors in the callback. 
		The client should be prepared to deal with this.
		\arg
		If the HRESULT is S_OK, then ppError can be ignored (all results in it are guaranteed to be S_OK). 
		\arg
		If the HRESULT is any FAILED code then (as noted earlier) the server should return NULL pointers 
		for all OUT parameters. Note that in this case no Callback will occur.
		\arg
		If ALL errors in ppError are Failure codes then No callback will take place.
		\arg
		Items for which ppError returns any success code (including  S_xxx) will be returned in the OnReadComplete callback. 
		Note that the error result for an item returned in the callback may differ from that returned from Read.
		\arg
		NOTE: the server must return all of the results in a single callback. 
		Thus, if the items in the group require multiple physical transactions to one or more physical devices 
		then the server must wait until all of them are complete before invoking OnReadComplete.
		\arg
		The Client must free the returned ppError array.
*/
STDMETHODIMP AsyncIO::Read(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [in] */ DWORD dwTransactionID,
	/* [out] */ DWORD *pdwCancelID,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if( phServer == NULL || pdwCancelID == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*pdwCancelID = 0;
	*ppErrors    = NULL;

	if( dwCount == 0 )
		return E_INVALIDARG;

	if( ! isConnected( IID_IOPCDataCallback ) )
		return CONNECT_E_NOCONNECTION;

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT result = S_OK;
	std::list<OPCHANDLE> handles;

	boost::mutex::scoped_lock guard( groupGuard );
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		GroupItemElemList::iterator it = itemList.find( phServer[i] );
		if( it == end )
		{
			result = S_FALSE;
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			continue;
		}
		handles.push_back( (*it).first );
		(*ppErrors)[i] = S_OK;
	}

	if( handles.size() > 0 )
	{
		GroupElem tmp = GroupElem( dynamic_cast< Group* >( this ) );
		AsyncRequestListElem request( new AsyncRequest( tmp, async_request::READ, handles) );
		*pdwCancelID = request->getCancelID();
		request->setTransactionID( dwTransactionID );
		server->addAsyncRequest( request );
	}

	return result;
}


/*!
	Write one or more items in a group. 
	The results are returned via the client's IOPCDataCallback connection established through 
	the server's IConnectionPointContainer.

	\param[in] dwCount
		Number of items to be written.

	\param[in] phServer
		List of server items handles for the items to be written.

	\param[in] pItemValues
		List of values to be written. The value data types do not match the requested 
		or canonical item data type but must be "convertible" to the canonical type.

	\param[in] dwTransactionID
		The Client generated transaction ID. This is included in the "completion" information provided to the OnWriteComplete.

	\param[out] pdwCancelID
		Place to return a Server generated ID to be used in case the operation needs to be canceled.

	\param[out] ppErrors
		Array of errors for each item - returned by the server. See below.

	\retval S_OK
		The operation succeeded.

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory.

	\retval E_INVALIDARG
		An argument to the function was invalid (e.g dwCount = 0).

	\retval S_FALSE
		One or more of the passed items could not be written. 
		The ppError array indicates which items in phServer could not be write 
		(see \ref AsyncIO_Write_ppErrors "ppErrors codes").
		Any items which do not return errors (E) here will be written and results \b will be returned to OnWriteComplete. 
		Items which do return errors here <b>will not</b> be returned in the callback.

	\retval CONNECT_E_NOCONNECTION
		The client has not registered a callback through IConnectionPoint::Advise.

	\section AsyncIO_Write_ppErrors ppErrors codes
		\arg \b S_OK - 
			The corresponding Item handle was valid. 
			The write will be attempted and the results will be returned on OnWriteComplete
		\arg \b E_FAIL - 
			The function was unsuccessful.
		\arg \b OPC_E_BADRIGHTS - 
			The item is not writeable.
		\arg \b OPC_E_INVALIDHANDLE - 
			The passed item handle was invalid.
		\arg \b OPC_E_UNKNOWNITEMID - 
			The item is no longer available in the server address space.
		\arg \b E_xxx - 
			Vendor specific errors may also be returned. Descriptive information 
			for such errors can be obtained from GetErrorString().
		\arg \b S_xxx - 
			Vendor specific errors may also be returned. Descriptive information 
			for such errors can be obtained from GetErrorString().

	\section AsyncIO_Write_Comments Comments
	\arg
		Some servers will be "smarter" at write time and return "early" errors, 
		others may simply queue the request with minimal checking 
		and return "late" errors in the callback. The client should be prepared to deal with this.
	\arg
		If the HRESULT is S_OK, then ppError can be ignored 
		(all results in it are guaranteed to be S_OK).
	\arg
		If the HRESULT is any FAILED code then (as noted earlier) the server should 
		return NULL pointers for all OUT parameters. Note that in this case no Callback will occur.
	\arg
		If ALL errors in ppError are Failure codes then No callback will take place.
	\arg
		Items for which ppError returns any success code (including S_xxx) will also have 
		a result returned in the OnWriteComplete callback. 
		Note that the error result for an item returned in the callback may differ from that returned from Write.
	\arg
		NOTE: all of the results must be returned by the server in a single callback. 
		Thus if the items in the group require multiple physical transactions to one 
		or more physical devices then the server must wait until all of them are complete before invoking the callback.
	\arg
		Client must free the returned ppError array.
*/
STDMETHODIMP AsyncIO::Write(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][in] */ VARIANT *pItemValues,
	/* [in] */ DWORD dwTransactionID,
	/* [out] */ DWORD *pdwCancelID,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if( phServer == NULL || pItemValues == NULL || pdwCancelID == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*pdwCancelID = 0;
	*ppErrors    = NULL;

	if( dwCount == 0 )
		return E_INVALIDARG;

	if( ! isConnected( IID_IOPCDataCallback ) )
		return CONNECT_E_NOCONNECTION;

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT result = S_OK;
	std::list< OPCHANDLE > handles;

	boost::mutex::scoped_lock guard( groupGuard );
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		GroupItemElemList::iterator it = itemList.find( phServer[i] );
		if( it == end )
		{
			result = S_FALSE;
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			continue;
		}
		handles.push_back( (*it).first );
		(*ppErrors)[i] = S_OK;
	}

	if( handles.size() > 0 )
	{
		GroupElem tmp = GroupElem( dynamic_cast< Group* >( this ) );
		AsyncRequestListElem request( new AsyncRequest( tmp, async_request::WRITE ) );
		request->init( handles, pItemValues );
		*pdwCancelID = request->getCancelID();
		request->setTransactionID( dwTransactionID );
		server->addAsyncRequest( request );
	}
	return result;
}

/*!
	Force a callback to IOPCDataCallback::OnDataChange() for all active items in the group 
	(whether they have changed or not). Inactive items are not included in the callback.

	\param[in] dwSource
		Data source CACHE or DEVICE. If the DEVICE, 
		then all active items in the CACHE are refreshed from the device BEFORE the callback.

	\param[in] dwTransactionID
		The Client generated transaction ID. 
		This is included in the "completion" information provided to the OnDataChange.

	\param[out] pdwCancelID
		Place to return a Server generated ID 
		to be used in case the operation needs to be canceled.

	\retval S_OK - 
		The operation succeeded.

	\retval E_FAIL - 
		The operation failed (see notes in \ref AsyncIO_Refresh2_Comments below).

	\retval E_OUTOFMEMORY - 
		Not enough memory.

	\retval E_INVALIDARG - 
		An argument to the function was invalid.

	\retval CONNECT_E_NOCONNECTION - 
		The client has not registered a callback through IConnectionPoint::Advise().

	\section AsyncIO_Refresh2_Comments Comments
	\arg
		If the HRESULT is any FAILED code then no Callback will occur.
	\arg
		Calling  Refresh for an InActive Group will return E_FAIL. 
		Calling refresh for an Active Group, where all the items in the group are InActive also returns E_FAIL.
	\arg
		The behavior of this function is identical to what happens when Advise 
		is called initially except that the OnDataChange Callback will include the transaction ID specified here 
		(The initial OnDataChange callback will contain a Transaction ID of 0). 
		Thus if it is important to the client to distinguish between OnDataChange callbacks resulting from changes 
		to values and OnDataChange callbacks resulting from a Refresh2 request then a non-zero ID should be passed to Refresh2().
	\arg
		Functionally it is also similar to what could be achieved by doing a READ of all of the active items in a group.
	\arg
		NOTE: all of the results must be returned in a single callback. 
		Thus if the items in the group require multiple physical transactions to one or more physical devices 
		then the server must wait until all of them are complete before invoking OnDataChange().
	\arg
		The expected behavior is that this Refresh will not affect the timing of normal OnDataChange callbacks which 
		are based on the UpdateRate.  For example, if the update rate is 1 hour and this method is called after 45 minutes 
		then the server should still do its internal "checking" at the end of the hour (15 minutes after the Refresh call). 
		Calling this method may affect the contents of that next callback (15 minutes later) since only items where the 
		value or status changed during that 15 minutes would be included. 
		Items which had changed during the 45 minutes preceding the Refresh will be sent (along with all other values) 
		as part of the Refresh Transaction. They would not be sent a second time at the end of the hour. 
		The value sent in response to the Refresh becomes the "last value sent" 
		to the client when performing the normal subscription logic.
*/
STDMETHODIMP AsyncIO::Refresh2(
	/* [in] */ OPCDATASOURCE dwSource,
	/* [in] */ DWORD dwTransactionID,
	/* [out] */ DWORD *pdwCancelID )
{
	if( deleted )
		return E_FAIL;

	if( pdwCancelID == NULL )
		return E_INVALIDARG;

	if( dwSource != OPC_DS_CACHE && dwSource != OPC_DS_DEVICE )
		return E_INVALIDARG;

	*pdwCancelID = 0;

	boost::mutex::scoped_lock guard( groupGuard );

	if( ! isConnected( IID_IOPCDataCallback ) )
		return CONNECT_E_NOCONNECTION;

	if( ! actived )
		return E_FAIL;

	if( itemList.empty() )
		return E_FAIL;

	std::list< OPCHANDLE > handles;
	GroupItemElemList::iterator end = itemList.end();
	for( GroupItemElemList::iterator it = itemList.begin(); it != end; ++it )
	{
		if( (*it).second->isActived() )
			handles.push_back( (*it).first );
	}

	if( handles.empty() )
		return E_FAIL;

	GroupElem tmp = GroupElem( dynamic_cast< Group* >( this ) );
	AsyncRequestListElem request( new AsyncRequest( tmp, async_request::UPDATE, handles) );
	*pdwCancelID = request->getCancelID();
	request->setTransactionID( dwTransactionID );
	request->setSource( dwSource );
	doAsyncRefresh( request );
	return S_OK;
}

/*!
	Request that the server cancel an outstanding transaction.

	\param[in] dwCancelID
		The Server generated Cancel ID which was associated with the operation when it was initiated.

	\retval S_OK
		The operation succeeded.

	\retval E_FAIL
		The operation failed. Either the Cancel ID was invalid or it was "too late" to cancel the transaction.

	\section AsyncIO_Cancel2_Comments Comments
		\arg
			The exact behavior (for example whether an operation that has actually started will be aborted) 
			will be server specific and will also depend on the timing of the cancel request. 
			Also, depending on the timing, a Callback for the transaction may or may not occur. 
			This method is intended for use during shutdown of a task.
		\arg
			In general, if this operation succeeds then a OnCancelComplete callback will occur. 
			If this operation fails then a read, write or data change callback may occur (or may already have occurred).
*/
STDMETHODIMP
AsyncIO::Cancel2( /* [in] */ DWORD dwCancelID )
{
	if( deleted )
		return E_FAIL;

	if( ! isConnected( IID_IOPCDataCallback ) )
		return CONNECT_E_NOCONNECTION;

	if( server->asyncRequestCancel( dwCancelID ) )
		return S_OK;
	return E_FAIL;
}

/*!
	Controls the operation of OnDataChange. Basically setting Enable to FALSE will disable 
	any OnDataChange callbacks with a transaction ID of 0 (those which are not the result of a Refresh).

	\param[in] bEnable
		TRUE enables OnDataChange callbacks, FALSE disables OnDataChange callbacks.

	\retval S_OK
		The operation succeeded.

	\retval CONNECT_E_NOCONNECTION
		The client has not registered a callback through IConnectionPoint::Advise.

	\retval E_FAIL
		The operation failed.

	\section AsyncIO_SetEnable_Comments Comments
		\arg
			The initial value of this variable when the group is created i
			s TRUE and thus OnDataChange callbacks are enabled by default.
		\arg
			The purpose of this function is to allow a Connection 
			to be established to an active group without necessarily enabling the OnDataChange notifications. 
			An example might be a client doing an occasional Refresh from cache.
		\arg
			Even if a client does not intend to use the OnDataChange, 
			it should still be prepared to deal with one or more OnDataChange callbacks 
			which might occur before the client has time to disable them 
			(i.e. at least free the memory associated with the "out" parameters).
		\arg
			If the client really needs to prevent these initial unwanted 
			callbacks then the following procedure can be used. 
			Client creates and populates the group. Client sets the group Active state to FALSE. 
			Client creates connection to group. 
			Client uses this function to disable OnDataChange. sets the group Active state back to TRUE. 
		\arg
			This does NOT affect operation of  Refresh2(). I.e. calling Refresh2 will still result in an OnDataChange callback 
			(with a non-zero transaction ID). Note that this allows Refresh to be used as essentially an Async read from Cache.
*/
STDMETHODIMP AsyncIO::SetEnable( /* [in] */ BOOL bEnable )
{
	if( deleted )
		return E_FAIL;

	boost::mutex::scoped_lock guard( groupGuard );

	if( ! isConnected( IID_IOPCDataCallback ) )
		return CONNECT_E_NOCONNECTION;

	if( bEnable == TRUE || bEnable == VARIANT_TRUE )
		enabled = frl::True;
	else
		enabled = frl::False;
	return S_OK;
}

/*!
	Retrieves the last Callback Enable value set with SetEnable.

	\param[out] pbEnable
		Where to save the returned result.

	\retval S_OK
		The operation succeeded.

	\retval CONNECT_E_NOCONNECTION
		The client has not registered a callback through IConnectionPoint::Advise.

	\retval E_FAIL
		The operation failed.

	\section AsyncIO_GetEnable_Comments Comments
		See IOPCAsyncIO2::SetEnable() \ref AsyncIO_SetEnable_Comments for additional information.
*/
STDMETHODIMP AsyncIO::GetEnable( /* [out] */ BOOL *pbEnable )
{
	if( deleted )
		return E_FAIL;

	if( pbEnable == NULL )
		return E_INVALIDARG;

	boost::mutex::scoped_lock guard( groupGuard );

	if( ! isConnected( IID_IOPCDataCallback ) )
		return CONNECT_E_NOCONNECTION;

	*pbEnable = enabled;

	return S_OK;
}

/*!
	Reads one or more values, qualities and timestamps for the items specified. 
	This is functionally similar to the OPCSyncIO::Read method except it is asynchronous and no source is specified (DEVICE or CACHE). 
	The server will make the determination as whether the information will be obtained from the device or cache. 
	This decision will be based upon the MaxAge parameter. 
	If the information in the cache is within the MaxAge, then the data will be obtained 
	from the cache, otherwise the server must access the device for the requested information. 

	\param[in] dwCount
		The number of items to be read.

	\param[in] phServer
		The list of server item handles for the items to be read.

	\param[in] pdwMaxAge
		An array of "staleness" for each item, requested in milliseconds. 
		The server will calculate, for each requested item, the number of milliseconds 
		between "now" and the timestamp on each item. 
		For each item that has not been updated within the last MaxAge milliseconds, 
		the item must be obtained from the underlying device. 
		Or if the item is not available from the cache, it will also need 
		to be obtained from the underlying device. 
		A max age of 0 is equivalent to OPC_DS_DEVICE and a max age of 0xFFFFFFFF is equivalent to OPC_DS_CACHE. 
		Without existence of a cache the server will always read from device. 
		In this case MaxAge is not relevant. 
		Clients should not expect that a cache exists, if they have 
		not activated both the item and the containing group. 
		Some servers maintain a global cache for all clients. 
		If the needed item is in this global cache, it is expected that the server makes use of it to check the MaxAge value. 
		Servers should not automatically create or change the caching of an item based on a Read call with MaxAge 
		(Note: Since this is a DWORD of milliseconds, the largest MaxAge value would be approximately is 49.7 days).

	\param[in] dwTransactionID
		The Client generated transaction ID. This is included in the "completion" information provided to the OnReadComplete.

	\param[out] pdwCancelID
		Place to return a Server generated ID to be used in case the operation needs to be canceled.

	\param[out] ppErrors
		Array of errors for each item - returned by the server. See below. 
		The errors correspond to the items or server item handles  passed in phServer. 
		Note that these must be freed by the client.

	\retval S_OK
		The operation succeeded. 

	\retval E_INVALIDARG
		An invalid argument was passed (e.g. dwCount = 0).

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory.

	\retval S_FALSE
		One or more of the passed items could not be read. 
		The ppErrors array indicates which items in phServer could not be read. 
		Any items which do not return errors (E) here will be read and results will be returned to OnReadComplete. 
		Items which do return errors here will not be returned in the callback.

	\retval CONNECT_E_NOCONNECTION
		The client has not registered a callback through IConnectionPoint::Advise.

	\section AsyncIO_ReadMaxAge_ppErrors ppErrors codes
		\arg \b S_OK - 
			Successful Read.
		\arg \b E_FAIL - 
			The Read failed for this item.
		\arg \b OPC_E_BADRIGHTS - 
			The item is not readable.
		\arg \b OPC_E_UNKNOWNITEMID - 
			The item is no longer available in the server address space.
		\arg \b OPC_E_INVALIDHANDLE - 
			The passed item handle was invalid.
		\arg \b S_xxx - 
			Vendor specific information can be provided if this item quality is other than GOOD.
		\arg \b E_xxx - 
			Vendor specific error if this item cannot be accessed. 
			These vendor specific codes can be passed to GetErrorString().

	\section AsyncIO_ReadMaxAge_Comments Comments
		\arg
			The MaxAge will only be tested upon receipt of this call.
			 For example, if 3 items are requested with a MaxAge of 1000 milliseconds 
			 and two of the three items are within the appropriate range, 
			 then the third item must be obtained from the underlying device. 
			 Once this item is obtained the three items will be packaged 
			 and returned to the client even if the MaxAge of the other two expired while obtaining the third value. 
			 The test for MaxAge will not be re-evaluated and therefore the two "stale" items will be returned 
			 with the items obtained directly from the device. 
			 This functionality is in place to prevent the server from recursively attempting to obtain the values.
		\arg
			Some servers will be "smarter" at read time and return "early" errors, 
			others may simply queue the request with minimal checking and return "late" errors in the callback. 
			The client should be prepared to deal with this.
		\arg
			Some servers may return always the actual value, if DEVICE = CACHE.
		\arg
			If the HRESULT is S_OK, then ppError can be ignored (all results in it are guaranteed to be S_OK).
		\arg
			If the HRESULT is any FAILED code then (as noted earlier) the server should return NULL pointers 
			for all OUT parameters. Note that in this case no Callback will occur.
		\arg
			If ALL errors in ppError are Failure codes then No callback will take place.
		\arg
			Items for which ppError returns any success code (including  S_xxx) will be returned in the OnReadComplete 
			callback. Note that the error result for an item returned in the callback may differ from that returned from Read.
		\arg
			NOTE: the server must return all of the results in a single callback. 
			Thus, if the items in the group require multiple physical transactions to one or 
			more physical devices then the server must wait until all of them are 
			complete before invoking OnReadComplete.
		\arg
			The Client must free the returned ppError array.
*/
STDMETHODIMP AsyncIO::ReadMaxAge(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][in] */ DWORD *pdwMaxAge,
	/* [in] */ DWORD dwTransactionID,
	/* [out] */ DWORD *pdwCancelID,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	// dirty hack!!!
	return Read( dwCount, phServer, dwTransactionID, pdwCancelID, ppErrors );
}

/*!
	Writes one or more values, qualities and timestamps for the items specified. 
	The results are returned via the client's IOPCDataCallback connection established 
	through the server's IConnectionPointContainer. 
	This is functionally similar to the IOPCAsyncIO2::Write except that Quality and Timestamp may be written. 
	If a client attempts to write VQ, VT, or VQT it should expect that the server will write them all or none at all. 

	\param[in] dwCount
		The Number of Items to be written.

	\param[in] phServer
		The list of server item handles for the items to be read

	\param[in] pItemVQT
		The list of OPCITEMVQT structures. 
		Each structure will potentially contain a value, quality and timestamp to be written to the corresponding ItemID. 
		It the value is equal to VT_EMPTY, then no value should be written. 
		There is a Boolean value associated with each Quality and Timestamp. 
		The Boolean is an indicator as to whether the Quality or Timestamp should be written for the corresponding ItemID. 
		True indicates write, while false indicates do not write.

	\param[in] dwTransactionID
		The Client generated transaction ID. This is included in the "completion" information provided to the OnWriteComplete.

	\param[out] pdwCancelID
		Place to return a Server generated ID to be used in case the operation needs to be canceled.

	\param[out] ppErrors
		Array of errors for each item - returned by the server. See below:  Note that these must be freed by the client.

	\retval S_OK
		The operation succeeded.

	\retval E_INVALIDARG
		An invalid argument was passed (e.g. dwCount = 0).

	\retval OPC_E_NOTSUPPORTED
		If a client attempts to write any value, quality, 
		timestamp combination and the server does not support the requested combination 
		(which could be a single quantity such as just timestamp), 
		then the server will not perform any write and will return this error code.

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory.

	\retval S_FALSE
		One or more of the passed items could not be written. 
		The ppError array indicates which items in phServer could not be written. 
		Any items which do not return errors (E) here will be written and results will be returned to OnWriteComplete. 
		Items which do return errors here will not be returned in the callback. 
		It is also necessary to return S_FALSE, if there are one ore more OPC_S_XXX results in ppErrors.

	\retval CONNECT_E_NOCONNECTION
		The client has not registered a callback through IConnectionPoint::Advise.

	\section AsyncIO_WriteVQT_ppErrors ppErrors codes
		\arg \b S_OK - 
			The function was successful.
		\arg \b E_FAIL - 
			The function was unsuccessful.
		\arg \b OPC_E_BADTYPE - 
	The passed data type cannot be accepted for this item 
		\arg \b OPC_E_BADRIGHTS - 
			The item is not writable.
		\arg \b OPC_E_UNKNOWNITEMID - 
			The item is no longer available in the server address space
		\arg \b OPC_E_INVALIDHANDLE - 
			The passed item handle was invalid.
		\arg \b E_xxx - 
			Vendor specific errors may also be returned. 
			Descriptive information for such errors can be obtained from GetErrorString().
		\arg \b S_xxx - 
			Vendor specific errors may also be returned. 
			Descriptive information for such errors can be obtained from GetErrorString().

	\section AsyncIO_WriteVQT_Comments Comments
		\arg
			Some servers will be "smarter" at write time and return "early" errors, 
			others may simply queue the request with minimal checking and return "late"
			 errors in the callback. The client should be prepared to deal with this.
		\arg
			If the HRESULT is S_OK, then ppError can be ignored (all results in it are guaranteed to be S_OK). 
		\arg
			If the HRESULT is any FAILED code then (as noted earlier) 
			the server should return NULL pointers for all OUT parameters. 
			Note that in this case no Callback will occur.
		\arg
			If ALL errors in ppError are Failure codes then No callback will take place.
		\arg
			Items for which ppError returns any success code (including S_xxx) will also have 
			a result returned in the OnWriteComplete callback. 
			Note that the error result for an item returned in the callback may differ from that returned from Write.
		\arg
			NOTE: all of the results must be returned by the server in a single callback. 
			Thus if the items in the group require multiple physical transactions to one or 
			more physical devices then the server must wait until all of them are complete before invoking the callback.
		\arg
			Client must free the returned ppError array.
*/
STDMETHODIMP AsyncIO::WriteVQT(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][in] */ OPCITEMVQT *pItemVQT,
	/* [in] */ DWORD dwTransactionID,
	/* [out] */ DWORD *pdwCancelID,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if( phServer == NULL || pdwCancelID == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*pdwCancelID = 0;
	*ppErrors    = NULL;

	if( dwCount == 0 )
		return E_INVALIDARG;

	if( ! isConnected( IID_IOPCDataCallback ) )
		return CONNECT_E_NOCONNECTION;

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT result = S_OK;
	std::list< ItemHVQT > itemsHVQTList;

	boost::mutex::scoped_lock guard( groupGuard );
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		GroupItemElemList::iterator it = itemList.find( phServer[i] );
		if( it == end )
		{
			result = S_FALSE;
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			continue;
		}

		if( ! (*it).second->isWritable() )
		{
			result = S_FALSE;
			(*ppErrors)[i] = OPC_E_BADRIGHTS;
			continue;
		}

		if( pItemVQT[i].vDataValue.vt == VT_EMPTY )
		{
			result = S_FALSE;
			(*ppErrors)[i] = OPC_E_BADTYPE;
			continue;
		}

		ItemHVQT tmp;
		tmp.setHandle( (*it).first );
		tmp.setValue( pItemVQT[i].vDataValue );

		if( pItemVQT[i].bQualitySpecified )
		{
			tmp.setQuality( pItemVQT[i].wQuality );
		}

		if( pItemVQT[i].bTimeStampSpecified )
		{
			tmp.setTimeStamp( pItemVQT[i].ftTimeStamp );
		}

		itemsHVQTList.push_back( tmp );
		(*ppErrors)[i] = S_OK;
	}

	if( itemsHVQTList.size() > 0 )
	{
		GroupElem tmp = GroupElem( dynamic_cast< Group* >( this ) );
		AsyncRequestListElem request( new AsyncRequest( tmp, async_request::WRITE, itemsHVQTList ) );
		*pdwCancelID = request->getCancelID();
		request->setTransactionID( dwTransactionID );
		server->addAsyncRequest( request );
	}
	return result;
}

/*!
	Force a callback to IOPCDataCallback::OnDataChange for all active items in the group (whether they have changed or not). 
	Inactive items are not included in the callback. The MaxAge value will determine where the data is obtained. 
	There will be only one MaxAge value, which will determine the MaxAge for all active items in the group. 
	This means some of the values may be obtained from cache while others could be obtained 
	from the device depending on the "freshness" of the data in the cache.

	\param[in] dwMaxAge
		An indicator of "staleness" for all items, requested in milliseconds. 
		The server will calculate the number of milliseconds between "now" 
		and the timestamp on each item. 
		For each item that has not been updated within the last MaxAge milliseconds, 
		the item must be obtained from the underlying device. 
		Or if the item is not available from the cache, it will also need to be obtained from the underlying device. 
		A max age of 0 is equivalent to OPC_DS_DEVICE and a max age of 0xFFFFFFFF is equivalent to OPC_DS_CACHE. 
		Without existence of a cache the server will always read from device. In this case MaxAge is not relevant. 
		Clients should not expect that a cache exists, if they have not activated both the item and the containing group. 
		Some servers maintain a global cache for all clients. If the needed item is in this global cache, 
		it is expected that the server makes use of it to check the MaxAge value. 
		Servers should not automatically create or change the caching of an item based on a Read call with MaxAge 
		(Note: Since this is a DWORD of milliseconds, the largest MaxAge value would be approximately is 49.7 days).

	\param[in] dwTransactionID
		The Client generated transaction ID. 
		This is included in the "completion" information provided to the OnDataChange.

	\param[out] pdwCancelID
		Place to return a Server generated ID to be used in case the operation needs to be canceled.

	\retval S_OK
		The operation succeeded.

	\retval E_FAIL
		The operation failed. (See notes below)

	\retval E_OUTOFMEMORY
		Not enough memory.

	\retval E_INVALIDARG
		An argument to the function was invalid.

	\retval CONNECT_E_NOCONNECTION
		The client has not registered a callback through IConnectionPoint::Advise.

	\section AsyncIO_RefreshMaxAge_Comments Comments
		\arg
			If the HRESULT is any FAILED code then no Callback will occur.
		\arg
			Calling  RefreshMaxAge for an inactive Group will return E_FAIL. 
			Calling RefreshMaxAge for an Active Group, where all the items in the group are inactive also returns E_FAIL.
		\arg
			The behavior of this function is identical to what happens when Advise 
			is called initially except that the OnDataChange Callback will include the transaction ID specified here 
			(The initial OnDataChange callback will contain a Transaction ID of 0). 
			Thus if it is important to the client to distinguish between OnDataChange callbacks resulting 
			from changes to values and OnDataChange callbacks resulting from 
			a RefreshMaxAge request then a non-zero ID should be passed to RefreshMaxAge().
		\arg
			Functionally it is also similar to what could be achieved by doing 
			a ReadMaxAge of all of the active items in a group using a common MaxAge value.
		\arg
			NOTE: all of the results must be returned in a single callback. 
			Thus if the items in the group require multiple physical transactions to one 
			or more physical devices then the server must wait until all of them are complete before invoking OnDataChange.
		\arg
			The expected behavior is that this RefreshMaxAge will not affect the timing 
			of normal OnDataChange callbacks which are based on the UpdateRate. 
			For example, if the update rate is 1 hour and this method is called after 
			45 minutes then the server should still do its internal "checking" at the end of the hour 
			(15 minutes after the Refresh call). 
			Calling this method may affect the contents of that next callback 
			(15 minutes later) since only items where the value or status changed 
			during that 15 minutes would be included. 
			Items which had changed during the 45 minutes preceding the Refresh 
			will be sent (along with all other values) as part of the Refresh Transaction. 
			They would not be sent a second time at the end of the hour. 
			The value sent in response to the Refresh becomes the "last value sent" 
			to the client when performing the normal subscription logic.
*/
STDMETHODIMP AsyncIO::RefreshMaxAge(
	/* [in] */ DWORD dwMaxAge,
	/* [in] */ DWORD dwTransactionID,
	/* [out] */ DWORD *pdwCancelID )
{
	// dirty hack!!!
	if( dwMaxAge == 0xFFFFFFFF )
	{
		return Refresh2( OPC_DS_CACHE, dwTransactionID, pdwCancelID );
	}
	return Refresh2( OPC_DS_DEVICE, dwTransactionID, pdwCancelID );
}

} // namespace impl
} // namespace opc
} // FatRat Library

#endif
