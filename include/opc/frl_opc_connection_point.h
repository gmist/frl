#ifndef frl_opc_connection_point_h_
#define frl_opc_connection_point_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <OCIdl.h>
#include "frl_types.h"
#include "opc\frl_opc_com_allocator.h"

namespace frl
{
	namespace opc
	{
		class ConnectionPointContainer;
		class ConnectionPoint : public IConnectionPoint, public ComAllocator
		{
		private:
			volatile LONG refCount;
			IID iid_interface;
			ConnectionPointContainer* container;
			IUnknown* callBack;
			DWORD cookie;

		public:
			ConnectionPoint();
			ConnectionPoint( const IID &iid, ConnectionPointContainer *container_ );
			~ConnectionPoint();

			IUnknown* getCallBack();
			const IID& getInterface();
			Bool isConnected();
			Bool deleting();

			// IUnknown implementation
			STDMETHODIMP QueryInterface( REFIID iid, LPVOID* ppInterface);
			STDMETHODIMP_(ULONG) AddRef( void);
			STDMETHODIMP_(ULONG) Release( void);

			// IConnectionPoint implementation
			HRESULT STDMETHODCALLTYPE GetConnectionInterface( 
			/* [out] */ IID *pIID);

			HRESULT STDMETHODCALLTYPE GetConnectionPointContainer( 
			/* [out] */ IConnectionPointContainer **ppCPC);

			HRESULT STDMETHODCALLTYPE Advise( 
			/* [in] */ IUnknown *pUnkSink,
			/* [out] */ DWORD *pdwCookie);

			HRESULT STDMETHODCALLTYPE Unadvise( 
			/* [in] */ DWORD dwCookie);

			HRESULT STDMETHODCALLTYPE EnumConnections( 
			/* [out] */ IEnumConnections **ppEnum);
		};
	} // namespace opc
} // namespace FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_connection_point_h_