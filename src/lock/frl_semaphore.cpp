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
			SemaphoreDestroy( semaphore);
		}

		void Semaphore::Init( frl::Long value_ )
		{
			ScopeGuard scopeGuard( mutex );		
			SemaphoreInit( semaphore, value_ );
		}
		
		frl::Long Semaphore::GetValue( void )
		{
			return SemaphoreGetValue( semaphore );
		}

		void Semaphore::Post( void )
		{
			ScopeGuard scopeGuard( mutex );
			SemaphorePost( semaphore );
		}
		
		void Semaphore::Wait( void )
		{
			ScopeGuard scopeGuard( wait_mutex );
			SemaphoreWait( semaphore );
		}
		
		frl::Bool Semaphore::TimedWait( TimeOut time_ )
		{
			ScopeGuard scopeGuard( wait_mutex );			
			return SemaphoreTimedWait( semaphore, time_ );
		}

		frl::Bool Semaphore::TryWait( void )
		{	
			ScopeGuard scopeGuard( wait_mutex );
			return SemaphoreTryWait( semaphore );
		}

		void Semaphore::ReInit( frl::Long value_ )
		{
			ScopeGuard scopeGuard( mutex );
			SemaphoreDestroy( semaphore);
			SemaphoreInit( semaphore, value_ );
		}

	} // namespace lock
} // Fat Rat Library
