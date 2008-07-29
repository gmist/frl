#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_event.h"

namespace frl{ namespace opc{

Event::Event() : handle( CreateEvent(NULL, FALSE, FALSE, NULL) )
{
	FRL_EXCEPT_GUARD();
	if( handle == NULL )
		FRL_THROW_S_CLASS( Event::InitializeError );
}

Event::~Event()
{
	CloseHandle( handle );	
}

void Event::signal()
{
	SetEvent( handle );
}

void Event::wait( void )
{
	WaitForSingleObject( handle, INFINITE );
}

bool Event::timedWait( unsigned long time_ )
{
	DWORD result = WaitForSingleObject( handle, time_ );
	if( result == WAIT_TIMEOUT )
		return false;
	return true; // WAIT_OBJECT_0
}

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
