#ifndef frl_opc_timer_h_
#define frl_opc_timer_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <boost/thread/mutex.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include "opc/frl_opc_event.h"

namespace frl
{
namespace opc
{

template< class T >
class Timer : private boost::noncopyable
{
private:
	unsigned long time_ms;
	volatile bool stopIt;
	volatile bool isTryStop;
	boost::mutex opMtx;
	boost::mutex startThreadGuard;
	opc::Event stopEvent;

	boost::function< void() > executionFunction;
	typedef void ( T::*FunctionDef )( void );

	boost::thread process;
public:

	Timer()
	:	time_ms( 100 ),
		stopIt( true ),
		isTryStop( false )
	{
	}

	~Timer()
	{
		stop();
	}

	void func( void )
	{
		if( time_ms < 50 )
		{
			while( ! stopIt )
			{
				executionFunction();
			}
		}
		else
		{
			unsigned long time_ms_local = time_ms;
			unsigned long tmp_time = time_ms;
			LONGLONG timeStart = 0;
			LONGLONG timeEnd   = 0;
			LONGLONG frequency  = 0;

			QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
			QueryPerformanceCounter( (LARGE_INTEGER*)&timeStart );
			boost::mutex::scoped_lock lock( startThreadGuard );
			while( ! stopEvent.timedWait( tmp_time ) )
			{
				executionFunction();
				QueryPerformanceCounter( (LARGE_INTEGER*)&timeEnd );
				double delay = ( ( ( double )( timeEnd - timeStart ) ) / ( (double) frequency ) ) * 1000;
				unsigned int delta = (unsigned int)delay;
				if( delta > ( time_ms_local + 100 ) )
					tmp_time = time_ms_local - (delta - time_ms_local);
				else
				{
					if( delta > 100 && delta < time_ms_local )
						tmp_time = time_ms_local - ( time_ms_local -delta );
					else
						tmp_time = time_ms_local;
				}
				QueryPerformanceCounter( (LARGE_INTEGER*)&timeStart );
			}
		}
	}

	void init( T *ptr, typename Timer::FunctionDef function_ )
	{		
		boost::mutex::scoped_lock lock( opMtx );
		executionFunction = boost::bind( function_, ptr );
	}

	void setTimer( int time_ )
	{
		{
			boost::mutex::scoped_lock lock( opMtx );
			if( stopIt )
			{
				time_ms = time_;
				return;
			}
		}

		stop();
		time_ms = time_;
		start();
	}

	void start()
	{
		boost::mutex::scoped_lock lock( opMtx );
		if( stopIt )
		{
			boost::mutex::scoped_lock lock( startThreadGuard );
			process = boost::thread(  boost::bind( &Timer::func, this ) );
			stopIt = false;
		}
	}

	void stop()
	{
		boost::mutex::scoped_lock lock( opMtx );
		if( isTryStop )
		{
			stopEvent.signal();
			process.join();
			isTryStop = false;
			return;
		}
		if( ! stopIt )
		{
			stopEvent.signal();
			stopIt = true;
			process.join();
		}
	}

	Bool isStop()
	{
		return stopIt;
	}

	void tryStop()
	{
		boost::mutex::scoped_lock lock( opMtx );
		if( ! stopIt )
		{
			stopIt = true;
			isTryStop = true;
		}
	}
};
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_timer_h_
