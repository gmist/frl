#ifndef FRL_RW_MUTEX_LINUX_H_
#define FRL_RW_MUTEX_LINUX_H_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_LINUX )
#include <pthread.h>

namespace frl
{
namespace lock
{

class RWMutex
{
private:
	pthread_mutex_t mutex;
	pthread_mutexattr_t attribute;
	Bool isInitialize;
	RWMutex();
public:
	RWMutex( Bool many_writers = false );
	virtual ~RWMutex();
	Bool lock( void );
	Bool unLock( void );
}; // class RWMutex

} // namespace lock
} // FatRat Library

#endif /* FRL_PLATFORM_LINUX */

#endif /*FRL_RW_MUTEX_LINUX_H_*/
