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
			frl::Bool TimedWait( TimeOut time_ );

			// Reinitialize the semaphore
			void ReInit( frl::Long value_ );

		};	// class Semaphore

		class SemaphoreMutex : public Mutex_I
		{
			Semaphore sem;
		public:			

			// Конструктор
			SemaphoreMutex( void )
			{
				sem.Init( 1 );
			}

			// Деструктор
			~SemaphoreMutex( void )
			{
			}

			// Захват мьютекса
			virtual void Lock( void )
			{
				sem.Wait();
				sem.ReInit( 1 );
			}

			// Попытка захвата мьютекса
			virtual Bool TryLock( void )
			{
				return sem.TryWait();
			}

			// Освобождение мьютекса
			virtual void UnLock( void )
			{
				sem.Post();
			}
		};
	} // namespace lock
} // namespace FatRat Library

#endif // FRL_SEMAPHORE_H_
