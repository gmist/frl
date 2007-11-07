#ifndef FRL_MUTEX_H_
#define FRL_MUTEX_H_

#include "lock/frl_scope_guard.h"
#include "lock/frl_mutex_fn.h"
#include "frl_non_copyable.h"

namespace frl
{
	namespace lock
	{		
		class Mutex
			:	private NonCopyable
		{
		private:
			MutexDescriptor mutex;
			volatile Bool isInitialize;
		public:

			typedef frl::lock::ScopeGuard< Mutex > ScopeGuard;
			
			// Конструктор
			Mutex( void );

			// Деструктор
			~Mutex( void );

			// Захват мьютекса
			void Lock( void );

			// Попытка захвата мьютекса
			Bool TryLock( void );

			// Освобождение мьютекса
			void UnLock( void );
		}; // class Mutex
	} // namespace lock
} // FatRat Library

#endif /*FRL_MUTEX_H_*/
