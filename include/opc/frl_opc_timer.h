#ifndef frl_opc_timer_h_
#define frl_opc_timer_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "thread/frl_thread.h"
#include "frl_lock.h"
#include <iostream>
#include <math.h>

namespace frl
{
	namespace opc
	{
		template< class T >
		class TimerProxy
		{
		protected:
			frl::frl_timeout time_ms;
			lock::Semaphore *sem;
			typedef void ( T::*THREAD_FUNC ) ( void );
			T *ptr;
			THREAD_FUNC function;

		public:
			void func( void )
			{
				if( time_ms < 100 )
				{
					while( ! sem->TimedWait( time_ms ) )
					{
						(*ptr.*function)();
					}
				}
				else
				{
					frl::frl_timeout tmp_time = time_ms;
					LONGLONG llEnd   = 0;
					LONGLONG llStart = 0;
					LONGLONG llFreq  = 0;
					QueryPerformanceFrequency((LARGE_INTEGER*)&llFreq);
					while( ! sem->TimedWait( tmp_time ) )
					{
						(*ptr.*function)();
						QueryPerformanceCounter( (LARGE_INTEGER*)&llEnd );

						if (llStart != 0)
						{
							double delay = ( ( ( double )( llEnd - llStart ) ) / ( (double) llFreq ) ) * 1000;
							DWORD delta = ( DWORD )floor( delay );
							if( delta < time_ms )
								tmp_time = time_ms + ( time_ms - delta );
							else
								tmp_time = time_ms - ( delta - time_ms );
						}
						QueryPerformanceCounter( (LARGE_INTEGER*)&llStart );
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
				sem = new lock::Semaphore();
				sem->Init( 0 );
			}

			~Timer()
			{
				stop();
				delete sem;
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
					sem->ReInit( 0 );
					process.Create( &TimerProxy<T>::func, *this );
					process.Start();
				}
				else
				{
					stop();
					sem->ReInit( 0 );
					process.Create( &TimerProxy<T>::func, *this );
					process.Start();
				}
			}

			void stop()
			{
				if( process.IsRunning() )
				{
					sem->Post();
					process.Join();
				}
			}

		};
	} // namespace opc
} // namespace FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_timer_h_