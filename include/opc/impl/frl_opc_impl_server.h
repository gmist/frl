#ifndef frl_opc_impl_server_h_
#define frl_opc_impl_server_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_server_base.h"
#include "frl_exception.h"

namespace frl { namespace opc { namespace impl {

/*!
	\brief
		IOPCServer implementation.

	\details
		IOPCServer is the main interface to an OPC server. 
		The OPC server is registered with the operating system as specified in the Installation and Registration Chapter of this specification. 
		This interface must be provided, and all  functions implemented as specified.
*/
class OPCServer
	:	public IOPCServer,
		virtual public opc::OPCServerBase
{
protected:
	OPCSERVERSTATUS serverStatus; //!< Current state

public:
	/*! Subsidiary exception class. */
	FRL_EXCEPTION_CLASS( InvalidServerState );

	virtual ~OPCServer();
	void setServerState( OPCSERVERSTATE newState );
	OPCSERVERSTATE getServerState();

	STDMETHODIMP AddGroup( 
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

	STDMETHODIMP GetErrorString( 
		/* [in] */ HRESULT dwError,
		/* [in] */ LCID dwLocale,
		/* [string][out] */ LPWSTR *ppString);

	STDMETHODIMP GetGroupByName( 
		/* [string][in] */ LPCWSTR szName,
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ LPUNKNOWN *ppUnk);

	STDMETHODIMP GetStatus( 
		/* [out] */ OPCSERVERSTATUS **ppServerStatus);

	STDMETHODIMP RemoveGroup( 
		/* [in] */ OPCHANDLE hServerGroup,
		/* [in] */ BOOL bForce);

	STDMETHODIMP CreateGroupEnumerator( 
		/* [in] */ OPCENUMSCOPE dwScope,
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ LPUNKNOWN *ppUnk);
}; // class OPCServer

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_impl_server_h_
