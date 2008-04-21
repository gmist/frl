#ifndef FRL_SCOPE_GUARD_H_
#define FRL_SCOPE_GUARD_H_

#include "frl_non_copyable.h"
#include "lock/frl_lock_mutex_i.h"

namespace frl
{
	namespace lock
	{
		// Class for comfortable locked objects in input member-function and
		// automatic unlock in exit.
		class ScopeGuard : private NonCopyable
		{
		private:
			Mutex_I *locked_value;
			ScopeGuard( void ) { }
		public:
			// Constructor
			ScopeGuard( Mutex_I &locked_value_ )
				:	locked_value( &locked_value_ )
			{
				locked_value->lock();
			}

			// Destructor
			~ScopeGuard( void )
			{
				locked_value->unLock();
			}
		}; // class ScopeGuard
	} // namespace lock
} // FatRat Library

#endif /*FRL_SCOPE_GUARD_H_*/
