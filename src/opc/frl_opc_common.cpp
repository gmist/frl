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
			// check that the locale is supported.
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
			// invalid arguments - return error.
			if (pdwLcid == NULL)
			{
				return E_INVALIDARG;
			}
			*pdwLcid = lcid;
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE OPCCommon::QueryAvailableLocaleIDs( /* [out] */ DWORD *pdwCount, /* [size_is][size_is][out] */ LCID **pdwLcid )
		{
			if (pdwCount == NULL || pdwLcid == NULL)
			{
				return E_INVALIDARG;
			}
			
			*pdwCount = 4;
			*pdwLcid = (LCID*)CoTaskMemAlloc((*pdwCount)*sizeof(LCID));
			
			(*pdwLcid)[0] = LOCALE_SYSTEM_DEFAULT;
			(*pdwLcid)[1] = LOCALE_USER_DEFAULT;
			(*pdwLcid)[2] = LOCALE_NEUTRAL;
			(*pdwLcid)[3] = LOCALE_INVARIANT;			
			
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE OPCCommon::GetErrorString( /* [in] */ HRESULT dwError, /* [string][out] */ LPWSTR *ppString )
		{
			// check arguments.
			if (ppString == NULL)
			{
				return E_INVALIDARG;
			}

			*ppString = NULL;

			LANGID langID;

			// lookup languages for 'special' locales.
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
			default:
					langID = LANGIDFROMLCID(LOCALE_NEUTRAL);
			}

			String cMsg = sys::util::getCodeErrorDescription( langID, dwError );

			// allocate string for return.			
			*ppString = (LPWSTR)CoTaskMemAlloc( cMsg.size() * sizeof(WCHAR));
			wcsncpy_s(*ppString, cMsg.size(), cMsg.c_str(), cMsg.size() );
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
