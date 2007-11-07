#ifndef FRL_RECURCIVE_MUTEX_WIN32_H_
#define FRL_RECURCIVE_MUTEX_WIN32_H_

#include "frl_platform.h"

#if (FRL_PLATFORM ==  FRL_PLATFORM_WIN32 )

#include "lock/frl_mutex.h"

namespace frl
{
	namespace lock
	{
		class RecursiveMutex
			: public Mutex
		{

		}; // class RecursiveMutex
	}
}

#endif /* FRL_PLATFORM_WIN32 */

#endif /*FRL_RECURCIVE_MUTEX_WIN32_H_*/
