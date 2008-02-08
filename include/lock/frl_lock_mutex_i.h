#ifndef frl_lock_mutex_i_h_
#define frl_lock_mutex_i_h_

namespace frl
{
	namespace lock
	{
		class Mutex_I
		{
		public:

			// lock mutex
			virtual void Lock( void ) = 0;

			// non block try lock mutex
			virtual Bool TryLock( void ) = 0;

			// unlock mutex
			virtual void UnLock( void ) = 0;
		};
	}
} // namespace FatRat Library

#endif // frl_lock_mutex_i_h_