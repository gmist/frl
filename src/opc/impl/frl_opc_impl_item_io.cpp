<<<<<<< .working
#include "opc/impl/frl_opc_impl_item_io.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <boost/thread/mutex.hpp>
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "os/win32/com/frl_os_win32_com_allocator.h"
#include "opc/address_space/frl_opc_address_space.h"

namespace frl { namespace opc { namespace impl {

OPCItemIO::~OPCItemIO()
{
}

STDMETHODIMP OPCItemIO::Read(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ LPCWSTR *pszItemIDs,
	/* [size_is][in] */ DWORD *pdwMaxAge,
	/* [size_is][size_is][out] */ VARIANT **ppvValues,
	/* [size_is][size_is][out] */ WORD **ppwQualities,
	/* [size_is][size_is][out] */ FILETIME **ppftTimeStamps,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	boost::mutex::scoped_lock guard( scopeGuard );

	if( pszItemIDs == NULL || pdwMaxAge == NULL ||
		ppvValues == NULL || ppwQualities == NULL ||
		ppftTimeStamps == NULL || ppErrors == NULL )
	{
		return E_INVALIDARG;
	}

	*ppvValues = NULL;
	*ppwQualities = NULL;
	*ppftTimeStamps = NULL;
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

	HRESULT res = S_OK;
	address_space::Tag *item = NULL;
	String itemID;
	for( DWORD i = 0; i < dwCount; ++i )
	{
		try
		{
			#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
				if( pszItemIDs[i] )
					itemID = pszItemIDs[i];
			#else
				if( pszItemIDs[i] )
					itemID = wstring2string( pszItemIDs[i] );
			#endif
			item = opcAddressSpace::getInstance().getLeaf( itemID );
		}
		catch( frl::Exception& )
		{
			(*ppErrors)[i] = OPC_E_INVALIDITEMID;
			res = S_FALSE;
			continue;
		}

		if( ! ( item->isReadable() ) )
		{
			res = S_FALSE;
			(*ppErrors)[i] = OPC_E_BADRIGHTS;
			continue;
		}

		(*ppErrors)[i] = os::win32::com::Variant::variantCopy( (*ppvValues)[i], item->read() );
		if( FAILED( (*ppErrors)[i] ) )
		{
			res = S_FALSE;
			continue;
		}
		(*ppwQualities)[i] = item->getQuality();
		(*ppftTimeStamps)[i] = item->getTimeStamp();
	}
	return res;
}

STDMETHODIMP OPCItemIO::WriteVQT(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ LPCWSTR *pszItemIDs,
	/* [size_is][in] */ OPCITEMVQT *pItemVQT,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	boost::mutex::scoped_lock guard( scopeGuard );

	if( pszItemIDs == NULL || pItemVQT == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*ppErrors = NULL;

	if( dwCount == 0 )
		return E_INVALIDARG;

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT res = S_OK;
	address_space::Tag *item = NULL;
	String itemID;
	for( DWORD i = 0; i < dwCount; ++i )
	{
		try
		{
			#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
				if( pszItemIDs[i] )
					itemID = pszItemIDs[i];
			#else
				if( pszItemIDs[i] )
					itemID = wstring2string( pszItemIDs[i] );
			#endif
			item = opcAddressSpace::getInstance().getLeaf( itemID );
		}
		catch( frl::Exception& )
		{
			(*ppErrors)[i] = OPC_E_INVALIDITEMID;
			res = S_FALSE;
			continue;
		}

		if( ! item->isWritable() )
		{
			res = S_FALSE;
			(*ppErrors)[i] = OPC_E_BADRIGHTS;
			continue;
		}

		if( pItemVQT[i].vDataValue.vt == VT_EMPTY )
		{
			res = S_FALSE;
			(*ppErrors)[i] = OPC_E_BADTYPE;
			continue;	
		}

		VARIANT tmp;
		::VariantInit( &tmp );
		os::win32::com::Variant::variantCopy( &tmp, &pItemVQT[i].vDataValue );
		(*ppErrors)[i] = ::VariantChangeType( &tmp, &tmp, 0, item->getCanonicalDataType() );
		if( FAILED( (*ppErrors)[i] ) )
		{
			res = S_FALSE;
			continue;
		}
		item->writeFromOPC( tmp );

		if( pItemVQT[i].bQualitySpecified )
		{
			item->setQuality( pItemVQT[i].wQuality );
		}

		if( pItemVQT[i].bTimeStampSpecified )
		{
			item->setTimeStamp( pItemVQT[i].ftTimeStamp );
		}
	}
	return res;
}

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
=======
#include "opc/impl/frl_opc_impl_item_io.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <boost/thread/mutex.hpp>
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "os/win32/com/frl_os_win32_com_allocator.h"
#include "opc/address_space/frl_opc_address_space.h"

namespace frl { namespace opc { namespace impl {

/*! Dtor */
OPCItemIO::~OPCItemIO()
{
}

/*!
	Reads one or more values, qualities and timestamps for the items specified. 
	This is functionally similar to the IOPCSyncIO::Read method.

	\param dwCount
		The number of items to be read.

	\param pszItemIDs
		A list of fully qualified ItemIDs. 
		These might be obtained from IOPCBrowse::GetProperties().

	\param pdwMaxAge
		An array of "staleness" for each item, requested in milliseconds. 
		The server will calculate, for each requested item, the number of milliseconds between "now" 
		and the timestamp on each item. 
		For each item that has not been updated within the last MaxAge milliseconds, 
		must be obtained from the underlying device. 
		Or if the item is not available from the cache, it will also need to be obtained from the underlying device. 
		A max age of 0 is equivalent to OPC_DS_DEVICE and a max age of 0xFFFFFFFF is equivalent to OPC_DS_CACHE. 
		Without existence of a cache the server will always read from device. In this case MaxAge is not relevant. 
		Some servers maintain a global cache for all clients. If the needed item is in this global cache, 
		it is expected that the server makes use of it to check the MaxAge value. 
		Servers should not automatically create or change the caching of an item based on a Read call with MaxAge.
		(\b Note: Since this is a DWORD of milliseconds, the largest MaxAge value would be approximately 49.7 days).

	\param ppvValues
		A pointer to an array of VARIANTs in which the results are  returned. 
		Note that the array and its contained variants must be freed by the client after receipt.

	\param ppwQualities
		An array of Words in which to store the Quality of each result. 
		Note that these must be freed by the client.

	\param ppftTimeStamps
		An array of FILETIMEs in which to store the timestamps of each result. 
		Note that these must be freed by the client.

	\param ppErrors
		Array of HRESULTs indicating the success of the individual item reads. 
		The errors correspond to the ItemIDs  passed in pszItemIDs. 
		This indicates whether the read succeeded in obtaining a defined value, quality and timestamp. 
		NOTE any FAILED error code indicates that the corresponding Value,
		 Quality and Time stamp are UNDEFINED. Note that these must be freed by the client.

	\retval S_OK
		The operation succeeded.

	\retval S_FALSE
		The operation succeeded but there are one or more errors in ppErrors. 
		Refer to individual error returns for more information.

	\retval E_INVALIDARG
		An invalid argument was passed (e.g. dwCount = 0 ).

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory.

	\section ppErrorsCodes
		\arg \b S_OK - Successful Read().
		\arg \b E_FAIL The Read() - failed for this item.
		\arg \b OPC_E_BADRIGHTS - The item is not readable.
		\arg \b OPC_E_UNKNOWNITEMID - The item is no longer available in the server address space.
		\arg \b OPC_E_INVALIDITEMID - The ItemID doesn't conform to the server's syntax.
		\arg \b S_xxx - Vendor specific information can be provided if this item quality is other than GOOD.
							These vendor specific codes can be passed to GetErrorString().
		\arg \b E_xxx - Vendor specific error if this item cannot be accessed.
										These vendor specific codes can be passed to GetErrorString().

	\section OPCItemIO_Read_Comments Comments
		\arg
		The MaxAge will only be tested upon receipt of this call. 
		For example, if 3 items are requested with a MaxAge of 1000 milliseconds 
		and two of the three items are within the appropriate range, 
		then the third item must be obtained from the underlying device. 
		Once this item is obtained, it may take longer than the requested MaxAge, 
		then the three items will be packaged and returned to the client. 
		In this case the original two items that were within the MaxAge threshold are now "stale". 
		The test for MaxAge will not be re-evaluated and therefore the two "stale" items
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
		It is recommended (but not required) that server vendors provide additional information 
		here regarding UNCERTAIN or BAD items.

		\arg
		For any FAILED (E_) result in  ppError the client may not make use of the corresponding Value, 
		Quality and Timestamp except to call VariantClear before destroying the VARIANT. 
		When returning an E_ result in ppError, the Server must set the corresponding 
		Value's VARIANT to VT_EMPTY so that it can be marshaled properly
		 and so that the client can execute VariantClear on it.

		\arg
		Be sure to call VariantClear() on the variants in the Values before freeing the memory containing them.

		\arg
		The client is responsible for freeing all memory associated with the out parameters.
*/
STDMETHODIMP OPCItemIO::Read(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ LPCWSTR *pszItemIDs,
	/* [size_is][in] */ DWORD *pdwMaxAge,
	/* [size_is][size_is][out] */ VARIANT **ppvValues,
	/* [size_is][size_is][out] */ WORD **ppwQualities,
	/* [size_is][size_is][out] */ FILETIME **ppftTimeStamps,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	boost::mutex::scoped_lock guard( scopeGuard );

	if( pszItemIDs == NULL || pdwMaxAge == NULL ||
		ppvValues == NULL || ppwQualities == NULL ||
		ppftTimeStamps == NULL || ppErrors == NULL )
	{
		return E_INVALIDARG;
	}

	*ppvValues = NULL;
	*ppwQualities = NULL;
	*ppftTimeStamps = NULL;
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

	HRESULT res = S_OK;
	address_space::Tag *item = NULL;
	String itemID;
	for( DWORD i = 0; i < dwCount; ++i )
	{
		if( pszItemIDs[i] )
			itemID = similarCompatibility( pszItemIDs[i] );
		try
		{
			item = opcAddressSpace::getInstance().getLeaf( itemID );
		}
		catch( frl::Exception& )
		{
			(*ppErrors)[i] = OPC_E_INVALIDITEMID;
			res = S_FALSE;
			continue;
		}

		if( ! ( item->isReadable() ) )
		{
			res = S_FALSE;
			(*ppErrors)[i] = OPC_E_BADRIGHTS;
			continue;
		}

		(*ppErrors)[i] = os::win32::com::Variant::variantCopy( (*ppvValues)[i], item->read() );
		if( FAILED( (*ppErrors)[i] ) )
		{
			res = S_FALSE;
			continue;
		}
		(*ppwQualities)[i] = item->getQuality();
		(*ppftTimeStamps)[i] = item->getTimeStamp();
	}
	return res;
}

/*!
	Writes one or more values, qualities and timestamps for the items specified. 
	This is functionally similar to the IOPCSyncIO2::WriteVQT except that there is no associated group. 
	If a client attempts to write VQ, VT, or VQT it should expect that the server will write them all or none at all.

	\param dwCount
		The Number of ItemIDs to be written.

	\param pszItemIDs
		The list of ItemIDs to be written.

	\param pItemVQT
		The list of OPCITEMVQT structures. 
		Each structure will potentially contain a value, quality and timestamp to be written to the corresponding ItemID. 
		If the value is equal to VT_EMPTY, then no value should be written. 
		There is a Boolean value associated with each Quality and Timestamp. 
		The Boolean is an indicator as to whether the Quality or Timestamp should be written for the corresponding ItemID. 
		True indicates write, while false indicates do not write.

	\param ppErrors
		The list of errors resulting from the write (1 per item). Note that these must be freed by the client.

	\retval S_OK
		The operations succeeded.

	\retval S_FALSE
		The operation succeeded but there are one or more errors in ppErrors. 
		Refer to individual error returns for more information.

	\retval E_INVALIDARG
		An invalid argument was passed.

	\retval OPC_E_NOTSUPPORTED
		If a client attempts to write any value, quality, timestamp combination 
		and the server does not support the requested combination 
		(which could be a single quantity such as just timestamp), 
		then the server will not perform any write and will return this error code.

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory.

	\section ppErrorCodes
		\arg \b S_OK - The function was successful.
		\arg \b E_FAIL - The function was unsuccessful.
		\arg \b OPC_S_CLAMP - The value was accepted but was clamped.
		\arg \b OPC_E_RANGE - The value was out of range.
		\arg \b OPC_E_BADTYPE - The passed data type cannot be accepted for this item.
		\arg \b OPC_E_BADRIGHTS - The item is not writable.
		\arg \b OPC_E_INVALIDITEMID - The item ID doesn't conform to the server's syntax.
		\arg \b S_xxx - Vendor specific information.
							These vendor specific codes can be passed to GetErrorString().
		\arg \b E_xxx - Vendor specific error.
							These vendor specific codes can be passed to GetErrorString().

	\section OPCItemIO_WriteVQT_Comments Comments
		\arg
		If the HRESULT is S_OK, then ppErrors can be ignored (all results in it are guaranteed to be S_OK).

		\arg
		If the HRESULT is any FAILED code then the contents of ppErrors is undefined.

		\arg
		As an alternative to OPC_E_BADTPYE it is acceptable for the server to return 
		any FAILED error returned by VariantChangeType() or VariantChangeTypeEx().

		\arg
		\b Note: There is no way to validate the writing of the timestamp. 
		If writing the timestamp is supported by the server, then the timestamp 
		will be updated on the device as opposed to the cache. 
		Writing timestamps is generally expected to be used for values 
		which are in some sort of manual override mode or for values which are
		 in some form of holding register. 
		 In general it is not useful to write timestamps for values which are actually 
		 being generated or scanned by the device since the device 
		 will generally re-stamp the data each time it is generated or scanned.
*/
STDMETHODIMP OPCItemIO::WriteVQT(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ LPCWSTR *pszItemIDs,
	/* [size_is][in] */ OPCITEMVQT *pItemVQT,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	boost::mutex::scoped_lock guard( scopeGuard );

	if( pszItemIDs == NULL || pItemVQT == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*ppErrors = NULL;

	if( dwCount == 0 )
		return E_INVALIDARG;

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT res = S_OK;
	address_space::Tag *item = NULL;
	String itemID;
	for( DWORD i = 0; i < dwCount; ++i )
	{
		if( pszItemIDs[i] )
			itemID = similarCompatibility( pszItemIDs[i] );
		try
		{
			item = opcAddressSpace::getInstance().getLeaf( itemID );
		}
		catch( frl::Exception& )
		{
			(*ppErrors)[i] = OPC_E_INVALIDITEMID;
			res = S_FALSE;
			continue;
		}

		if( ! item->isWritable() )
		{
			res = S_FALSE;
			(*ppErrors)[i] = OPC_E_BADRIGHTS;
			continue;
		}

		if( pItemVQT[i].vDataValue.vt == VT_EMPTY )
		{
			res = S_FALSE;
			(*ppErrors)[i] = OPC_E_BADTYPE;
			continue;	
		}

		VARIANT tmp;
		::VariantInit( &tmp );
		os::win32::com::Variant::variantCopy( &tmp, &pItemVQT[i].vDataValue );
		(*ppErrors)[i] = ::VariantChangeType( &tmp, &tmp, 0, item->getCanonicalDataType() );
		if( FAILED( (*ppErrors)[i] ) )
		{
			res = S_FALSE;
			continue;
		}
		item->writeFromOPC( tmp );

		if( pItemVQT[i].bQualitySpecified )
		{
			item->setQuality( pItemVQT[i].wQuality );
		}

		if( pItemVQT[i].bTimeStampSpecified )
		{
			item->setTimeStamp( pItemVQT[i].ftTimeStamp );
		}
	}
	return res;
}

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
>>>>>>> .merge-right.r218
