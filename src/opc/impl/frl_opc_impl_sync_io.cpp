#include "opc/impl/frl_opc_impl_sync_io.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "opc/address_space/frl_opc_address_space.h"
using namespace frl::opc::address_space;

namespace frl { namespace opc { namespace impl {

/*! Dtor */
SyncIO::~SyncIO()
{
}

/*!
	This function reads the value, quality and timestamp information 
	for one or more items in a group. 
	The function runs to completion before returning. 
	The data can be read from CACHE in which case it should be accurate 
	to within the "UpdateRate" and percent deadband of the group. 
	The data can be read from the DEVICE in which case an actual read 
	of the physical device is to be performed. 
	The exact implementation of CACHE and DEVICE reads is not defined by this specification.

	When reading from CACHE, the data is only valid if both the group and the item are active. 
	If either the group or the item is inactive, then  the Quality will indicate 
	out of service (OPC_QUALITY_OUT_OF_SERVICE). 
	Refer to the discussion of the quality bits later in this document for further information.

	DEVICE reads are not affected by the ACTIVE state of the group or item.

	Refer to the Data Acquisition and Active State Behavior table earlier 
	in this document for an overview of the server data acquisition 
	behavior and it's affect on functionality within this interface.

	\param[in] dwSource
	The "data source"; OPC_DS_CACHE or OPC_DS_DEVICE.

	\param[in] dwCount
		The number of items to be read.

	\param[in] phServer
		The list of server item handles for the items to be read

	\param[out] ppItemValues
	Array of structures in which the item values are returned.

	\param[out] ppErrors
		Array of HRESULTs indicating the success of the individual item reads. 
		The errors correspond to the handles passed in phServer. 
		This indicates whether the read succeeded in obtaining a defined value, 
		quality and timestamp. NOTE any FAILED error code indicates that 
		the corresponding Value, Quality and Time stamp are UNDEFINED.

	\retval S_OK
		The operation succeeded. 

	\retval S_FALSE
		The operation succeeded but there are one or more errors in ppErrors 
		(see \ref SyncIO_Read_ppErros "ppErrors codes"). 
		Refer to individual error returns for more information.

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory.

	\retval E_INVALIDARG
		An argument to the function was invalid (e.g dwCoun t= 0).

	\section SyncIO_Read_ppErros ppErros codes
		\arg \b S_OK - 
			Successful Read.
		\arg \b E_FAIL - 
			The Read failed for this item
		\arg \b OPC_E_BADRIGHTS - 
			The item is not readable
		\arg \b OPC_E_INVALIDHANDLE - 
			The passed item handle was invalid.
		\arg \b OPC_E_UNKNOWNITEMID - 
			The item is no longer available in the server address space.
		\arg \b S_xxx - 
			Vendor specific information can be provided if this item quality is other than GOOD. 
		\arg \b E_xxx - 
			Vendor specific error if this item cannot be accessed. 
			These vendor specific codes can be passed to GetErrorString().

	\section SyncIO_Read_Comments Comments
	\arg
		If the HRESULT is S_OK, then ppError can be ignored 
		(all results in it are guaranteed to be S_OK).
	\arg
		If the HRESULT is S_FALSE, then ppError will indicate 
		which the status of each individual Item Read.
	\arg
		If the HRESULT is any FAILED code then (as noted earlier) the server 
		should return NULL pointers for all OUT parameters including ppErrors.
	\arg
		For any S_xxx ppError code the client should assume the 
		corresponding ITEMSTATE is well defined although the Quality 
		may be UNCERTAIN or BAD. 
		It is recommended (but not required) that server 
		vendors provide additional information here regarding UNCERTAIN or BAD items.
	\arg
		For any FAILED ppError code the client should assume the 
		corresponding ITEMSTATE is undefined. 
		In fact the Server must set the corresponding ITEMSTATE VARIANT 
		to VT_EMPTY so that it can be marshaled properly 
		and so that the client can execute VariantClear on it. 
	\arg
		Note that here (as in the OPCItemMgt methods) 
		OPC_E_INVALIDHANDLE on one item will not affect the processing 
		of other items and will cause the main HRESULT to return as S_FALSE
	\arg
		Expected behavior is that a CACHE read should be completed 
		very quickly (within milliseconds). A DEVICE read may take a very 
		long time (many seconds or more). 
		Depending on the details of the implementation 
		(e.g. which threading model is used) the DEVICE read may also prevent any 
		other operations from being performed on the server by any other clients.
	\arg
		For this reason Clients are expected to use CACHE reads in most cases. 
		DEVICE reads are intended for "special" circumstances such as diagnostics.
	\arg
		The ppItemValues and ppErrors arrays are allocated by the server and must be 
		freed by the client. Be sure to call VariantClear() on the variant in the ITEMRESULT.
*/
STDMETHODIMP SyncIO::Read(
	/* [in] */ OPCDATASOURCE dwSource,
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][size_is][out] */ OPCITEMSTATE **ppItemValues,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if( phServer == NULL || ppItemValues == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	if( dwSource != OPC_DS_CACHE && dwSource != OPC_DS_DEVICE )
		return E_INVALIDARG;

	*ppErrors = NULL;

	if (dwCount == 0)
		return E_INVALIDARG;

	*ppItemValues = os::win32::com::allocMemory< OPCITEMSTATE >( dwCount );
	if( ppItemValues == NULL )
		return E_OUTOFMEMORY;
	os::win32::com::zeroMemory< OPCITEMSTATE >( *ppItemValues, dwCount );

	*ppErrors =  os::win32::com::allocMemory< HRESULT >( dwCount );
	if( ppErrors == NULL )
	{
		os::win32::com::freeMemory( ppItemValues );
		return E_OUTOFMEMORY;
	}
	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT result = S_OK;

	boost::mutex::scoped_lock guard( groupGuard );
	GroupItemElemList::iterator it;
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		it = itemList.find( phServer[i] );
		if( it == end )
		{
			result = S_FALSE;
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			continue;
		}

		if( ! (*it).second->isReadable() )
		{
			result = S_FALSE;
			(*ppErrors)[i] = OPC_E_BADRIGHTS;
			continue;
		}

		try
		{
			if( dwSource == OPC_DS_CACHE )
				( *ppErrors )[i] = ( (*it).second->getCachedValue().copyTo( (*ppItemValues)[i].vDataValue ) );
			else
				( *ppErrors )[i] = ( (*it).second->readValue() ).copyTo( (*ppItemValues)[i].vDataValue );
		}
		catch( Tag::NotExistTag& )
		{
			( *ppErrors )[i] = OPC_E_INVALIDHANDLE;
		}

		if( FAILED( ( *ppErrors)[i] ) )
		{
			result = S_FALSE;
			continue;
		}

		if ( ( ! (*it).second->isActived() || ! actived ) && dwSource == OPC_DS_CACHE )
			(*ppItemValues)[i].wQuality = OPC_QUALITY_OUT_OF_SERVICE;
		else
			(*ppItemValues)[i].wQuality = (*it).second->getQuality();

		(*ppItemValues)[i].hClient = (*it).second->getClientHandle();
		(*ppItemValues)[i].ftTimeStamp = (*it).second->getTimeStamp();
	}

	return result;
}

/*!
	Writes values to one or more items in a group. The function runs to completion. 
	The values are written to the DEVICE. That is, the function should not 
	return until it verifies that the device has actually accepted (or rejected) the data.
	Writes are not affected by the ACTIVE state of the group or item

	\param[in] dwCount
		Number of items to be written

	\param[in] phServer
		The list of server item handles for the items to be read

	\param[in] pItemValues
		List of values to be written to the items. 
		The data types of the values do not need to match 
		the data types of the target items. 
		However an error will be returned if a conversion cannot be done.

	\param[out] ppErrors
		Array of HRESULTs indicating the success of the individual item Writes. 
		The errors correspond to the handles passed in phServer. 
		This indicates whether the target device or system accepted the value. 
		NOTE any FAILED error code indicates that the value was rejected by the device.  

	\retval S_OK
		The operation succeeded.

	\retval S_FALSE
		The operation succeeded but there are one or more errors in ppErrors 
		(see \ref SyncIO_Write_ppErrors "ppErrors codes").
		Refer to individual error returns for more information.

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory

	\retval E_INVALIDARG
		An argument to the function was invalid (e.g dwCount = 0).

	\section SyncIO_Write_ppErrors ppErrors codes
		\arg \b S_OK - 
			The function was successful.
		\arg \b E_FAIL - 
			The function was unsuccessful. 
		\arg \b OPC_S_CLAMP - 
			The value was accepted but was clamped.
		\arg \b OPC_E_RANGE - 
			The value was out of range.
		\arg \b OPC_E_BADTYPE - 
			The passed data type cannot be accepted for this item (See comment)
		\arg \b OPC_E_BADRIGHTS - 
			The item is not writeable
		\arg \b OPC_E_INVALIDHANDLE - 
			The passed item handle was invalid. 
		\arg \b OPC_E_UNKNOWNITEMID - 
			The item is no longer available in the server address space
		\arg \b E_xxx - 
			Vendor specific errors may also be returned. 
			Descriptive information for such errors can be obtained from GetErrorString().
		\arg \b S_xxx - 
			Vendor specific errors may also be returned. 
			Descriptive information for such errors can be obtained from GetErrorString().

	\section SyncIO_Write_Comments Comments
		\arg
			If the HRESULT is S_OK, then ppError can be ignored 
			(all results in it are guaranteed to be S_OK).
		\arg
			If the HRESULT is any FAILED code then (as noted earlier) 
			the server should return NULL pointers for all OUT parameters.
		\arg
			Note that here (as in the OPCItemMgt methods) 
			OPC_E_INVALIDHANDLE on one item will not 
			affect the processing of other items and 
			will cause the main HRESULT to return as S_FALSE
		\arg
			As an alternative to OPC_E_BADTPYE it is acceptable 
			for the server to return any FAILED error returned 
			by VariantChangeType or VariantChangeTypeEx.
		\arg
			A DEVICE write may take a very long time (many seconds or more). 
			Depending on the details of the implementation 
			(e.g. which threading model is used) the DEVICE write may also prevent a
			ny other operations from being performed on the server by any other clients.
		\arg
			For this reason Clients are expected to use ASYNC 
			write rather than SYNC write in most cases.
		\arg
			The ppErrors array is allocated by the server 
			and must be freed by the client.
*/
STDMETHODIMP SyncIO::Write(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][in] */ VARIANT *pItemValues,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if (phServer == NULL || pItemValues == NULL || ppErrors == NULL)
		return E_INVALIDARG;

	*ppErrors = NULL;

	if (dwCount == 0)
		return E_INVALIDARG;

	*ppErrors =  os::win32::com::allocMemory< HRESULT >( dwCount );
	if( ppErrors == NULL )
		return E_OUTOFMEMORY;
	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT result = S_OK;
	boost::mutex::scoped_lock guard( groupGuard );
	GroupItemElemList::iterator it;
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		it = itemList.find( phServer[i] );
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

		if( pItemValues[i].vt == VT_EMPTY )
		{
			result = S_FALSE;
			(*ppErrors)[i] = OPC_E_BADTYPE;
			continue;
		}

		(*ppErrors)[i] = (*it).second->writeValue( pItemValues[i] );

		if( FAILED( (*ppErrors)[i] ) )
			result = S_FALSE;
	}
	return result;
}

/*!
	Reads one or more values, qualities and timestamps for the items specified. 
	This is functionally similar to the OPCSyncIO::Read method except no source is specified (DEVICE or CACHE). 
	The server will make the determination as whether the information will be obtained from the device or cache. 
	This decision will be based upon the MaxAge parameter. 
	If the information in the cache is within the MaxAge, then the data will be obtained from the cache, 
	otherwise the server must access the device for the requested information.

	\param[in] dwCount
		The number of items to be read.

	\param[in] phServer
		The list of server item handles for the items to be read.

	\param[in] pdwMaxAge
		An array of “staleness” for each item, requested in milliseconds. 
		The server will calculate, for each requested item, the number of milliseconds between “now” 
		and the timestamp on each item.  For each item that has not been updated within 
		the last MaxAge milliseconds, the item must be obtained from the underlying device. 
		Or if the item is not available from the cache, it will also need to be obtained from the underlying device. 
		A max age of 0 is equivalent to OPC_DS_DEVICE and a max age of 0xFFFFFFFF is equivalent to OPC_DS_CACHE. 
		Without existence of a cache the server will always read from device. In this case MaxAge is not relevant. 
		Clients should not expect that a cache exists, if they have not activated both the item and the containing group. 
		Some servers maintain a global cache for all clients. If the needed item is in this global cache, 
		it is expected that the server makes use of it to check the MaxAge value. 
		Servers should not automatically create or change the caching of an item based on a Read call with MaxAge. 
		(\b Note: Since this is a DWORD of milliseconds, the largest MaxAge value would be approximately is 49.7 days).

	\param[out] ppvValues
		A pointer to an array of VARIANTs in which the results are returned. 
		Note that the array and its contained variants must be freed by the client after receipt.

	\param[out] ppwQualities
		An array of Words in which to store the Quality of each result. Note that these must be freed by the client.

	\param[out] ppftTimeStamps
		An array of FILETIMEs in which to store the timestamps of each result. Note that these must be freed by the client.

	\param[out] ppErrors
		Array of HRESULTs indicating the success of the individual item reads. 
		The errors correspond to the items or server item handles passed in phServer. 
		This indicates whether the read succeeded in obtaining a defined value, quality and timestamp. 
		NOTE any FAILED error code indicates that the corresponding Value, 
		Quality and Time stamp are UNDEFINED. Note that these must be freed by the client.

	\retval S_OK
		The operation succeeded.

	\retval S_FALSE
		The operation succeeded but there are one or more errors in ppErrors. 
		Refer to individual error returns for more information.

	\retval E_INVALIDARG
		An invalid argument was passed (e.g. dwCount = 0).

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory.

	\section SyncIO_ReadMaxAge_ppErrors ppErrors codes
		\arg \b S_OK - 
			Successful Read.
		\arg \b E_FAIL - 
			The Read failed for this item.
		\arg \b OPC_E_BADRIGHTS - 
			The item is not readable
		\arg \b OPC_E_UNKNOWNITEMID - 
			The item is no longer available 
			in the server address space.
		\arg \b OPC_E_INVALIDHANDLE - 
			The passed item handle was invalid.
		\arg \b S_xxx - 
			Vendor specific information can be provided if this item quality is other than GOOD.
		\arg \b E_xxx - 
			Vendor specific error if this item cannot be accessed.
			These vendor specific codes can be passed to GetErrorString().

	\section SyncIO_ReadMaxAge_Comments Comments
	\arg
		The MaxAge will only be tested upon receipt of this call. 
		For example, if 3 items are requested with a MaxAge of 1000 milliseconds 
		and two of the three items are within the appropriate range, 
		then the third item must be obtained from the underlying device. 
		Once this item is obtained the three items will be packaged and returned to 
		the client even if the MaxAge of the other two expired while obtaining the third value. 
		The test for MaxAge will not be re-evaluated and therefore the two “stale” items 
		will be returned with the items obtained directly from the device. 
		This functionality is in place to prevent the server from recursively attempting to obtain the values.
	\arg
		Some servers may return always the actual value, if DEVICE = CACHE.
	\arg
		If the HRESULT is S_OK, then ppErrors can be ignored (all results in it are guaranteed to be S_OK). 
	\arg
		If the HRESULT is S_FALSE, then ppErrors will indicate which the status of each individual Item Read. 
	\arg
		If the HRESULT is any FAILED code then the contents of  all OUT parameters including ppErrors is undefined.
	\arg
		For any S_xxx result in ppError the client may assume the corresponding Value, 
		Quality and Timestamp are well defined although the Quality may be UNCERTAIN or BAD. 
		It is recommended (but not required) that server vendors provide additional information here regarding UNCERTAIN or BAD items.
	\arg
		For any FAILED (E_) result in  ppError the client may not make use of the corresponding Value, 
		Quality and Timestamp except to call VariantClear before destroying the VARIANT. 
		When returning an E_ result in ppError, the Server must set the corresponding Value's VARIANT 
		to VT_EMPTY so that it can be marshaled properly and so that the client can execute VariantClear on it.
	\arg
		Be sure to call VariantClear() on the variants in the Values before freeing the memory containing them.
	\arg
		The client is responsible for freeing all memory associated with the out parameters.
*/
STDMETHODIMP SyncIO::ReadMaxAge(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][in] */ DWORD *pdwMaxAge,
	/* [size_is][size_is][out] */ VARIANT **ppvValues,
	/* [size_is][size_is][out] */ WORD **ppwQualities,
	/* [size_is][size_is][out] */ FILETIME **ppftTimeStamps,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;
	if ( phServer == NULL || 
		pdwMaxAge == NULL || 
		ppvValues == NULL || 
		ppwQualities == NULL || 
		ppftTimeStamps == NULL ||
		ppErrors == NULL )
	{
		return E_INVALIDARG;
	}
	*ppErrors = NULL;
	if( dwCount == 0 )
		return E_INVALIDARG;

	*ppvValues = os::win32::com::allocMemory< VARIANT >( dwCount );
	*ppwQualities = os::win32::com::allocMemory< WORD >( dwCount );
	*ppftTimeStamps = os::win32::com::allocMemory< FILETIME >( dwCount );
	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );

	os::win32::com::zeroMemory< VARIANT >( *ppvValues, dwCount );
	os::win32::com::zeroMemory< WORD >( *ppwQualities, dwCount );
	os::win32::com::zeroMemory< FILETIME >( *ppftTimeStamps, dwCount );
	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT result = S_OK;

	boost::mutex::scoped_lock guard( groupGuard );
	GroupItemElemList::iterator it;
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		it = itemList.find( phServer[i] );
		if( it == end )
		{
			result = S_FALSE;
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			continue;
		}
		if( ! (*it).second->isReadable() )
		{
			result = S_FALSE;
			(*ppErrors)[i] = OPC_E_BADRIGHTS;
			continue;
		}
		try
		{
			if( pdwMaxAge[i] == 0xFFFFFFFF )
			{
				( *ppErrors )[i] = (*it).second->getCachedValue().copyTo( (*ppvValues)[i] );
			}
			else
			{
				( *ppErrors )[i] = (*it).second->readValue().copyTo( (*ppvValues)[i]  );
			}
		}
		catch( Tag::NotExistTag& )
		{
			( *ppErrors )[i] = OPC_E_INVALIDHANDLE;
		}

		if( FAILED( ( *ppErrors)[i] ) )
		{
			result = S_FALSE;
			continue;
		}

		(*ppwQualities)[i] = (*it).second->getQuality();
		(*ppftTimeStamps)[i] = (*it).second->getTimeStamp();
	}
	return result;
}

/*!
	Writes one or more values, qualities and timestamps for the items specified. 
	This is functionally similar to the IOPCSyncIO::Write except that Quality and Timestamp may be written. 
	If a client attempts to write VQ, VT, or VQT it should expect that the server will write them all or none at all.

	\param[in] dwCount
		The Number of Items to be written.

	\param[in] phServer
		The list of server item handles for the items to be read

	\param[in] pItemVQT
		The list of OPCITEMVQT structures. 
		Each structure will potentially contain a value, quality and 
		timestamp to be written to the corresponding ItemID. 
		It the value is equal to VT_EMPTY, then no value should be written. 
		There is a Boolean value associated with each Quality and Timestamp. 
		The Boolean is an indicator as to whether the Quality or Timestamp 
		should be written for the corresponding item or server item handle. 
		True indicates write, while false indicates do not write.

	\param[out] ppErrors
		The list of errors resulting from the write (1 per item). Note that these must be freed by the client.

	\retval S_OK
		The operation succeeded.

	\retval S_FALSE
		The operation succeeded but there are one or more errors in ppErrors. 
		Refer to individual error returns for more information. 
		It is also necessary to return S_FALSE, if there are one ore more OPC_S_XXX results in ppErrors 
		(see \ref SyncIO_WriteVQT_ppErrors "ppErrors codes").

	\retval OPC_E_NOTSUPPORTED
		If a client attempts to write any value, quality, timestamp combination 
		and the server does not support the requested combination 
		(which could be a single quantity such as just timestamp), 
		then the server will not perform any write and will return this error code.

	\retval E_INVALIDARG
		An invalid argument was passed (e.g. dwCount =0 ).

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory

	\section SyncIO_WriteVQT_ppErrors ppErrors codes
		\arg \b S_OK
			The function was successful.
		\arg \b E_FAIL
			The function was unsuccessful.
		\arg \b OPC_S_CLAMP
			The value was accepted but was clamped.
		\arg \b OPC_E_RANGE
			The value was out of range.
		\arg \b OPC_E_BADTYPE
			The passed data type cannot be accepted for this item.
		\arg \b OPC_E_BADRIGHTS
			The item is not writable.
		\arg \b OPC_E_UNKNOWNITEMID
			The item is no longer available in the server address space.
		\arg \b OPC_E_INVALIDHANDLE
			The passed item handle was invalid.
		\arg \b E_xxx
			Vendor specific errors may also be returned. 
			Descriptive information for such errors can be obtained from GetErrorString().
		\arg \b S_xxx
			Vendor specific errors may also be returned. 
			Descriptive information for such errors can be obtained from GetErrorString().

	\section SyncIO_WriteVQT_Comments Comments
		\arg
			If the HRESULT is S_OK, then ppErrors can be ignored (all results in it are guaranteed to be S_OK). 
		\arg
			If the HRESULT is any FAILED code then the contents of ppErrors is undefined.
		\arg
			As an alternative to OPC_E_BADTPYE it is acceptable for the server to return 
			any FAILED error returned by VariantChangeType or VariantChangeTypeEx.
		\arg
			\b Note: There is no way to validate the writing of the timestamp. 
			If writing the timestamp is supported by the server, then the timestamp 
			will be updated on the device as opposed to the cache. 
			Writing timestamps is generally expected to be used for values which 
			are in some sort of manual override mode or for values which are in 
			some form of holding register. In general it is not useful to write timestamps 
			for values which are actually being generated or scanned by the device 
			since the device will generally re-stamp the data each time it is generated or scanned.
*/
STDMETHODIMP SyncIO::WriteVQT(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][in] */ OPCITEMVQT *pItemVQT,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if( phServer == NULL || pItemVQT == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*ppErrors = NULL;

	if( dwCount == 0 )
		return E_INVALIDARG;

	// write items.
	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT res = S_OK;
	GroupItemElemList::iterator it;
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		it = itemList.find( phServer[i] );
		if( it == end )
		{
			res = S_FALSE;
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			continue;
		}

		if( pItemVQT[i].vDataValue.vt == VT_EMPTY )
		{
			res = S_FALSE;
			(*ppErrors)[i] = OPC_E_BADTYPE;
			continue;
		}

		if( ! (*it).second->isWritable() )
		{
			res = S_FALSE;
			(*ppErrors)[i] = OPC_E_BADRIGHTS;
			continue;
		}

		(*ppErrors)[i] = (*it).second->writeValue( pItemVQT[i].vDataValue );

		if( FAILED( (*ppErrors)[i] ) )
		{
			res = S_FALSE;
			continue;
		}

		if( pItemVQT[i].bQualitySpecified )
		{
			(*it).second->setQuality( pItemVQT[i].wQuality );
		}

		if( pItemVQT[i].bTimeStampSpecified )
		{
			(*it).second->setTimeStamp( pItemVQT[i].ftTimeStamp );
		}
	}
	return res;
}

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
