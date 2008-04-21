#include "lock/frl_semaphore.h"
#include "frl_exception.h"

namespace frl
{
	namespace lock
	{
		Semaphore::Semaphore( void )
		{
		}

		Semaphore::~Semaphore( void )
		{
			ScopeGuard scopeGuard( mutex );
			semaphoreDestroy( semaphore);
		}

		void Semaphore::init( frl::Long value_ )
		{
			ScopeGuard scopeGuard( mutex );		
			semaphoreInit( semaphore, value_ );
		}
		
		frl::Long Semaphore::getValue( void )
		{
			return semaphoreGetValue( semaphore );
		}

		void Semaphore::post( void )
		{
			ScopeGuard scopeGuard( mutex );
			semaphorePost( semaphore );
		}
		
		void Semaphore::wait( void )
		{
			ScopeGuard scopeGuard( wait_mutex );
			semaphoreWait( semaphore );
		}
		
		frl::Bool Semaphore::timedWait( TimeOut time_ )
		{
			ScopeGuard scopeGuard( wait_mutex );			
			return semaphoreTimedWait( semaphore, time_ );
		}

		frl::Bool Semaphore::tryWait( void )
		{	
			ScopeGuard scopeGuard( wait_mutex );
			return semaphoreTryWait( semaphore );
		}

		void Semaphore::reInit( frl::Long value_ )
		{
			ScopeGuard scopeGuard( mutex );
			semaphoreDestroy( semaphore);
			semaphoreInit( semaphore, value_ );
		}

	} // namespace lock
} // FatRat Library
