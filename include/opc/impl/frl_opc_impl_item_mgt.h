#ifndef frl_opc_impl_item_mgt_h_
#define frl_opc_impl_item_mgt_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_group_base.h"

namespace frl { namespace opc { namespace impl {

class ItemMgt
	:	public IOPCItemMgt,
		virtual public opc::GroupBase
{
public:
	virtual ~ItemMgt();
	HRESULT STDMETHODCALLTYPE AddItems( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCITEMDEF *pItemArray,
		/* [size_is][size_is][out] */ OPCITEMRESULT **ppAddResults,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	HRESULT STDMETHODCALLTYPE ValidateItems( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCITEMDEF *pItemArray,
		/* [in] */ BOOL bBlobUpdate,
		/* [size_is][size_is][out] */ OPCITEMRESULT **ppValidationResults,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	HRESULT STDMETHODCALLTYPE RemoveItems( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	HRESULT STDMETHODCALLTYPE SetActiveState( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [in] */ BOOL bActive,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	HRESULT STDMETHODCALLTYPE SetClientHandles( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ OPCHANDLE *phClient,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	HRESULT STDMETHODCALLTYPE SetDatatypes( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ VARTYPE *pRequestedDatatypes,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	HRESULT STDMETHODCALLTYPE CreateEnumerator( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ LPUNKNOWN *ppUnk);

}; // class ItemMgt

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_impl_item_mgt_h_
