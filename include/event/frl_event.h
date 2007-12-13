#ifndef FRL_EVENT_H_LINUX_
#define FRL_EVENT_H_LINUX_

#include "frl_types.h"
#include "frl_lock.h"

namespace frl
{
	namespace event
	{
		class Event
		{
		private:
			frl::lock::Semaphore semaphore;
		public:
			Event( void );
			~Event( void );
			void Signal( void );
			void Wait( void );
			bool TimedWait( frl_timeout time_ );
		};
	}// namespace event
} // Fat Rat library

#endif /*FRL_EVENT_H_LINUX_*/
