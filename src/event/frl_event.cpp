#include "event/frl_event.h"

namespace frl
{
	namespace event
	{
		Event::Event( void )
		{
			semaphore.Init( 0 );
		}

		Event::~Event( void )
		{

		}

		void Event::Signal( void )
		{
			semaphore.Post();
		}

		void Event::Wait( void )
		{
			semaphore.Wait();
		}

		bool Event::TimedWait( frl::frl_timeout time_ )
		{
			return semaphore.TimedWait( time_ );
		}

	} // namespace event
} // Fat Rat Library
