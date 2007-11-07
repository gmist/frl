#include "frl_platform.h"
#if ( FRL_PLATFORM == FRL_PLATFORM_LINUX )
#include "frl_lock.h"

namespace frl
{
	namespace lock
	{
		void MutexInit( MutexDescriptor &mutex )
		{
			if( pthread_mutex_init( &mutex, NULL ) != 0 )
				FRL_THROW( "Error mutex initialize." );
		}

		void MutexDestroy( MutexDescriptor &mutex )
		{
			pthread_mutex_destroy( &mutex ); //FIXME
		}

		void MutexLock( MutexDescriptor &mutex )
		{
			if( pthread_mutex_lock( &mutex ) != 0 )
				FRL_THROW( "Error mutex locking." );
		}

		void MutexUnlock( MutexDescriptor &mutex )
		{
			if( pthread_mutex_unlock( &mutex ) != 0 )
				FRL_THROW( "Error mutex unlocking." );
		}

		Bool MutexTryLock( MutexDescriptor &mutex )
		{
			return False; // FIXME
		}

	}// namespace lock
}; // Fat Rat Library

#endif /* FRL_PLATFORM_LINUX */
