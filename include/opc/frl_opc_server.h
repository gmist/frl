#ifndef frl_opc_server_h_
#define frl_opc_server_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <map>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/noncopyable.hpp>
#include "opc/frl_opc_common_impl.h"
#include "opc/frl_opc_item_properties_impl.h"
#include "opc/frl_opc_browse_server_address_space_impl.h"
#include "opc/frl_opc_connection_point_container.h"
#include "os/win32/com/frl_os_win32_com_allocator.h"
#include "opc/frl_opc_browse_impl.h"
#include "opc/frl_opc_item_io_impl.h"
#include "opc/frl_opc_server_impl.h"


namespace frl
{
namespace opc
{

class OPCServer
	:	public OPCCommonImpl,
		public OPCServerImpl,
		public OPCItemPropertiesImpl,
		public BrowseServerAddressSpaceImpl,
		public ConnectionPointContainer,
		public os::win32::com::Allocator,
		public BrowseImpl,
		public OPCItemIO_Impl
{
public:
	FRL_EXCEPTION_CLASS( InvalidServerState );

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
