#include "opc/impl/frl_opc_impl_common.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <WinNT.h>
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "os/win32/com/frl_os_win32_com_allocator.h"
#include "opc/frl_opc_util.h"
#include "sys/frl_sys_util.h"

namespace frl { namespace opc { namespace impl {

OPCCommon::OPCCommon() : lcid( 0 )
{
}

OPCCommon::~OPCCommon()
{
}

/*!
	Set the default LocaleID for this server/client session. 
	This LocaleID will be used by the GetErrorString method on this interface.
	It should also be used as the 'default' LocaleID by any other server functions that are affected by LocaleID.
	Other OPC interfaces may provide additional LocaleID capability by allowing this LocalID
	to be overridden either via a parameter to a method or via a property on a child object.

	\param dwLcid
		The default LocaleID for this server/client session

	\retval E_FAIL
		The operation failed.

	\retval E_INVALIDARG
		An argument to the function was invalid. (For example, the LocaleID specified is not valid).

	\retval S_OK
		The operation succeeded.
*/
STDMETHODIMP OPCCommon::SetLocaleID( /* [in] */ LCID dwLcid )
{
	switch( dwLcid )
	{
	case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT):
	case MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL), SORT_DEFAULT):
	case LOCALE_NEUTRAL:
	case LOCALE_INVARIANT:
	case LOCALE_SYSTEM_DEFAULT:
	case LOCALE_USER_DEFAULT:
		{
			lcid = dwLcid;
			return S_OK;
		}
	}
	return E_INVALIDARG;
}

/*!
	Return the default LocaleID for this server/client session. 

	\param pdwLcid
		Where to return the default LocaleID for this server/client session

	\retval E_FAIL
		The operation failed.

	\retval E_INVALIDARG
		An argument to the function was invalid 
		(for example, the passed pointer is not valid).

	\retval S_OK
		The operation succeeded.
*/
STDMETHODIMP OPCCommon::GetLocaleID( /* [out] */ LCID *pdwLcid )
{
	if( pdwLcid == NULL )
		return E_INVALIDARG;
	*pdwLcid = lcid;
	return S_OK;
}

/*!
	Return the available LocaleIDs for this server/client session. 

	\param pdwCount
		Where to return the LocaleID count.

	\param pdwLcid
		Where to return the LocaleID list.

	\retval E_FAIL
		The operation failed.

	\retval E_INVALIDARG
		An argument to the function was invalid. (For example, the passed pointer is not valid).

	\retval S_OK
		The operation succeeded.
*/
STDMETHODIMP OPCCommon::QueryAvailableLocaleIDs( /* [out] */ DWORD *pdwCount, /* [size_is][size_is][out] */ LCID **pdwLcid )
{
	if( pdwCount == NULL || pdwLcid == NULL )
		return E_INVALIDARG;

	*pdwCount = 6;
	*pdwLcid = os::win32::com::allocMemory< LCID >( *pdwCount );

	(*pdwLcid)[0] = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
	(*pdwLcid)[1] = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL), SORT_DEFAULT);
	(*pdwLcid)[2] = LOCALE_SYSTEM_DEFAULT;
	(*pdwLcid)[3] = LOCALE_USER_DEFAULT;
	(*pdwLcid)[4] = LOCALE_NEUTRAL;
	(*pdwLcid)[5] = LOCALE_INVARIANT;

	return S_OK;
}

/*!
	Returns the error string for a server specific error code.

	\param dwError
		A server specific error code that the client application had 
		returned from an interface function from the server, and for 
		which the client application is requesting the server’s textual representation. 
	
	\param ppString
		Pointer to pointer where server supplied result will be saved.

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory.

	\retval E_INVALIDARG
		An argument to the function was invalid 
		(for example, the error code specified is not valid).

	\retval S_OK
		The operation succeeded.
*/

STDMETHODIMP OPCCommon::GetErrorString( /* [in] */ HRESULT dwError, /* [string][out] */ LPWSTR *ppString )
{
	if( ppString == NULL )
		return E_INVALIDARG;

	*ppString = NULL;
	return util::getErrorString( dwError, lcid, &ppString );
}

/*!
	Allows the client to optionally register a client name with the server. 
	This is included primarily for debugging purposes.
	The recommended behavior is that the client set his Node name and EXE name here. 

	\param szName
		An arbitrary string containing information about the client task.

	\retval E_FAIL
		The operation failed.

	\retval E_INVALIDARG
		An argument to the function was invalid 
		(for example, the pointer specified is not valid).

	\retval S_OK
		The operation succeeded.
*/
STDMETHODIMP OPCCommon::SetClientName( /* [string][in] */ LPCWSTR szName )
{
	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		clientName = szName;
	#else
		clientName = wstring2string( szName );
	#endif

	return S_OK;
}

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
