#include "opc/frl_opc_server.h"
#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include "sys/frl_sys_util.h"

namespace frl
{
	namespace opc
	{	
		HRESULT STDMETHODCALLTYPE OPCServer::AddGroup( /* [string][in] */ LPCWSTR szName, /* [in] */ BOOL bActive, /* [in] */ DWORD dwRequestedUpdateRate, /* [in] */ OPCHANDLE hClientGroup, /* [in][unique] */ LONG *pTimeBias, /* [in][unique] */ FLOAT *pPercentDeadband, /* [in] */ DWORD dwLCID, /* [out] */ OPCHANDLE *phServerGroup, /* [out] */ DWORD *pRevisedUpdateRate, /* [in] */ REFIID riid, /* [iid_is][out] */ LPUNKNOWN *ppUnk )
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE OPCServer::GetErrorString( /* [in] */ HRESULT dwError, /* [in] */ LCID dwLocale, /* [string][out] */ LPWSTR *ppString )
		{
			// check arguments.
			if (ppString == NULL)
			{
				return E_INVALIDARG;
			}

			*ppString = NULL;

			LANGID langID;
			switch (dwLocale)
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

			*ppString = (LPWSTR)CoTaskMemAlloc( cMsg.size() * sizeof(WCHAR));
			wcsncpy_s(*ppString, cMsg.size(), cMsg.c_str(), cMsg.size() );
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE OPCServer::GetGroupByName( /* [string][in] */ LPCWSTR szName, /* [in] */ REFIID riid, /* [iid_is][out] */ LPUNKNOWN *ppUnk )
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE OPCServer::GetStatus( /* [out] */ OPCSERVERSTATUS **ppServerStatus )
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE OPCServer::RemoveGroup( /* [in] */ OPCHANDLE hServerGroup, /* [in] */ BOOL bForce )
		{
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE OPCServer::CreateGroupEnumerator( /* [in] */ OPCENUMSCOPE dwScope, /* [in] */ REFIID riid, /* [iid_is][out] */ LPUNKNOWN *ppUnk )
		{
			return E_NOTIMPL;
		}
		
	} // namespace opc
} // namespace FatRat Library
#endif /* FRL_PLATFORM_WIN32 */
