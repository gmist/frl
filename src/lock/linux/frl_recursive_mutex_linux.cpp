#include "frl_platform.h"

#if ( FRL_PLATFORM == FRL_PLATFORM_LINUX )
#include "lock/frl_recursive_mutex.h"

namespace frl
{
	namespace lock
	{

			RecursiveMutex::RecursiveMutex( void )
				:	isInitialize( false ),
					count( 0 )
			{
				if( pthread_mutexattr_init( &attribute ) !=0 )
					return;
				if( pthread_mutexattr_settype( &attribute, PTHREAD_MUTEX_RECURSIVE ) != 0 )
					return;
				isInitialize = true;
				if( pthread_mutex_init( &mutex, &attribute ) != 0 )
				{
					pthread_mutexattr_destroy( &attribute );
					return;
				}
				pthread_mutexattr_destroy( &attribute );
				isInitialize = true;
			}

			RecursiveMutex::~RecursiveMutex( void )
			{
				if( isInitialize )
				{
					pthread_mutex_destroy( &mutex );
				}
			}

			Bool RecursiveMutex::lock( void )
			{
				if( !isInitialize )
					return false;
				if( pthread_mutex_lock( &mutex ) != 0 )
					return false;
				if( ++count > 1 )
				{
					if( pthread_mutex_unlock( &mutex ) != 0 )
						return false;
				}
				return true;
			}

			Bool RecursiveMutex::unLock( void )
			{
				if( !isInitialize )
					return false;
				if( --count == 0 )
				{
					if( pthread_mutex_unlock( &mutex ) != 0 )
						return false;
				}
				return true;
			}
	}// namespace lock
}; // FatRat Library

#endif /* FRL_PLATFORM_LINUX */
