#ifndef FRL_NULL_MUTEX_H_
#define FRL_NULL_MUTEX_H_

#include "lock/frl_scope_guard.h"
#include "frl_types.h"

namespace frl
{
	namespace lock
	{
		// class NullMutex
		class NullMutex
		{
		public:
			// Redefinition ScopeGuard
			typedef frl::lock::ScopeGuard< NullMutex > ScopeGuard;

			// Default destructor
			NullMutex( void )
			{
			}

			// Destructor
			virtual ~NullMutex( void )
			{
			}

			// Lock the mutex
			void Lock( void ) { }

			// Not blocked lock the mutex
			void TryLock() { }

			// Unlock the mutex
			void Unlock( void ) { }

		}; // class NullMutex
	}	// namespace lock
}	// namespace FatRat Library

#endif // FRL_NULL_MUTEX_H_
