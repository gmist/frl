#ifndef frl_io_h_header_
#define frl_io_h_header_
#include "frl_platform.h"

#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "os/win32/registry/frl_registry_RootKey.h"
#include "os/win32/registry/frl_registry_RootKeys.h"
#include "os/win32/registry/frl_registry_Key.h"
#endif // FRL_PLATFORM_WIN32

#include "io/fs/frl_fs_fn.h"
#include "io/comm_ports/frl_comm_ports_serial_fh.h"
#include "io/comm_ports/frl_comm_ports_serial.h"

#endif // frl_io_h_header_
