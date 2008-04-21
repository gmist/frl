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
	void init( frl::Long value_ );

	// Get current value the semaphore
	frl::Long getValue( void );

	// Increment value the semaphore
	void post( void );
	
	// Not blocked wait not zero value the semaphore
	frl::Bool tryWait( void );

	// Blocked wait not zero value the semaphore
	void wait( void );
	
	// Blocked wait not zero value the semaphore
	// in the course of a set time out
	frl::Bool timedWait( TimeOut time_ );

	// Reinitialize the semaphore
	void reInit( frl::Long value_ );

};	// class Semaphore

class SemaphoreMutex : public Mutex_I
{
	Semaphore sem;
public:			

	// Конструктор
	SemaphoreMutex( void )
	{
		sem.init( 1 );
	}

	// Деструктор
	virtual ~SemaphoreMutex( void )
	{
	}

	// Захват мьютекса
	virtual void lock( void )
	{
		sem.wait();
	}

	// Попытка захвата мьютекса
	virtual Bool tryLock( void )
	{
		return sem.tryWait();
	}

	// Освобождение мьютекса
	virtual void unLock( void )
	{
		sem.post();
	}
};
} // namespace lock
} // FatRat Library

#endif // FRL_SEMAPHORE_H_
