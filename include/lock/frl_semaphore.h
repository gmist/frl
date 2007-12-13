#ifndef FRL_SEMAPHORE_H_
#define FRL_SEMAPHORE_H_

#include "frl_types.h"
#include "lock/frl_mutex.h"
#include "lock/frl_semaphore_fn.h"
#include "frl_non_copyable.h"

namespace frl
{
	namespace lock
	{
		class Semaphore
			:	private NonCopyable
		{
		private:
			SemaphoreDescriptor semaphore; // semaphore descriptor
			Mutex mutex, wait_mutex;
		public:
			// Default constructor
			Semaphore( void );

			// Destructor
			~Semaphore( void );

			// Initialize the semaphore
			void Init( frl::Long value_ );

			// Get current value the semaphore
			frl::Long GetValue( void );

			// Increment value the semaphore
			void Post( void );
			
			// Not blocked wait not zero value the semaphore
			frl::Bool TryWait( void );

			// Blocked wait not zero value the semaphore
			void Wait( void );
			
			// Blocked wait not zero value the semaphore
			// in the course of a set time out
			#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
				frl::Bool TimedWait( DWORD time_ );
			#endif
			#if( FRL_PLATFORM == FRL_PLATFORM_LINUX )
				frl::Bool TimedWait( unsigned long time_ );
			#endif

			// Reinitialize the semaphore
			void ReInit( frl::Long value_ );

		};	// class Semaphore
	} // namespace lock
} // namespace FatRat Library

#endif /*FRL_SEMAPHORE_H_*/
