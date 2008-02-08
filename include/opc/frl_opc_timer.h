#ifndef frl_opc_timer_h_
#define frl_opc_timer_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "thread/frl_thread.h"
#include "lock/frl_event.h"


namespace frl
{
	namespace opc
	{
		template< class T >
		class TimerProxy
		{
		protected:
			frl::TimeOut time_ms;
			lock::Event stopEvent;
			typedef void ( T::*THREAD_FUNC ) ( void );
			T *ptr;
			THREAD_FUNC function;
			volatile Bool stopIt;

		public:
			void func( void )
			{
				if( time_ms < 50 )
				{
					while( ! stopIt )
					{
						(*ptr.*function)();
					}
				}
				else
				{
					frl::TimeOut tmp_time = time_ms;
					LONGLONG timeStart = 0;
					LONGLONG timeEnd   = 0;
					LONGLONG frequency  = 0;
					QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
					QueryPerformanceCounter( (LARGE_INTEGER*)&timeStart );
					while( ! stopEvent.TimedWait( tmp_time ) )
					{
						if( stopIt )
							return;

						(*ptr.*function)();

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

			Timer( T *ptr_, THREAD_FUNC function_ )
			{
				ptr = ptr_;
				function = function_;
				time_ms = 100;
				stopIt = True;
			}

			~Timer()
			{
				stop();
			}

			void setTimer( int time_ )
			{
				time_ms = time_;
				if( process.IsRunning() )
				{
					stop();
					start();
				}
			}

			void start()
			{
				if( ! process.IsRunning() )
				{
					process.Create( &TimerProxy<T>::func, *this );
					stopIt = False;
					process.Start();
				}
				else
				{
					stop();
					process.Create( &TimerProxy<T>::func, *this );
					stopIt = False;
					process.Start();
				}
			}

			void stop()
			{
				if( process.IsRunning() )
				{
					stopIt = True;
					stopEvent.Signal();
					process.Join();
				}
			}

			Bool isStop()
			{
				return stopIt;
			}

			void tryStop()
			{
				if( process.IsRunning() )
				{
					stopIt = True;
					stopEvent.Signal();
				}
			}
		};
	} // namespace opc
} // namespace FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_timer_h_