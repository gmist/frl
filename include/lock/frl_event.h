#ifndef frl_lock_event_h_
#define frl_lock_event_h_

#include "frl_types.h"
#include "frl_exception.h"

namespace frl
{
	namespace lock
	{
		#if( FRL_PLATFORM == FRL_PLATFORM_LINUX )
		class Semaphore;
		#endif

		class Event
		{
		private:
			#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
				HANDLE handle;
			#endif
			#if( FRL_PLATFORM == FRL_PLATFORM_LINUX )
				frl::lock::Semaphore semaphore;
			#endif
		public:
			FRL_EXCEPTION_CLASS( InitializeError );
			Event( void );
			~Event( void );
			void signal( void );
			void wait( void );
			Bool timedWait( TimeOut time_ );
		};
	}// namespace lock
} // Fat Rat library

#endif // frl_lock_event_h_
