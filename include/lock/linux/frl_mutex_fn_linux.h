#ifndef FRL_MUTEX_FN_LINUX_H_
#define FRL_MUTEX_FN_LINUX_H_

#include "frl_platform.h"

#if( FRL_PLATFORM == FRL_PLATFORM_LINUX )
namespace frl
{
	namespace lock
	{
		typedef pthread_mutex_t MutexDescriptor;
	}
}

#endif /* FRL_PLATFORM_LINUX */

#endif /* FRL_MUTEX_FN_LINUX_H_ */
