#ifndef FRL_MUTEX_FN_H_
#define FRL_MUTEX_FN_H_

#include "frl_types.h"
#include "lock/win32/frl_mutex_fn_win32.h"
#include "lock/linux/frl_mutex_fn_linux.h"

namespace frl
{
	namespace lock
	{
		void MutexInit( MutexDescriptor &mutex );
		void MutexDestroy( MutexDescriptor &mutex );
		void MutexLock( MutexDescriptor &mutex );
		void MutexUnlock( MutexDescriptor &mutex );
		Bool MutexTryLock( MutexDescriptor &mutex );
	} // namespace lock
} // namespace FatRat Library

#endif /* FRL_MUTEX_FN_H_ */
