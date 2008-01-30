#ifndef FRL_THREAD_H_
#define FRL_THREAD_H_

#include "thread/frl_thread_fn.h"
#include "frl_empty_type.h"
#include "frl_non_copyable.h"
#include "frl_lock.h"
#include "event/frl_event.h"
#include "frl_auto_value.h"
#include "frl_exception.h"
#include "frl_empty_type.h"

namespace frl
{
	namespace thread
	{
		namespace private_
		{
			template < class ResultType, class ParameterType, class Type = EmptyType >
			struct FuncSelect
			{
				typedef ResultType ( Type::*FRL_THREAD_FUNC ) ( ParameterType parameter_ );
			};

			template < class Type >
			struct FuncSelect< void, void, Type >
			{
				typedef void ( Type::*FRL_THREAD_FUNC ) ( void );
				template< class ThreadType >
				void Run( ThreadType *thread )
				{
					( (Type*)( thread->type )->*thread->function )();
				}
			};

			template< class ResultType, class Type >
			struct FuncSelect< ResultType, void, Type >
			{
				typedef ResultType ( Type::*FRL_THREAD_FUNC ) ( void );
				template< class ThreadType >
				void Run( ThreadType *thread )
				{
					thread->result.value = ( ( Type* )( thread->type )->*thread->function )();
				}
			};

			template< class ParameterType, class Type >
			struct FuncSelect< void, ParameterType, Type >
			{
				typedef void ( Type::*FRL_THREAD_FUNC ) ( ParameterType );
				template< class ThreadType >
				void Run( ThreadType *thread )
				{
					( ( Type* )( thread->type )->*thread->function )( thread->parameter.value );
				}
			};

			template < class ResultType, class ParameterType >
			struct FuncSelect< ResultType, ParameterType, EmptyType >
			{
				typedef ResultType ( *FRL_THREAD_FUNC ) ( ParameterType parameter_ );
				template< class ThreadType >
				void Run( ThreadType *thread )
				{
					thread->result.value = thread->function( thread->parameter.value );
				}
			};

			template < class ResultType >
			struct FuncSelect< ResultType, void, EmptyType >
			{
				typedef typename ResultType ( *FRL_THREAD_FUNC ) ( void );
				template< class ThreadType >
				void Run( ThreadType *thread )
				{
					thread->result.value = thread->function();
				}
			};

			template < class ParameterType >
			struct FuncSelect< void, ParameterType, EmptyType >
			{
				typedef void ( *FRL_THREAD_FUNC ) ( ParameterType parameter_ );
				template< class ThreadType >
				void Run( ThreadType *thread )
				{
					thread->function( thread->parameter.value );
				}
			};

			template < >
			struct FuncSelect< void, void, EmptyType >
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
		}

		template< class T >
		class MasterThreadBase
		{
		protected:
			lock::Mutex scopeGuard;	
			AutoValue< volatile Bool > isCreated;
			AutoValue< volatile Bool > isRunning;
			frl::event::Event startWait;
			frl::event::Event joinWait;
			ThreadDescriptor descriptor;
		public:

			virtual ~MasterThreadBase( void )
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
				T* pT = static_cast<T*> (this);
				if( pT->function == NULL )
					FRL_THROW( FRL_STR( "Pointer to threading function == NULL." ) );
				thread::Create( descriptor, pT->RunThread, pT, isDetached_, stackSize_ );
				isCreated = True;
				if ( isDetached_ )
				{
					// FIXME
				}
			}

			Bool IsRunning( void )
			{
				return isRunning;
			}

			void Kill( void )
			{
				FRL_EXCEPT_GUARD();
				if ( isCreated || isRunning )
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

			ThreadDescriptor Self( void )
			{
				return descriptor;
			}

			Bool operator == ( const MasterThreadBase &rvl ) throw()
			{
				FRL_EXCEPT_GUARD();
				if( this == &rvl )
					return True;
				return thread::IsEqual( this->descriptor, rvl.descriptor );
			}

			Bool operator != ( const MasterThreadBase &rvl ) throw()
			{
				return ! operator == ( rvl );
			}

		};

		template< class ResultType, class ParameterType, class Type = EmptyType >
		class ThreadBase
			:	private NonCopyable,
				public MasterThreadBase< ThreadBase< ResultType, ParameterType, Type > >,
				public private_::FuncSelect< ResultType, ParameterType, Type >
		{
			friend private_::FuncSelect< ResultType, ParameterType, Type >;
		protected:
			private_::ParamResultType< ResultType > result;
			private_::ParamResultType< ParameterType > parameter;
			const Type *type;
		public:
			private_::FuncSelect< ResultType, ParameterType, Type >::FRL_THREAD_FUNC function;

			#if defined WIN32
				static DWORD WINAPI RunThread( void* parameter_ ) throw()
				{
					try
					{
						Thread< ResultType, ParameterType, Type > *thread = reinterpret_cast< Thread< ResultType, ParameterType, Type >* >( parameter_ );				
						thread->startWait.Wait();
						thread->FuncSelect::Run< Thread< ResultType, ParameterType, Type > > ( thread );
						thread->isRunning = False;
						thread->isCreated = False;
					}
					catch ( ... ) {}
					return 0;
				}
			#endif // WIN32

			void Create( private_::FuncSelect<ResultType, ParameterType, Type >::FRL_THREAD_FUNC function_, const Type &type_in , Bool isDetached_  = False , UInt stackSize_  = 0 )
			{
				lock::Mutex::ScopeGuard guard( scopeGuard );
				FRL_EXCEPT_GUARD();
				type = &type_in;
				function = function_;
				MasterThreadBase::Create( isDetached_, stackSize_ );
			}
		};

		template< class ResultType, class ParameterType >
		class ThreadBase< ResultType, ParameterType, EmptyType >
			:	private NonCopyable,
				public MasterThreadBase< ThreadBase< ResultType, ParameterType, EmptyType > >,
				protected private_::FuncSelect< ResultType, ParameterType >
		{
			friend private_::FuncSelect< ResultType, ParameterType >;
		protected:
			private_::ParamResultType< ResultType > result;
			private_::ParamResultType< ParameterType > parameter;
		public:
			private_::FuncSelect< ResultType, ParameterType >::FRL_THREAD_FUNC function;

			#if defined WIN32
				static DWORD WINAPI RunThread( void* parameter_ ) throw()
				{
					try
					{
						Thread< ResultType, ParameterType > *thread = reinterpret_cast< Thread< ResultType, ParameterType >* >( parameter_ );				
						thread->startWait.Wait();
						thread->FuncSelect::Run< Thread< ResultType, ParameterType > > ( thread );
						thread->isRunning = False;
						thread->isCreated = False;
					}
					catch ( ... ) {}
					return 0;
				}
			#endif // WIN32

			void Create( private_::FuncSelect<ResultType, ParameterType >::FRL_THREAD_FUNC function_, Bool isDetached_  = False , UInt stackSize_  = 0 )
			{
				FRL_EXCEPT_GUARD();
				function = function_;
				MasterThreadBase::Create( isDetached_, stackSize_ );
			}
		};

		template< class ResultType, class ParameterType, class Type = EmptyType >
		class Thread : public ThreadBase< ResultType, ParameterType, Type >
		{
		public:
			typedef ResultType ( *FRL_THREAD_FUNC ) ( ParameterType parameter_ );

			Thread( void ) 
			{
				function = NULL; 
			}

			Thread( FRL_THREAD_FUNC function_, ParameterType parameter_, const Type &type_ )
			{
				function = function_;
				parameter.value = parameter_;
				type = type_;
			}

			void Start( ParameterType parameter_ )
			{
				lock::Mutex::ScopeGuard guard( scopeGuard );
				FRL_EXCEPT_GUARD();
				if( ! isCreated )
					FRL_THROW( FRL_STR( "Thread not created. Create thread before use function Thread::Start()" ) );
				if ( isRunning )
					FRL_THROW( FRL_STR( "Thread already created and running." ) );
				parameter.value = parameter_;
				isRunning = True;
				startWait.Signal();
			}

			virtual ~Thread( void ) {}

			ResultType GetWorkResult( void ) { return result.value; }
		};

		template< class Type >
		class Thread< void, void, Type > : public ThreadBase< void, void, Type >
		{
		public:
			typedef void ( *FRL_THREAD_FUNC ) ( void );

			Thread( void ) 
			{
				function = NULL; 
			}

			Thread( FRL_THREAD_FUNC function_, const Type &type_ )
			{
				function = function_;
				type = type_;
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

			virtual ~Thread( void ) {}
		};

		template< class ResultType, class Type >
		class Thread< ResultType, void, Type> : public ThreadBase< ResultType, void, Type >
		{
		public:
			typedef ResultType ( *FRL_THREAD_FUNC ) ( void );

			Thread( void ) 
			{
				function = NULL; 
			}

			Thread( FRL_THREAD_FUNC function_,  const Type &type_ )
			{
				function = function_;
				type = type_;
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

			virtual ~Thread( void ) {}

			ResultType GetWorkResult( void )
			{
				return result.value;
			}
		};

		template< class ParameterType, class Type >
		class Thread< void, ParameterType, Type> : public ThreadBase< void, ParameterType, Type >
		{
		public:
			typedef void ( *FRL_THREAD_FUNC ) ( ParameterType parameter_ );

			Thread( void ) 
			{
				function = NULL; 
			}

			Thread( FRL_THREAD_FUNC function_, ParameterType parameter_,  const Type &type_ )
			{
				function = function_;
				parameter.value = parameter_;
				type = type_;
			}

			void Start( ParameterType parameter_ )
			{
				lock::Mutex::ScopeGuard guard( scopeGuard );
				FRL_EXCEPT_GUARD();
				if( ! isCreated )
					FRL_THROW( FRL_STR( "Thread not created. Create thread before use function Thread::Start()" ) );
				if ( isRunning )
					FRL_THROW( FRL_STR( "Thread already created and running." ) );
				parameter.value = parameter_;
				isRunning = True;
				startWait.Signal();
			}
			virtual ~Thread( void ) {}
		};

		//---------
		template < class ResultType, class ParameterType >
		class Thread < ResultType, ParameterType, EmptyType >
			: public ThreadBase< ResultType, ParameterType, EmptyType >
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
		class Thread< void, ParameterType, EmptyType >
			:	public ThreadBase< void, ParameterType, EmptyType >
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
		class Thread< ResultType, void, EmptyType >
			:	public ThreadBase< ResultType, void,  EmptyType >
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

			ResultType GetWorkResult( void ) { return result.value; }

		}; // class Thread< ResultType, void >

		template< >
		class Thread< void, void, EmptyType >
			:	public ThreadBase< void, void, EmptyType >
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

			virtual ~Thread( void ) { }

			void GetWorkResult( void ) {}

		};

	} // namespace thread
} // namespace Fat Rat Library

#endif // FRL_THREAD_H_
