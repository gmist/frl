#ifndef frl_opc_impl_sync_io_h_
#define frl_opc_impl_sync_io_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_group_base.h"

namespace frl { namespace opc { namespace impl {

class SyncIO
	:	public IOPCSyncIO2,
		virtual public GroupBase
{
public:
	virtual ~SyncIO();

	STDMETHODIMP Read( 
		/* [in] */ OPCDATASOURCE dwSource,
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][size_is][out] */ OPCITEMSTATE **ppItemValues,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);;

	STDMETHODIMP Write( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ VARIANT *pItemValues,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	STDMETHODIMP ReadMaxAge( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ DWORD *pdwMaxAge,
		/* [size_is][size_is][out] */ VARIANT **ppvValues,
		/* [size_is][size_is][out] */ WORD **ppwQualities,
		/* [size_is][size_is][out] */ FILETIME **ppftTimeStamps,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	STDMETHODIMP WriteVQT( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ OPCITEMVQT *pItemVQT,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);
};

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_impl_sync_io_h_
