#ifndef frl_opc_server_impl_h_
#define frl_opc_server_impl_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_server_base.h"

namespace frl
{
namespace opc
{

class OPCServer;
class OPCServerImpl
	:	virtual public OPCServerBase,
		virtual public IOPCServer
{
public:
	virtual ~OPCServerImpl();

	// IOPCServer implementation
	HRESULT STDMETHODCALLTYPE AddGroup( 
		/* [string][in] */ LPCWSTR szName,
		/* [in] */ BOOL bActive,
		/* [in] */ DWORD dwRequestedUpdateRate,
		/* [in] */ OPCHANDLE hClientGroup,
		/* [in][unique] */ LONG *pTimeBias,
		/* [in][unique] */ FLOAT *pPercentDeadband,
		/* [in] */ DWORD dwLCID,
		/* [out] */ OPCHANDLE *phServerGroup,
		/* [out] */ DWORD *pRevisedUpdateRate,
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ LPUNKNOWN *ppUnk);

	HRESULT STDMETHODCALLTYPE GetErrorString( 
		/* [in] */ HRESULT dwError,
		/* [in] */ LCID dwLocale,
		/* [string][out] */ LPWSTR *ppString);

	HRESULT STDMETHODCALLTYPE GetGroupByName( 
		/* [string][in] */ LPCWSTR szName,
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ LPUNKNOWN *ppUnk);

	HRESULT STDMETHODCALLTYPE GetStatus( 
		/* [out] */ OPCSERVERSTATUS **ppServerStatus);

	HRESULT STDMETHODCALLTYPE RemoveGroup( 
		/* [in] */ OPCHANDLE hServerGroup,
		/* [in] */ BOOL bForce);

	HRESULT STDMETHODCALLTYPE CreateGroupEnumerator( 
		/* [in] */ OPCENUMSCOPE dwScope,
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ LPUNKNOWN *ppUnk);
};
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_server_impl_h_
