#ifndef FRL_THREAD_FN_H_
#define FRL_THREAD_FN_H_

#include "frl_platform.h"
#if defined ( FRL_PLATFORM_WIN32 )
#include <Windows.h>
#endif

#include "frl_types.h"
#include "frl_lock.h"
#include <list>
#include <algorithm>

namespace frl
{
	namespace thread
	{
		void IsValid( const ThreadDescriptor &descriptor );
		void Yield_( void );
		void Join( ThreadDescriptor &descriptor );
		Bool Join( ThreadDescriptor &descriptor, frl_timeout msec_ );
		void Kill( ThreadDescriptor &descriptor );
		Bool IsEqual( const ThreadDescriptor &rvl, const ThreadDescriptor &lvl );
		void Create( ThreadDescriptor &descriptor, FRL_LPTHREAD_START_ROUTINE startRoutine, void* parameter_ = NULL, Bool isDetached_ = False, UInt stackSize_ = 0 );
	}
}

#endif /* FRL_THREAD_FN_H_ */