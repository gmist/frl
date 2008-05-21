#include "lock/frl_event.h"

#if( FRL_PLATFORM == FRL_PLATFORM_LINUX )
#include "lock/frl_semaphore.h"
#endif

namespace frl
{
	namespace lock
	{
		Event::Event( void )
		{
			#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
				handle = CreateEvent(NULL, FALSE, FALSE, NULL);
				if( handle == NULL )
					FRL_THROW_S_CLASS( Event::InitializeError );
			#endif
			#if( FRL_PLATFORM == FRL_PLATFORM_LINUX )
				semaphore.init( 0 );
			#endif
		}

		Event::~Event( void )
		{
			#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
				CloseHandle( handle );
			#endif
		}

		void Event::signal( void )
		{
			#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
				SetEvent( handle );
			#endif
			#if( FRL_PLATFORM == FRL_PLATFORM_LINUX )
				semaphore.post();
			#endif
		}

		void Event::wait( void )
		{
			#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
				WaitForSingleObject( handle, INFINITE );
			#endif
			#if( FRL_PLATFORM == FRL_PLATFORM_LINUX )
				semaphore.wait();
			#endif
		}

		Bool Event::timedWait( frl::TimeOut time_ )
		{
			#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
				DWORD result = WaitForSingleObject( handle, time_ );
				if( result == WAIT_TIMEOUT )
					return False;
				return True; // WAIT_OBJECT_0
			#endif
			#if( FRL_PLATFORM == FRL_PLATFORM_LINUX )
				return semaphore.timedWait( time_ );
			#endif
		}

	} // namespace lock
} // FatRat Library
