#ifndef frl_opc_async_io2_h_
#define frl_opc_async_io2_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"

namespace frl
{
	namespace opc
	{
		template < class T >
		class AsyncIO2 : public IOPCAsyncIO2
		{
		public:
			HRESULT STDMETHODCALLTYPE Read( 
				/* [in] */ DWORD dwCount,
				/* [size_is][in] */ OPCHANDLE *phServer,
				/* [in] */ DWORD dwTransactionID,
				/* [out] */ DWORD *pdwCancelID,
				/* [size_is][size_is][out] */ HRESULT **ppErrors)
			{
				return E_NOTIMPL;
			}

			HRESULT STDMETHODCALLTYPE Write( 
				/* [in] */ DWORD dwCount,
				/* [size_is][in] */ OPCHANDLE *phServer,
				/* [size_is][in] */ VARIANT *pItemValues,
				/* [in] */ DWORD dwTransactionID,
				/* [out] */ DWORD *pdwCancelID,
				/* [size_is][size_is][out] */ HRESULT **ppErrors)
			{
				return E_NOTIMPL;
			}

			HRESULT STDMETHODCALLTYPE Refresh2( 
				/* [in] */ OPCDATASOURCE dwSource,
				/* [in] */ DWORD dwTransactionID,
				/* [out] */ DWORD *pdwCancelID)
			{
				return E_NOTIMPL;
			}

			HRESULT STDMETHODCALLTYPE Cancel2( 
				/* [in] */ DWORD dwCancelID)
			{
				return E_NOTIMPL;
			};

			HRESULT STDMETHODCALLTYPE SetEnable( 
				/* [in] */ BOOL bEnable)
			{
				return E_NOTIMPL;
			}

			HRESULT STDMETHODCALLTYPE GetEnable( 
				/* [out] */ BOOL *pbEnable)
			{
				return E_NOTIMPL;
			}
		};
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_async_io2_h_ */
