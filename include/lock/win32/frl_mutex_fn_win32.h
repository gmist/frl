#ifndef FRL_MUTEX_FN_WIN32_H_
#define FRL_MUTEX_FN_WIN32_H_

#include "frl_platform.h"

#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32)

#include <Windows.h>

namespace frl
{
	namespace lock
	{
		typedef ::CRITICAL_SECTION MutexDescriptor;	
	}
}

#endif /* FRL_PLATFORM_WIN32 */

#endif /* FRL_MUTEX_FN_WIN32_H_ */
