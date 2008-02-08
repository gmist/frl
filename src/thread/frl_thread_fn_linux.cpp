#include "frl_platform.h"
#if ( FRL_PLATFORM == FRL_PLATFORM_LINUX )
#include "thread/frl_thread_fn.h"
#include "frl_exception.h"

namespace frl
{
	namespace thread
	{
		void IsValid( const ThreadDescriptor &descriptor )
		{
			FRL_EXCEPT_GUARD();
			if( descriptor == 0 )
				FRL_THROW("Thread descriptor not valid.");
		}

		void Yield_( void )
		{
			::pthread_yield();
		}

		void Join( ThreadDescriptor &descriptor )
		{
			FRL_EXCEPT_GUARD();
			IsValid( descriptor );
			pthread_join( descriptor, NULL ); // FIXME
		}

		Bool Join( ThreadDescriptor &descriptor, TimeOut msec_ )
		{
			FRL_EXCEPT_GUARD();
			IsValid( descriptor );
			pthread_join( descriptor, msec_ ); // FIXME
		}

		void Kill( ThreadDescriptor &descriptor )
		{
			FRL_EXCEPT_GUARD();
			IsValid( descriptor );
			pthread_cancel( id ); // FIXME
			descriptor = 0;
		}

		Bool IsEqual( const ThreadDescriptor &rvl, const ThreadDescriptor &lvl )
		{
			FRL_EXCEPT_GUARD();
			IsValid( rvl );
			IsValid( lvl );
			return ::pthread_equal( id, r_val.id ) !=0;
		}

		// TODO: Заменить void* parameter_ на template class< T >
		ThreadDescriptor Create( FRL_LPTHREAD_START_ROUTINE startRoutine, void* parameter_ = NULL, Bool isDetached_ = False, UInt stackSize_ = 0 )
		{
			FRL_EXCEPT_GUARD();
			pthread_attr_t attribute;
			pthread_attr_init( &attribute );

			if( stackSize_ )
				if ( pthread_attr_setstacksize( &attribute, stackSize_ ) !=0 )
				{
					pthread_attr_destroy( &attribute );
					FRL_THROW( "Create thread error. Do not set stack size." );
				}
				
				if( isDetached_ )
				{
					if( pthread_attr_setdetachstate( &attribute, PTHREAD_CREATE_DETACHED ) !=0 )
					{
						pthread_attr_destroy( &attribute );
						FRL_THROW( "Create thread error. Do not set detached attribute." );
					}
				}

				pthread_testcancel();
				
				ThreadDescriptor id;
				if( pthread_create( &id, &attribute, startRoutine, parameter_ ) !=0 )
				{
					pthread_attr_destroy( &attribute );
					FRL_THROW( "Create thread error." );
				}
				pthread_attr_destroy( &attribute );
				return id;
		}
	} // namespace thread
} // Fat Rat Library

#endif /* FRL_PLATFORM_LINUX */
