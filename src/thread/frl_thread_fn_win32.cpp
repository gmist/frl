#include "frl_platform.h"
#if (FRL_PLATFORM ==  FRL_PLATFORM_WIN32 )
#include "thread/frl_thread_fn.h"
#include "frl_exception.h"

namespace frl
{
namespace thread
{

void isValid( const ThreadDescriptor &descriptor )
{
	FRL_EXCEPT_GUARD();
	if( descriptor == InvalidThreadDescriptor )
		FRL_THROW( FRL_STR( "Thread descriptor not valid." ) );
}

void yield( void )
{
	Sleep( 0 );
}

void join( ThreadDescriptor &descriptor )
{
	FRL_EXCEPT_GUARD();
	isValid( descriptor );
	DWORD result = ::WaitForSingleObject( descriptor, INFINITE );
	switch( result )
	{
	case WAIT_OBJECT_0:
		CloseHandle( descriptor );
		descriptor = InvalidThreadDescriptor;
		break;
	case WAIT_TIMEOUT:
		break;
	case WAIT_ABANDONED:
		break;
	default:	
		FRL_THROW( FRL_STR( "Unknown error thread joining.") ); // Analyze GetLastError
	}
}

Bool join( ThreadDescriptor &descriptor , TimeOut msec_ )
{
	FRL_EXCEPT_GUARD();
	isValid( descriptor );
	DWORD result = ::WaitForSingleObject( descriptor, msec_ );
	switch( result )
	{
		case WAIT_OBJECT_0:
			CloseHandle( descriptor );
			descriptor = InvalidThreadDescriptor;
			return True;
		break;
		case WAIT_TIMEOUT:
			return False;
		break;
		case WAIT_ABANDONED:
			return False;
		break;
		default:
			FRL_THROW( FRL_STR( "Unknown error thread joining.") ); // Analyze GetLastError
	}
}

void kill( ThreadDescriptor &descriptor )
{				
	FRL_EXCEPT_GUARD();
	isValid( descriptor );
	if( ::TerminateThread( descriptor, 0) )
	{
		CloseHandle( descriptor );
		descriptor = InvalidThreadDescriptor;
	}
	else
		FRL_THROW( FRL_STR( "Terminating thread error." ) );
}

void create( ThreadDescriptor &descriptor,
				FRL_LPTHREAD_START_ROUTINE startRoutine,
				void* parameter_,
				Bool isDetached_,
				UInt stackSize_ )
{
	FRL_EXCEPT_GUARD();
	DWORD threadID;
	descriptor = ::CreateThread( NULL, stackSize_, startRoutine, parameter_, 0, &threadID);
	if( descriptor == InvalidThreadDescriptor )
		FRL_THROW( FRL_STR( "Thread creating error." ) );
}

Bool isEqual( const ThreadDescriptor &rvl, const ThreadDescriptor &lvl )
{
	FRL_EXCEPT_GUARD();
	isValid( rvl );
	isValid( lvl );
	if ( rvl == lvl )
		return True;
	return False;
}

frl::ULong getThreadId()
{
	return ::GetCurrentThreadId();
}

} // namespace thread
} // FatRat Library

#endif
