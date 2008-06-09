#ifndef frl_opc_timer_h_
#define frl_opc_timer_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <libs/thread/src/win32/timeconv.inl>
#include <boost/thread/xtime.hpp>
#include "thread/frl_thread.h"
#include "frl_function.h"
#include "frl_auto_value.h"

namespace frl
{
namespace opc
{
template< class T >
class TimerProxy
{
protected:
	frl::TimeOut time_ms;
	boost::mutex mtx;
	boost::condition cnd;
	volatile bool stopIt;

	frl::Function0< void, T > function;
	typedef typename Function0< void, T>::FunctionDef FuncDef;
public:

	TimerProxy()
	:	time_ms( 100 ),
		stopIt( false )
	{
	}

	void init( Function0< void, T > function_ )
	{
		function = function_;
	}

	void func( void )
	{
		if( time_ms < 50 )
		{
			while( ! stopIt )
			{
				function();
			}
		}
		else
		{
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

				function();
				
				if( stopIt ) // for fast exit if condition entry in function()
					return;

				QueryPerformanceCounter( (LARGE_INTEGER*)&timeEnd );
				double delay = ( ( ( double )( timeEnd - timeStart ) ) / ( (double) frequency ) ) * 1000;
				unsigned int delta = (unsigned int)delay;
				if( delta > ( time_ms + 100 ) )
					tmp_time = time_ms - (delta - time_ms);
				else
				{
					if( delta > 100 && delta < time_ms )
						tmp_time = time_ms - ( time_ms -delta );
					else
						tmp_time = time_ms;
				}
				QueryPerformanceCounter( (LARGE_INTEGER*)&timeStart );	
			}
		}
	}
};

template< class T >
class Timer : protected TimerProxy< T >, private NonCopyable
{
private:
	thread::Thread< void, void, TimerProxy<T> > process;
public:

	Timer()
	:	TimerProxy< T >()
	{
	}

	~Timer()
	{
		stop();
	}

	void init( T *ptr, typename TimerProxy< T >::FuncDef function_ )
	{
		TimerProxy< T >::init( Function0< void, T > ( *ptr, function_ ) );
	}

	void setTimer( int time_ )
	{
		TimerProxy< T >::time_ms = time_;
		if( process.isRunning() )
		{
			stop();
			start();
		}
	}

	void start()
	{
		if( ! process.isRunning() )
		{
			process.create( &TimerProxy<T>::func, *this );
			TimerProxy< T >::stopIt = false;
			process.start();
		}
	}

	void stop()
	{
		if( process.isRunning() )
		{
			TimerProxy< T >::stopIt = true;
			TimerProxy< T >::cnd.notify_one();
			process.join();
		}
	}

	Bool isStop()
	{
		return TimerProxy< T >::stopIt;
	}

	void tryStop()
	{
		if( process.isRunning() )
		{
			TimerProxy< T >::stopIt = True;
			TimerProxy< T >::cnd.notify_one();
		}
	}
};
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_timer_h_
