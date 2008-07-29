#ifndef frl_opc_server_factory_h_
#define frl_opc_server_factory_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Unknwn.h>
#include "frl_types.h"
#include "os/win32/com/frl_os_win32_com_allocator.h"
#include "frl_singleton.h"

namespace frl{ namespace opc{

class OPCServerFactory : public IClassFactory, public os::win32::com::Allocator
{
private:
	// reference counter
	#if( FRL_COMPILER == FRL_COMPILER_MSVC )
		volatile LONG refCount;
	#else
		LONG refCount;
	#endif

	static LONG serverLocks;
	volatile Bool outProc;
public:
	// Constructor, destructor
	OPCServerFactory();
	virtual ~OPCServerFactory();

	// IUnknown implementation
	STDMETHODIMP QueryInterface( REFIID iid, LPVOID* ppInterface);
	STDMETHODIMP_(ULONG) AddRef( void);
	STDMETHODIMP_(ULONG) Release( void);

	//IClassFactory implementation
	STDMETHODIMP CreateInstance( 
		/* [unique][in] */ IUnknown *pUnkOuter,
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void **ppvObject );

	STDMETHODIMP LockServer(  /* [in] */ BOOL fLock );

	// OPCServerFactory methods
	void isOutProc( Bool isOutProc );
	Bool isServerInUse();
};

static OPCServerFactory factory;

} // namespace opc
} // namespace frl

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_server_factory_h__
