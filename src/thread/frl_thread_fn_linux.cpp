#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_LINUX )
#include "thread/frl_thread_fn.h"
#include "frl_exception.h"

namespace frl
{
namespace thread
{
void isValid( const ThreadDescriptor &descriptor )
{
	FRL_EXCEPT_GUARD();
	if( descriptor == 0 )
		FRL_THROW("Thread descriptor not valid.");
}

void yield( void )
{
	::pthread_yield();
}

void join( ThreadDescriptor &descriptor )
{
	FRL_EXCEPT_GUARD();
	isValid( descriptor );
	pthread_join( descriptor, NULL ); // FIXME
}

Bool join( ThreadDescriptor &descriptor, TimeOut msec_ )
{
	FRL_EXCEPT_GUARD();
	isValid( descriptor );
	pthread_join( descriptor, msec_ ); // FIXME
}

void kill( ThreadDescriptor &descriptor )
{
	FRL_EXCEPT_GUARD();
	isValid( descriptor );
	pthread_cancel( id ); // FIXME
	descriptor = 0;
}

Bool isEqual( const ThreadDescriptor &rvl, const ThreadDescriptor &lvl )
{
	FRL_EXCEPT_GUARD();
	isValid( rvl );
	isValid( lvl );
	return ::pthread_equal( id, r_val.id ) !=0;
}

// TODO: «‡ÏÂÌËÚ¸ void* parameter_ Ì‡ template class< T >
ThreadDescriptor create( FRL_LPTHREAD_START_ROUTINE startRoutine, void* parameter_ = NULL, Bool isDetached_ = False, UInt stackSize_ = 0 )
{
	FRL_EXCEPT_GUARD();
	pthread_attr_t attribute;
	pthread_attr_init( &attribute );

	if( stackSize_ )
		if ( pthread_attr_setstacksize( &attribute, stackSize_ ) !=0 )
		{
			pthread_attr_destroy( &attribute );
			FRL_THROW( "Òreate thread error. Do not set stack size." );
		}
		
		if( isDetached_ )
		{
			if( pthread_attr_setdetachstate( &attribute, PTHREAD_CREATE_DETACHED ) !=0 )
			{
				pthread_attr_destroy( &attribute );
				FRL_THROW( "Òreate thread error. Do not set detached attribute." );
			}
		}

		pthread_testcancel();
		
		ThreadDescriptor id;
		if( pthread_create( &id, &attribute, startRoutine, parameter_ ) !=0 )
		{
			pthread_attr_destroy( &attribute );
			FRL_THROW( "Òreate thread error." );
		}
		pthread_attr_destroy( &attribute );
		return id;
}

frl::ULong getThreadId()
{
	return pthread_self()
}
} // namespace thread
} // FatRat Library

#endif /* FRL_PLATFORM_LINUX */
