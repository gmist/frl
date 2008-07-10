#ifndef frl_opc_impl_item_deadband_mgt_h_
#define frl_opc_impl_item_deadband_mgt_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_group_base.h"

namespace frl { namespace opc { namespace impl {

class ItemDeadBandMgt
	:	public IOPCItemDeadbandMgt,
		virtual public opc::GroupBase
{
public:
	virtual ~ItemDeadBandMgt();

	HRESULT STDMETHODCALLTYPE SetItemDeadband( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ FLOAT *pPercentDeadband,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	HRESULT STDMETHODCALLTYPE GetItemDeadband( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][size_is][out] */ FLOAT **ppPercentDeadband,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	HRESULT STDMETHODCALLTYPE ClearItemDeadband( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);
}; // class ItemDeadBandMgt

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_impl_item_deadband_mgt_h_
