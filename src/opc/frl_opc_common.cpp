#include "opc/frl_opc_common.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <WinNT.h>
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "os/win32/com/frl_os_win32_com_allocator.h"
#include "opc/frl_opc_util.h"
#include "sys/frl_sys_util.h"

namespace frl
{
namespace opc
{
OPCCommon::OPCCommon() : lcid( 0 )
{
}

OPCCommon::~OPCCommon()
{
}

HRESULT STDMETHODCALLTYPE OPCCommon::SetLocaleID( /* [in] */ LCID dwLcid )
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

HRESULT STDMETHODCALLTYPE OPCCommon::GetLocaleID( /* [out] */ LCID *pdwLcid )
{
	if( pdwLcid == NULL )
		return E_INVALIDARG;
	*pdwLcid = lcid;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE OPCCommon::QueryAvailableLocaleIDs( /* [out] */ DWORD *pdwCount, /* [size_is][size_is][out] */ LCID **pdwLcid )
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

HRESULT STDMETHODCALLTYPE OPCCommon::GetErrorString( /* [in] */ HRESULT dwError, /* [string][out] */ LPWSTR *ppString )
{
	if( ppString == NULL )
		return E_INVALIDARG;

	*ppString = NULL;
	return util::getErrorString( dwError, lcid, &ppString );
}

HRESULT STDMETHODCALLTYPE OPCCommon::SetClientName( /* [string][in] */ LPCWSTR szName )
{
	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		clientName = szName;
	#else
		clientName = wstring2string( szName );
	#endif

	return S_OK;
}
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
