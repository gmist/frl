#ifndef frl_opc_event_h_
#define frl_opc_event_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include <boost/noncopyable.hpp>
#include "frl_exception.h"

namespace frl{ namespace opc{

class Event : private boost::noncopyable
{
private:
	HANDLE handle;
public:
	FRL_EXCEPTION_CLASS( InitializeError ); 
	Event();
	~Event();
	void signal();
	void wait( void );
	bool timedWait( unsigned long time_ );
};

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_event_h_
