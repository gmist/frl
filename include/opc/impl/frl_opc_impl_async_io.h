#ifndef frl_opc_ipml_async_io_h_
#define frl_opc_ipml_async_io_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_group_base.h"

namespace frl { namespace opc { namespace impl {

class AsyncIO
	:	public IOPCAsyncIO3,
		virtual public opc::GroupBase
{
public:
	virtual ~AsyncIO();

	// IOPCAsyncIO3 implementation
	STDMETHODIMP Read( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [in] */ DWORD dwTransactionID,
		/* [out] */ DWORD *pdwCancelID,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	STDMETHODIMP Write( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ VARIANT *pItemValues,
		/* [in] */ DWORD dwTransactionID,
		/* [out] */ DWORD *pdwCancelID,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	STDMETHODIMP Refresh2( 
		/* [in] */ OPCDATASOURCE dwSource,
		/* [in] */ DWORD dwTransactionID,
		/* [out] */ DWORD *pdwCancelID);

	STDMETHODIMP Cancel2( 
		/* [in] */ DWORD dwCancelID);;

	STDMETHODIMP SetEnable( 
		/* [in] */ BOOL bEnable);

	STDMETHODIMP GetEnable( 
		/* [out] */ BOOL *pbEnable);

	STDMETHODIMP ReadMaxAge( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ DWORD *pdwMaxAge,
		/* [in] */ DWORD dwTransactionID,
		/* [out] */ DWORD *pdwCancelID,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	STDMETHODIMP WriteVQT( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ OPCITEMVQT *pItemVQT,
		/* [in] */ DWORD dwTransactionID,
		/* [out] */ DWORD *pdwCancelID,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	STDMETHODIMP RefreshMaxAge( 
		/* [in] */ DWORD dwMaxAge,
		/* [in] */ DWORD dwTransactionID,
		/* [out] */ DWORD *pdwCancelID);
}; // AsyncIO

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_ipml_async_io_h_
