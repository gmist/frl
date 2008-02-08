#ifndef FRL_MUTEX_H_
#define FRL_MUTEX_H_

#include "lock/frl_scope_guard.h"
#include "lock/frl_mutex_fn.h"
#include "frl_non_copyable.h"
#include "lock/frl_lock_mutex_i.h"

namespace frl
{
	namespace lock
	{
		class Mutex
			:	private NonCopyable, public Mutex_I
		{
		private:
			MutexDescriptor mutex;
			volatile Bool isInitialize;
		public:
			
			// Constructor
			Mutex( void );

			// Destructor
			virtual ~Mutex( void );

			// lock mutex
			virtual void Lock( void );

			// non block try lock mutex
			virtual Bool TryLock( void );

			// unlock mutex
			virtual void UnLock( void );
		}; // class Mutex
	} // namespace lock
} // namespace FatRat Library

#endif /*FRL_MUTEX_H_*/
