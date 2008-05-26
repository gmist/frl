#include "frl_types.h"
#include "lock/frl_mutex.h"
#include "frl_exception.h"

namespace frl
{
namespace lock
{
Mutex::Mutex( void )
{
	mutexInit( mutex );
	isInitialize = True;
}

Mutex::~Mutex( void )
{
	if ( isInitialize )
		mutexDestroy( mutex );
}

void Mutex::lock( void )
{
	if( !isInitialize )
		FRL_THROW( FRL_STR( "Mutex not initialize." ) );
	mutexLock( mutex );
}

void Mutex::unLock( void )
{
	if( !isInitialize )
		FRL_THROW( FRL_STR( "Mutex not initialize." ) );
	mutexUnlock( mutex );
}

Bool Mutex::tryLock( void )
{
	if( !isInitialize )
		FRL_THROW( FRL_STR( "Mutex not initialize." ) );
	return mutexTryLock( mutex );
}
} // namespace lock
} // FatRat Library
