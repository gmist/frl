#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_LINUX )
#include "frl_lock.h"

namespace frl
{
	namespace lock
	{
		void mutexInit( MutexDescriptor &mutex )
		{
			if( pthread_mutex_init( &mutex, NULL ) != 0 )
				FRL_THROW( "Error mutex initialize." );
		}

		void mutexDestroy( MutexDescriptor &mutex )
		{
			pthread_mutex_destroy( &mutex ); //FIXME
		}

		void mutexLock( MutexDescriptor &mutex )
		{
			if( pthread_mutex_lock( &mutex ) != 0 )
				FRL_THROW( "Error mutex locking." );
		}

		void mutexUnlock( MutexDescriptor &mutex )
		{
			if( pthread_mutex_unlock( &mutex ) != 0 )
				FRL_THROW( "Error mutex unlocking." );
		}

		Bool mutexTryLock( MutexDescriptor &mutex )
		{
			return False; // FIXME
		}

	}// namespace lock
}; // FatRat Library

#endif /* FRL_PLATFORM_LINUX */
