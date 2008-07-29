#ifndef frl_opc_connection_point_h_
#define frl_opc_connection_point_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <OCIdl.h>
#include "frl_types.h"
#include "os/win32/com/frl_os_win32_com_allocator.h"

namespace frl{ namespace opc{

class ConnectionPointContainer;
class ConnectionPoint : public IConnectionPoint, public os::win32::com::Allocator
{
private:
	// reference counter
	#if( FRL_COMPILER == FRL_COMPILER_MSVC )
		volatile LONG refCount;
	#else
		LONG refCount;
	#endif

	IID iid_interface;
	ConnectionPointContainer* container;
	IUnknown* callBack;
	DWORD cookie;

public:
	ConnectionPoint();
	ConnectionPoint( const IID &iid, ConnectionPointContainer *container_ );
	virtual ~ConnectionPoint();

	IUnknown* getCallBack();
	const IID& getInterface();
	Bool isConnected();

	// IUnknown implementation
	STDMETHODIMP QueryInterface( REFIID iid, LPVOID* ppInterface);
	STDMETHODIMP_(ULONG) AddRef( void);
	STDMETHODIMP_(ULONG) Release( void);

	// IConnectionPoint implementation
	STDMETHODIMP GetConnectionInterface( /* [out] */ IID *pIID );
	STDMETHODIMP GetConnectionPointContainer( /* [out] */ IConnectionPointContainer **ppCPC );
	STDMETHODIMP Advise( /* [in] */ IUnknown *pUnkSink, /* [out] */ DWORD *pdwCookie );
	STDMETHODIMP Unadvise( /* [in] */ DWORD dwCookie );
	STDMETHODIMP EnumConnections( /* [out] */ IEnumConnections **ppEnum );
};

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_connection_point_h_
