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
			Mutex::ScopeGuard scopeGuard( mutex );
			SemaphoreDestroy( semaphore);
		}

		void Semaphore::Init( frl::Long value_ )
		{
			Mutex::ScopeGuard scopeGuard( mutex );		
			SemaphoreInit( semaphore, value_ );
		}
		
		frl::Long Semaphore::GetValue( void )
		{
			return SemaphoreGetValue( semaphore );
		}

		void Semaphore::Post( void )
		{
			Mutex::ScopeGuard scopeGuard( mutex );
			SemaphorePost( semaphore );
		}
		
		void Semaphore::Wait( void )
		{
			Mutex::ScopeGuard scopeGuard( wait_mutex );
			SemaphoreWait( semaphore );
		}
		
		#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
		frl::Bool Semaphore::TimedWait( DWORD time_ )
		#endif
		#if( FRL_PLATFORM == FRL_PLATFORM_LINUX )
		frl::Bool Semaphore::TimedWait( unsigned long time_ )
		#endif
		{
			Mutex::ScopeGuard scopeGuard( wait_mutex );			
			return SemaphoreTimedWait( semaphore, time_ );
		}

		frl::Bool Semaphore::TryWait( void )
		{	
			Mutex::ScopeGuard scopeGuard( wait_mutex );
			return SemaphoreTryWait( semaphore );
		}

		void Semaphore::ReInit( frl::Long value_ )
		{
			Mutex::ScopeGuard scopeGuard( mutex );
			SemaphoreDestroy( semaphore);
			SemaphoreInit( semaphore, value_ );
		}

	} // namespace lock
} // Fat Rat Library
