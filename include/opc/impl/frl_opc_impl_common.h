#ifndef frl_opc_impl_common_h_
#define frl_opc_impl_common_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "frl_types.h"
#include "../dependency/vendors/opc_foundation/opccomn.h"

namespace frl { namespace opc { namespace impl {

class OPCCommon
	:	public IOPCCommon
{
private:
	String clientName;
	DWORD lcid;

public:

	OPCCommon();
	virtual ~OPCCommon();

	// IOPCCommon implementation
	STDMETHODIMP SetLocaleID( 
		/* [in] */ LCID dwLcid);

	STDMETHODIMP GetLocaleID( 
		/* [out] */ LCID *pdwLcid);

	STDMETHODIMP QueryAvailableLocaleIDs( 
		/* [out] */ DWORD *pdwCount,
		/* [size_is][size_is][out] */ LCID **pdwLcid);

	STDMETHODIMP GetErrorString( 
		/* [in] */ HRESULT dwError,
		/* [string][out] */ LPWSTR *ppString);

	STDMETHODIMP SetClientName( 
		/* [string][in] */ LPCWSTR szName);
}; // OPCCommon

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_impl_common_h_
