#ifndef FRL_SCOPE_GUARD_H_
#define FRL_SCOPE_GUARD_H_

namespace frl
{
	namespace lock
	{
		// Class for comfortable locked objects in input member-function and
		// automatic unlock in exit.
		template< class T >
		class ScopeGuard
		{
		private:
			T *locked_value;
			ScopeGuard( void ) { }
		public:
			// Constructor
			ScopeGuard( T &locked_value_ )
				:	locked_value( &locked_value_ )
			{
				locked_value->Lock();
			}

			// Destructor
			~ScopeGuard( void )
			{
				locked_value->UnLock();
			}
		}; // class ScopeGuard

	} // namespace lock
} // Fat Rat Library

#endif /*FRL_SCOPE_GUARD_H_*/
