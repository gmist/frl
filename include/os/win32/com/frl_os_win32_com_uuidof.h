#ifndef frl_os_win32_com_uuidof_h_
#define frl_os_win32_com_uuidof_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )

#if ! ( defined __uuidof )
#define __uuidof( interface_ ) IID_##interface_
#endif

#endif // FRL_PLATFORM == FRL_PLATFORM_WIN32
#endif // frl_os_win32_com_uuidof_h_
