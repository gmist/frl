#ifndef FRL_NULL_MUTEX_H_
#define FRL_NULL_MUTEX_H_

#include "frl_types.h"
#include "lock/frl_lock_mutex_i.h"

namespace frl
{
	namespace lock
	{
		// class NullMutex
		class NullMutex : public Mutex_I
		{
		public:
			// Redefinition ScopeGuard
			//typedef frl::lock::ScopeGuard< NullMutex > ScopeGuard;

			// Default destructor
			NullMutex( void )
			{
			}

			// Destructor
			virtual ~NullMutex( void )
			{
			}

			// Lock the mutex
			virtual void lock( void ) { }

			// Not blocked lock the mutex
			virtual Bool tryLock() { return True; }

			// Unlock the mutex
			virtual void unlock( void ) { }

		}; // class NullMutex
	}	// namespace lock
}	// namespace FatRat Library

#endif // FRL_NULL_MUTEX_H_
