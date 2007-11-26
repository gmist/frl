#include "opc/frl_opc_common.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include <WinNT.h>
#include "opc/frl_opc_util.h"
#include "sys/frl_sys_util.h"

namespace frl
{
	namespace opc
	{
		HRESULT STDMETHODCALLTYPE OPCCommon::SetLocaleID( /* [in] */ LCID dwLcid )
		{
			switch (dwLcid)
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
			if (pdwLcid == NULL)
				return E_INVALIDARG;
			*pdwLcid = lcid;
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE OPCCommon::QueryAvailableLocaleIDs( /* [out] */ DWORD *pdwCount, /* [size_is][size_is][out] */ LCID **pdwLcid )
		{
			if (pdwCount == NULL || pdwLcid == NULL)
				return E_INVALIDARG;

			*pdwCount = 6;
			*pdwLcid = (LCID*)CoTaskMemAlloc((*pdwCount)*sizeof(LCID));

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
			if (ppString == NULL )
				return E_INVALIDARG;

			*ppString = NULL;

			LANGID langID = LANGIDFROMLCID(lcid);
			switch (lcid)
			{
			case LOCALE_SYSTEM_DEFAULT:
				{
					langID = GetSystemDefaultLangID();
					break;
				}

			case LOCALE_USER_DEFAULT:
				{
					langID = GetUserDefaultLangID();
					break;
				}

			case LOCALE_INVARIANT:
				{
					langID = LANGIDFROMLCID(LOCALE_NEUTRAL);
					break;
				}
			}

			String cMsg = sys::util::getCodeErrorDescription( langID, dwError );
			
			if( cMsg.size() == 0 )
				return E_INVALIDARG;

			size_t size = ( cMsg.size() + 1 ) * sizeof(WCHAR);
			*ppString = (LPWSTR)CoTaskMemAlloc( size );
			if( ppString == NULL )
				return E_OUTOFMEMORY;			
			wcscpy_s( *ppString, size/sizeof(WCHAR), cMsg.c_str() );
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE OPCCommon::SetClientName( /* [string][in] */ LPCWSTR szName )
		{
			clientName = szName;
			return S_OK;
		}
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
