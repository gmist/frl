#ifndef FRL_MUTEX_FN_H_
#define FRL_MUTEX_FN_H_

#include "frl_types.h"
#include "lock/win32/frl_mutex_fn_win32.h"
#include "lock/linux/frl_mutex_fn_linux.h"

namespace frl
{
namespace lock
{

void mutexInit( MutexDescriptor &mutex );
void mutexDestroy( MutexDescriptor &mutex );
void mutexLock( MutexDescriptor &mutex );
void mutexUnlock( MutexDescriptor &mutex );
Bool mutexTryLock( MutexDescriptor &mutex );

} // namespace lock
} // FatRat Library

#endif /* FRL_MUTEX_FN_H_ */
