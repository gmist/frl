#ifndef frl_mutex_fn_win32_h_
#define frl_mutex_fn_win32_h_

#include "frl_platform.h"

#if( FRL_PLATFORM == FRL_PLATFORM_WIN32)
#include <Windows.h>

namespace frl
{
namespace lock
{
	typedef ::CRITICAL_SECTION MutexDescriptor;	
} // namespace lock
} // FatRat Library

#endif // FRL_PLATFORM_WIN32

#endif // frl_mutex_fn_win32_h_
