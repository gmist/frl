#if defined ( __linux__ ) || defined ( __linux )

#include "lock/frl_rw_mutex.h"

namespace frl
{
	namespace lock
	{
		RWMutex::RWMutex()
		{
		}

		RWMutex::RWMutex( bool many_writers )
			:	isInitialize( false )
		{
			pthread_mutexattr_init( &attribute );
			pthread_mutexattr_settype( &attribute, PTHREAD_MUTEX_RECURSIVE );
		}
		RWMutex::~RWMutex( void )
		{
			if( isInitialize )
				pthread_mutex_destroy( &mutex );
		}

	}// namespace lock
}// namespace FatRat Library

#endif /* __linux__ */

