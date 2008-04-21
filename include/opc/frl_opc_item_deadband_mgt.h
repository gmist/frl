#ifndef frl_opc_item_deadband_mgt_h_
#define frl_opc_item_deadband_mgt_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"

namespace frl
{
namespace opc
{
template< class T >
class ItemDeadBandMgt : public IOPCItemDeadbandMgt
{
public:
virtual HRESULT STDMETHODCALLTYPE SetItemDeadband( 
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][in] */ FLOAT *pPercentDeadband,
	/* [size_is][size_is][out] */ HRESULT **ppErrors)
	{
		return E_NOTIMPL;
	}

virtual HRESULT STDMETHODCALLTYPE GetItemDeadband( 
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][size_is][out] */ FLOAT **ppPercentDeadband,
	/* [size_is][size_is][out] */ HRESULT **ppErrors)
	{
		return E_NOTIMPL;
	}

virtual HRESULT STDMETHODCALLTYPE ClearItemDeadband( 
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][size_is][out] */ HRESULT **ppErrors)
	{
		return E_NOTIMPL;
	}
};
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_item_deadband_mgt_h_
