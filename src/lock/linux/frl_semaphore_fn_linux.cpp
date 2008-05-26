#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_LINUX )
#include <time.h>
#include <sys/time.h>
#include "frl_lock.h"
namespace frl
{
namespace lock
{
void semaphoreInit( SemaphoreDescriptor &semaphore, Long value_ )
{
	FRL_EXCEPT_GUARD();
	if ( semaphore.isInitialize )
		FRL_THROW( "Semaphore already initialize." );
	
	if( sem_init( &semaphore.semaphore, 0, value_ ) != 0 )
		FRL_THROW( "Semaphore initialize error." );
}

void semaphoreIsValid( SemaphoreDescriptor &semaphore )
{
	FRL_EXCEPT_GUARD();
	if ( ! semaphore.isInitialize)
		FRL_THROW( "Semaphore not valid. Maybe semaphore not initialize." );
}

void semaphoreDestroy( SemaphoreDescriptor &semaphore )
{
	FRL_EXCEPT_GUARD();
	semaphoreIsValid( semaphore );
	if( sem_destroy( &semaphore.semaphore ) != 0 )
		FRL_THROW( "Semaphore destroy error." );
	semaphore.isInitialize = False;
}

Long semaphoreGetValue( SemaphoreDescriptor &semaphore )
{
	FRL_EXCEPT_GUARD();
	semaphoreIsValid( semaphore );
	frl::Long value = -1;
	sem_getvalue( &semaphore.semaphore ,reinterpret_cast<int*>( &value ) );
	return value;
}

void semaphorePost( SemaphoreDescriptor &semaphore )
{
	FRL_EXCEPT_GUARD();
	semaphoreIsValid( semaphore );
	if( sem_post( &semaphore ) != 0 )
		FRL_THROW("Semaphore posting error." );
}

void semaphoreWait( SemaphoreDescriptor &semaphore )
{
	FRL_EXCEPT_GUARD();
	semaphoreIsValid( semaphore );
	sem_wait( &semaphore.semaphore ); // FIXME return value
}

Bool semaphoreTryWait( SemaphoreDescriptor &semaphore )
{
	FRL_EXCEPT_GUARD();
	semaphoreIsValid( semaphore );
	if( sem_trywait( &semaphore ) != 0 )
		FRL_THROW( "Semaphore try-wait error." );
}

Bool semaphoreTimedWait( SemaphoreDescriptor &semaphore, frl::TimeOut time_ )
{
	FRL_EXCEPT_GUARD();
	semaphoreIsValid( semaphore );
	timespec ts;
	timeval cur_time_val;
	gettimeofday( &cur_time_val, NULL );
	ts.tv_sec = cur_time_val.tv_sec + time_ / 1000;
	ts.tv_nsec = cur_time_val.tv_usec * 1000 + time_ % 1000;

	if( sem_timedwait( &semaphore, &ts) != 0 )
		FRL_THROW( "Semaphore timed wait error." );
}


} // namespace lock
} // FatRat Library

#endif /* FRL_PLATFORM_LINUX */
