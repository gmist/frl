#ifndef frl_opc_item_properties_impl_h_
#define frl_opc_item_properties_impl_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"

namespace frl
{
namespace opc
{
class OPCItemPropertiesImpl : public IOPCItemProperties
{
public:
	virtual ~OPCItemPropertiesImpl();

	virtual HRESULT STDMETHODCALLTYPE QueryAvailableProperties( 
		/* [in] */ LPWSTR szItemID,
		/* [out] */ DWORD *pdwCount,
		/* [size_is][size_is][out] */ DWORD **ppPropertyIDs,
		/* [size_is][size_is][out] */ LPWSTR **ppDescriptions,
		/* [size_is][size_is][out] */ VARTYPE **ppvtDataTypes);

	virtual HRESULT STDMETHODCALLTYPE GetItemProperties( 
		/* [in] */ LPWSTR szItemID,
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ DWORD *pdwPropertyIDs,
		/* [size_is][size_is][out] */ VARIANT **ppvData,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	virtual HRESULT STDMETHODCALLTYPE LookupItemIDs( 
		/* [in] */ LPWSTR szItemID,
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ DWORD *pdwPropertyIDs,
		/* [size_is][size_is][string][out] */ LPWSTR **ppszNewItemIDs,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);
}; // class OPCItemPropertiesImpl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_item_properties_impl_h_
