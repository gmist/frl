#ifndef FRL_RECURSIVE_MUTEX_LINUX_H_
#define FRL_RECURSIVE_MUTEX_LINUX_H_

#include "frl_platform.h"

#if ( FRL_PLATFORM == FRL_PLATFORM_LINUX )

#include <pthread.h>
#include "frl_lock.h"
#include "frl_non_copyable.h"

namespace frl
{
	namespace lock
	{
		class RecursiveMutex
			:	private NonCopyable
		{
		private:
			bool isInitialize;
			pthread_mutex_t mutex;
			pthread_mutexattr_t attribute;
			long count;
		public:
			typedef frl::lock::ScopeGuard< RecursiveMutex > ScopeGuard;
			RecursiveMutex();
			virtual ~RecursiveMutex();

			bool Lock( void );
			bool UnLock( void );
		}; // class RecursiveMutex
	} // namespace lock
} // FatRat Library

#endif /* FRL_PLATFORM_LINUX */

#endif /* FRL_RECURSIVE_MUTEX_LINUX_H_ */
