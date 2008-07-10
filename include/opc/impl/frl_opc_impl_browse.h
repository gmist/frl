#ifndef frl_opc_impl_browse_h_
#define frl_opc_impl_browse_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_server_base.h"

namespace frl { namespace opc { namespace impl {

class BrowseImpl
	:	public IOPCBrowse,
		virtual public opc::OPCServerBase
{
public:
	virtual ~BrowseImpl();

	// IOPCBrowse implementation
	HRESULT STDMETHODCALLTYPE GetProperties( 
		/* [in] */ DWORD dwItemCount,
		/* [size_is][string][in] */ LPWSTR *pszItemIDs,
		/* [in] */ BOOL bReturnPropertyValues,
		/* [in] */ DWORD dwPropertyCount,
		/* [size_is][in] */ DWORD *pdwPropertyIDs,
		/* [size_is][size_is][out] */ OPCITEMPROPERTIES **ppItemProperties );

	HRESULT STDMETHODCALLTYPE Browse( 
		/* [string][in] */ LPWSTR szItemID,
		/* [string][out][in] */ LPWSTR *pszContinuationPoint,
		/* [in] */ DWORD dwMaxElementsReturned,
		/* [in] */ OPCBROWSEFILTER dwBrowseFilter,
		/* [string][in] */ LPWSTR szElementNameFilter,
		/* [string][in] */ LPWSTR szVendorFilter,
		/* [in] */ BOOL bReturnAllProperties,
		/* [in] */ BOOL bReturnPropertyValues,
		/* [in] */ DWORD dwPropertyCount,
		/* [size_is][in] */ DWORD *pdwPropertyIDs,
		/* [out] */ BOOL *pbMoreElements,
		/* [out] */ DWORD *pdwCount,
		/* [size_is][size_is][out] */ OPCBROWSEELEMENT **ppBrowseElements );
}; // class BrowseImpl

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_impl_browse_h_
