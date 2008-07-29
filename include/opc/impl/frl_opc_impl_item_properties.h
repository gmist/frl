#ifndef frl_opc_impl_item_properties_h_
#define frl_opc_impl_item_properties_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"

namespace frl { namespace opc { namespace impl {

class OPCItemProperties
	:	public IOPCItemProperties
{
public:
	virtual ~OPCItemProperties();

	STDMETHODIMP QueryAvailableProperties( 
		/* [in] */ LPWSTR szItemID,
		/* [out] */ DWORD *pdwCount,
		/* [size_is][size_is][out] */ DWORD **ppPropertyIDs,
		/* [size_is][size_is][out] */ LPWSTR **ppDescriptions,
		/* [size_is][size_is][out] */ VARTYPE **ppvtDataTypes);

	STDMETHODIMP GetItemProperties( 
		/* [in] */ LPWSTR szItemID,
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ DWORD *pdwPropertyIDs,
		/* [size_is][size_is][out] */ VARIANT **ppvData,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	STDMETHODIMP LookupItemIDs( 
		/* [in] */ LPWSTR szItemID,
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ DWORD *pdwPropertyIDs,
		/* [size_is][size_is][string][out] */ LPWSTR **ppszNewItemIDs,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);
}; // class OPCItemProperties

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_impl_item_properties_h_
