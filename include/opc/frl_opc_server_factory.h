#ifndef frl_opc_server_factory_h_
#define frl_opc_server_factory_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Unknwn.h>
#include "frl_types.h"
#include "os/win32/com/frl_os_win32_com_allocator.h"

namespace frl
{
	namespace opc
	{
		class OPCServerFactory : public IClassFactory, public os::win32::com::Allocator
		{
		private:
			volatile LONG refCount;
			static LONG serverLocks;
			volatile Bool outProc;
			volatile Bool noMoreServers;
		public:
			// Constructor, destructor
			OPCServerFactory();
			~OPCServerFactory();

			// IUnknown implementation
			STDMETHODIMP QueryInterface( REFIID iid, LPVOID* ppInterface);
			STDMETHODIMP_(ULONG) AddRef( void);
			STDMETHODIMP_(ULONG) Release( void);

			//IClassFactory implementation
			HRESULT STDMETHODCALLTYPE CreateInstance( 
				/* [unique][in] */ IUnknown *pUnkOuter,
				/* [in] */ REFIID riid,
				/* [iid_is][out] */ void **ppvObject );

			HRESULT STDMETHODCALLTYPE LockServer(  /* [in] */ BOOL fLock );

			// OPCServerFactory methods
			void isOutProc( Bool isOutProc );
			void usageServer();
			Bool isServerInUse();
		};

		static OPCServerFactory factory;
	} // namespace opc
} // namespace frl

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_server_factory_h__
