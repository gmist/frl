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
				itemID = pszItemIDs[i];
			#else
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
				itemID = pszItemIDs[i];
			#else
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
		item->write( tmp );

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
