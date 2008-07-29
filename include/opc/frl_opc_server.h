#ifndef frl_opc_server_h_
#define frl_opc_server_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/impl/frl_opc_impl_common.h"
#include "opc/impl/frl_opc_impl_item_properties.h"
#include "opc/impl/frl_opc_impl_browse_server_address_space.h"
#include "opc/impl/frl_opc_impl_browse.h"
#include "opc/impl/frl_opc_impl_item_io.h"
#include "opc/impl/frl_opc_impl_server.h"
#include "opc/frl_opc_connection_point_container.h"
#include "os/win32/com/frl_os_win32_com_allocator.h"

namespace frl{ namespace opc{

class OPCServer
	:	public impl::OPCCommon,
		public impl::OPCServer,
		public impl::OPCItemProperties,
		public impl::BrowseServerAddressSpace,
		public impl::BrowseImpl,
		public impl::OPCItemIO,
		public ConnectionPointContainer,
		public os::win32::com::Allocator
{
private:
	// reference counter
	#if( FRL_COMPILER == FRL_COMPILER_MSVC )
		volatile LONG refCount;
	#else
		LONG refCount;
	#endif
public:
	OPCServer();
	virtual ~OPCServer();

	// IUnknown implementation
	STDMETHODIMP QueryInterface( REFIID iid, LPVOID* ppInterface);
	STDMETHODIMP_(ULONG) AddRef( void);
	STDMETHODIMP_(ULONG) Release( void);
}; // OPCServer

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_server_h_
