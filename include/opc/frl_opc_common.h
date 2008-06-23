#ifndef frl_opc_common_h_
#define frl_opc_common_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "frl_types.h"
#include "../dependency/vendors/opc_foundation/opccomn.h"

namespace frl
{
namespace opc
{

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
	virtual HRESULT STDMETHODCALLTYPE SetLocaleID( 
		/* [in] */ LCID dwLcid);

	virtual HRESULT STDMETHODCALLTYPE GetLocaleID( 
		/* [out] */ LCID *pdwLcid);

	virtual HRESULT STDMETHODCALLTYPE QueryAvailableLocaleIDs( 
		/* [out] */ DWORD *pdwCount,
		/* [size_is][size_is][out] */ LCID **pdwLcid);

	virtual HRESULT STDMETHODCALLTYPE GetErrorString( 
		/* [in] */ HRESULT dwError,
		/* [string][out] */ LPWSTR *ppString);

	virtual HRESULT STDMETHODCALLTYPE SetClientName( 
		/* [string][in] */ LPCWSTR szName);
};

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_common_h_
