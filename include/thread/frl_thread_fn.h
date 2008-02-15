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
		void isValid( const ThreadDescriptor &descriptor );
		void yield( void );
		void join( ThreadDescriptor &descriptor );
		Bool join( ThreadDescriptor &descriptor, TimeOut msec_ );
		void kill( ThreadDescriptor &descriptor );
		Bool isEqual( const ThreadDescriptor &rvl, const ThreadDescriptor &lvl );
		void create( ThreadDescriptor &descriptor, FRL_LPTHREAD_START_ROUTINE startRoutine, void* parameter_ = NULL, Bool isDetached_ = False, UInt stackSize_ = 0 );
	}
}

#endif /* FRL_THREAD_FN_H_ */