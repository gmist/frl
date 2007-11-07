#include "frl_types.h"
#include "lock/frl_mutex.h"
#include "frl_exception.h"

namespace frl
{
	namespace lock
	{
		Mutex::Mutex( void )
		{
			MutexInit( mutex );
			isInitialize = True;
		}

		Mutex::~Mutex( void )
		{
			if ( isInitialize )
			{
				MutexDestroy( mutex );
				isInitialize = False;
			}
		}

		void Mutex::Lock( void )
		{
			if( !isInitialize )
				FRL_THROW( FRL_STR( "Mutex not initialize." ) );
			MutexLock( mutex );
		}

		void Mutex::UnLock( void )
		{
			if( !isInitialize )
				FRL_THROW( FRL_STR( "Mutex not initialize." ) );
			MutexUnlock( mutex );
		}

		Bool Mutex::TryLock( void )
		{
			if( !isInitialize )
				FRL_THROW( FRL_STR( "Mutex not initialize." ) );
			return MutexTryLock( mutex );
		}
	} // namespace lock
} // Fat Rat Library
