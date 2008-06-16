#ifndef frl_opc_event_h_
#define frl_opc_event_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <libs/thread/src/win32/timeconv.inl>
#include <boost/thread/xtime.hpp>

namespace frl
{
namespace opc
{

class Event
{
private:
	boost::condition cndt;
	boost::mutex mtx;
	volatile bool isEvent;
public:
	
	Event()
	: isEvent( false )
	{
	}

	~Event()
	{

	}

	void signal()
	{
		boost::mutex::scoped_lock lock( mtx );
		isEvent = true;
		cndt.notify_one();
	}

	void wait( void )
	{
		boost::mutex::scoped_lock lock( mtx );
		while( ! isEvent )
			cndt.wait( lock );
		isEvent = false;
	}

	Bool timedWait( DWORD time_ )
	{
		boost::mutex::scoped_lock lock( mtx );
		boost::xtime xdelay;
		to_time( time_, xdelay );
		if( isEvent || ( cndt.timed_wait( lock, xdelay ) && isEvent ) )
		{
			isEvent = false;
			return true;
		}
		return false;
	}
};

}
}

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_event_h_
