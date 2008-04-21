#ifndef FRL_SEMAPHORE_FN_H_
#define FRL_SEMAPHORE_FN_H_

#include "frl_types.h"

#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#endif

namespace frl
{

	namespace lock
	{
		#if (FRL_PLATFORM ==  FRL_PLATFORM_WIN32 )
			struct SemaphoreDescriptor
			{
				HANDLE semaphore;
				volatile Long curValue;
				SemaphoreDescriptor()
					:	semaphore( INVALID_HANDLE_VALUE ),
						curValue( -1 )
				{}
			};
		#endif

		#if ( FRL_PLATFORM == FRL_PLATFORM_LINUX )
			struct SemaphoreDescriptor
			{
				sem_t semaphore;
				Bool isInitialize;
			};
		#endif

		void semaphoreInit( SemaphoreDescriptor &semaphore, Long value = 0 );
		void semaphoreDestroy( SemaphoreDescriptor &semaphore );
		Long semaphoreGetValue( SemaphoreDescriptor &semaphore );
		void semaphorePost( SemaphoreDescriptor &semaphore );
		void semaphoreWait( SemaphoreDescriptor &semaphore );
		void semaphoreIsValid( SemaphoreDescriptor &semaphore );
		Bool semaphoreTryWait( SemaphoreDescriptor &semaphore );
		Bool semaphoreTimedWait( SemaphoreDescriptor &semaphore, TimeOut time_ );
	}

}// FatRat Library

#endif /* FRL_SEMAPHORE_FN_H_ */
