#ifndef frl_opc_timer_h_
#define frl_opc_timer_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <libs/thread/src/win32/timeconv.inl>
#include <boost/thread/xtime.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

namespace frl
{
namespace opc
{

template< class T >
class Timer : private boost::noncopyable
{
private:
	frl::TimeOut time_ms;
	boost::mutex mtx;
	boost::condition cnd;
	volatile bool stopIt;
	boost::mutex opMtx;

	boost::function< void() > functionBoost;
	typedef void ( T::*FunctionDef )( void );

	boost::thread process;
public:

	Timer()
	:	time_ms( 100 ),
		stopIt( true )		
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
				functionBoost();
			}
		}
		else
		{
			frl::TimeOut time_ms_local = time_ms;
			frl::TimeOut tmp_time = time_ms;
			LONGLONG timeStart = 0;
			LONGLONG timeEnd   = 0;
			LONGLONG frequency  = 0;
			boost::xtime xdelay;
			QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
			QueryPerformanceCounter( (LARGE_INTEGER*)&timeStart );
			for( ; ; )
			{
				if( stopIt )
					return;

				to_time( tmp_time, xdelay );
				boost::mutex::scoped_lock lock( mtx );
				if( cnd.timed_wait(lock, xdelay) && stopIt ) 
					return;

				functionBoost();

				if( stopIt ) // for fast exit if condition entry in function()
					return;

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
		functionBoost = boost::bind( function_, ptr );
	}

	void setTimer( int time_ )
	{
		{
			boost::mutex::scoped_lock lock( mtx );
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
			boost::mutex::scoped_lock lock( mtx );	
			process = boost::thread(  boost::bind( &Timer::func, this ) );
			stopIt = false;
		}
	}

	void stop()
	{
		boost::mutex::scoped_lock lock( opMtx );
		if( ! stopIt )
		{
			stopIt = true;
			cnd.notify_one();
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
			cnd.notify_one();
		}
	}
};
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_timer_h_
