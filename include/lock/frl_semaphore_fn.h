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

		void SemaphoreInit( SemaphoreDescriptor &semaphore, Long value = 0 );
		void SemaphoreDestroy( SemaphoreDescriptor &semaphore );
		Long SemaphoreGetValue( SemaphoreDescriptor &semaphore );
		void SemaphorePost( SemaphoreDescriptor &semaphore );
		void SemaphoreWait( SemaphoreDescriptor &semaphore );
		void SemaphoreIsValid( SemaphoreDescriptor &semaphore );
		Bool SemaphoreTryWait( SemaphoreDescriptor &semaphore );

		#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
		Bool SemaphoreTimedWait( SemaphoreDescriptor &semaphore, DWORD time_ );
		#endif

		#if ( FRL_PLATFORM == FRL_PLATFORM_LINUX )
		Bool SemaphoreTimedWait( SemaphoreDescriptor &semaphore, unsigned long time_ );
		#endif
	}

}// Fat Rat Library

#endif /* FRL_SEMAPHORE_FN_H_ */
