#ifndef FRL_THREAD_H_
#define FRL_THREAD_H_

#include "thread/frl_thread_fn.h"
#include "frl_empty_type.h"
#include "frl_non_copyable.h"
#include "frl_lock.h"
#include "event/frl_event.h"
#include "frl_auto_value.h"
#include "frl_exception.h"

namespace frl
{
	namespace thread
	{
		namespace private_
		{
			template < class ResultType, class ParameterType >
			struct FuncSelect
			{
				typedef ResultType ( *FRL_THREAD_FUNC ) ( ParameterType parameter_ );
				template< class ThreadType >
				void Run( ThreadType *thread )
				{
					thread->result.value = thread->function( thread->parameter.value );
				}
			};

			template < class ResultType >
			struct FuncSelect< ResultType, void >
			{
				typedef typename ResultType ( *FRL_THREAD_FUNC ) ( void );
				template< class ThreadType >
				void Run( ThreadType *thread )
				{
					thread->result.value = thread->function();
				}
			};

			template < class ParameterType >
			struct FuncSelect< void, ParameterType >
			{
				typedef void ( *FRL_THREAD_FUNC ) ( ParameterType parameter_ );
				template< class ThreadType >
				void Run( ThreadType *thread )
				{
					thread->function( thread->parameter.value );
				}
			};

			template < >
			struct FuncSelect< void, void >
			{
				typedef void ( *FRL_THREAD_FUNC ) ( void );
				template< class ThreadType >
				void Run( ThreadType *thread )
				{
					thread->function();
				}
			};

			template < class Type >
			struct ParamResultType
			{
				Type value;
			};

			template < >
			struct ParamResultType< void >
			{
				EmptyType value;
			};
		} // namespace private_

		template< class ResultType, class ParameterType >
		class ThreadBase
			:	private NonCopyable,
				protected private_::FuncSelect< ResultType, ParameterType >
		{
			friend private_::FuncSelect< ResultType, ParameterType>;
		protected:
			private_::ParamResultType< ResultType > result;
			private_::ParamResultType< ParameterType > parameter;
			private_::FuncSelect<ResultType, ParameterType>::FRL_THREAD_FUNC function;

			lock::Mutex scopeGuard;	
			AutoValue< volatile Bool > isCreated;
			AutoValue< volatile Bool > isRunning;
			frl::event::Event startWait;
			frl::event::Event joinWait;
			ThreadDescriptor descriptor;

			#if defined WIN32
				static DWORD WINAPI RunThread( void* parameter_ ) throw()
				{
					try
					{
						ThreadBase< ResultType, ParameterType > *thread = reinterpret_cast< ThreadBase< ResultType, ParameterType >* >( parameter_ );				
						thread->startWait.Wait();
						thread->FuncSelect::Run< ThreadBase< ResultType, ParameterType > > ( thread );
						thread->isRunning = False;
						thread->isCreated = False;
					}
					catch ( ... ) {}
					return 0;
				}
			#endif // WIN32
			
		public:

			virtual ~ThreadBase( void )
			{
				lock::Mutex::ScopeGuard guard( scopeGuard );
				FRL_EXCEPT_GUARD();
				this->Kill();				
			}

			void Create( Bool isDetached_ = False , UInt stackSize_ = 0 )
			{				
				lock::Mutex::ScopeGuard guard( scopeGuard );
				FRL_EXCEPT_GUARD();
				if ( isCreated )
					FRL_THROW( FRL_STR( "Thread already created." ) );
				if( function == NULL )
					FRL_THROW( FRL_STR( "Pointer to threading function == NULL." ) );
				thread::Create( descriptor, RunThread, this, isDetached_, stackSize_ );
				isCreated = True;
				if ( isDetached_ )
				{
					// FIXME
				}
			}

			void Create( private_::FuncSelect<ResultType, ParameterType>::FRL_THREAD_FUNC function_, Bool isDetached_  = False , UInt stackSize_  = 0 )
			{
				FRL_EXCEPT_GUARD();
				function = function_;
				Create( isDetached_, stackSize_ );
			}

			Bool IsRunning( void )
			{
				return isRunning;
			}

			void Kill( void )
			{
				FRL_EXCEPT_GUARD();
				if ( isCreated )
				{
					startWait.Signal();
					thread::Kill( descriptor );	
				}
				descriptor = InvalidThreadDescriptor;
				isCreated = isRunning = frl::False;
			}
		
			void Yield_( void )
			{
				thread::Yield_();
			}

			void Join( void )
			{
				lock::Mutex::ScopeGuard guard( scopeGuard );
				FRL_EXCEPT_GUARD();
				if ( isCreated && isRunning )
					thread::Join( descriptor );
			}

			Bool Join( frl::frl_timeout msec_ )
			{
				lock::Mutex::ScopeGuard guard( scopeGuard );
				FRL_EXCEPT_GUARD();
				if ( isCreated && isRunning )
					return thread::Join( descriptor, msec_ );
				return False;
			}

			void Start( void )
			{
				lock::Mutex::ScopeGuard guard( scopeGuard );
				FRL_EXCEPT_GUARD();
				if( ! isCreated )
					FRL_THROW( FRL_STR( "Thread not created. Create thread before use function Thread::Start()" ) );
				if ( isRunning )
					FRL_THROW( FRL_STR( "Thread already created and running." ) );
				isRunning = True;
				startWait.Signal();
			}

			ThreadDescriptor Self( void )
			{
				return descriptor;
			}

			Bool operator == ( const ThreadBase &rvl ) throw()
			{
				FRL_EXCEPT_GUARD();
				if( this == &rvl )
					return True;
				return thread::IsEqual( this->descriptor, rvl.descriptor );
			}

			Bool operator != ( const ThreadBase &rvl ) throw()
			{
				return ! operator == ( rvl );
			}
		}; // class ThreadBase< ResultType, ParameterType >

		template < class ResultType, class ParameterType >
		class Thread
			: public ThreadBase< ResultType, ParameterType >
		{
		public:
			typedef ResultType ( *FRL_THREAD_FUNC ) ( ParameterType parameter_ );

			Thread( void ) 
			{
				function = NULL; 
			}

			Thread( FRL_THREAD_FUNC function_, ParameterType parameter_ )
			{
				function = function_;
				parameter.value = parameter_;
			}

			virtual ~Thread( void ) {}
		
			ResultType GetWorkResult( void ) { return result.value; }

		}; // Thread< ResultType, ParametrType >

		template< class ParameterType >
		class Thread< void, ParameterType >
			:	public ThreadBase< void, ParameterType >
		{
		public:
			typedef void ( *FRL_THREAD_FUNC ) ( ParameterType parameter_ );

			Thread( void ) 
			{
				function = NULL;
			}

			Thread( FRL_THREAD_FUNC function_, ParameterType parameter_ )
			{
				function = function_;
				parameter.value = parameter_;
			}

			virtual ~Thread( void ) {}

			void GetWorkResult( void ) { }

		}; // class Thread< void, ParameterType >

		template < class ResultType >
		class Thread< ResultType, void >
			:	public ThreadBase< ResultType, void >
		{
		public:
			typedef ResultType ( *FRL_THREAD_FUNC ) ( void );

			Thread( void )
			{
				function = NULL; 
			}

			Thread( FRL_THREAD_FUNC function_)
			{
				function = function_;
			}

			virtual ~Thread( void ) {}

			ResultType GetWorkResult( void ) { return result.value; }

		}; // class Thread< ResultType, void >

		template< >
		class Thread< void, void >
			:	public ThreadBase< void, void >
		{
		public:
			typedef void ( *FRL_THREAD_FUNC ) ( void );

			Thread( void ) 
			{
				function = NULL; 
			}

			Thread( FRL_THREAD_FUNC function_ )
			{
				function = function_;
			}

			virtual ~Thread( void ) { }

			void GetWorkResult( void ) {}

		}; // class Thread< void, void >

	} // namespace thread
} // namespace Fat Rat Library

#endif /* FRL_THREAD_H_ */
