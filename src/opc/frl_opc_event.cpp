#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <boost/thread/xtime.hpp>
#include <libs/thread/src/win32/timeconv.inl>
#include "opc/frl_opc_event.h"

namespace frl
{
namespace opc
{

Event::Event() : isEvent( false )
{
}

Event::~Event()
{
}

void Event::signal()
{
	boost::mutex::scoped_lock lock( mtx );
	isEvent = true;
	cndt.notify_one();
}

void Event::wait( void )
{
	boost::mutex::scoped_lock lock( mtx );
	while( ! isEvent )
		cndt.wait( lock );
	isEvent = false;
}

bool Event::timedWait( unsigned long time_ )
{
	boost::mutex::scoped_lock lock( mtx );
	boost::xtime xdelay;
	to_time( static_cast< int >( time_ ), xdelay );
	if( isEvent || ( cndt.timed_wait( lock, xdelay ) && isEvent ) )
	{
		isEvent = false;
		return true;
	}
	return false;
}

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
