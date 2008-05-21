#ifndef FRL_THREAD_H_
#define FRL_THREAD_H_

#include "thread/frl_thread_fn.h"
#include "frl_empty_type.h"
#include "frl_non_copyable.h"
#include "lock/frl_mutex.h"
#include "lock/frl_event.h"
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
	static void run( ThreadType *thread )
	{
		( (Type*)( thread->type )->*thread->function )();
	}
};

template< class ResultType, class Type >
struct FuncSelect< ResultType, void, Type >
{
	typedef ResultType ( Type::*FRL_THREAD_FUNC ) ( void );
	template< class ThreadType >
	static void run( ThreadType *thread )
	{
		thread->result.value = ( ( Type* )( thread->type )->*thread->function )();
	}
};

template< class ParameterType, class Type >
struct FuncSelect< void, ParameterType, Type >
{
	typedef void ( Type::*FRL_THREAD_FUNC ) ( ParameterType );
	template< class ThreadType >
	static void run( ThreadType *thread )
	{
		( ( Type* )( thread->type )->*thread->function )( thread->parameter.value );
	}
};

template < class ResultType, class ParameterType >
struct FuncSelect< ResultType, ParameterType, EmptyType >
{
	typedef ResultType ( *FRL_THREAD_FUNC ) ( ParameterType parameter_ );
	template< class ThreadType >
	static void run( ThreadType *thread )
	{
		thread->result.value = thread->function( thread->parameter.value );
	}
};

template < class ResultType >
struct FuncSelect< ResultType, void, EmptyType >
{
	typedef ResultType ( *FRL_THREAD_FUNC ) ( void );
	template< class ThreadType >
	static void run( ThreadType *thread )
	{
		thread->result.value = thread->function();
	}
};

template < class ParameterType >
struct FuncSelect< void, ParameterType, EmptyType >
{
	typedef void ( *FRL_THREAD_FUNC ) ( ParameterType parameter_ );
	template< class ThreadType >
	static void run( ThreadType *thread )
	{
		thread->function( thread->parameter.value );
	}
};

template < >
struct FuncSelect< void, void, EmptyType >
{
	typedef void ( *FRL_THREAD_FUNC ) ( void );
	template< class ThreadType >
	static void run( ThreadType *thread )
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
volatile Bool created;
volatile Bool running;
frl::lock::Event startWait;
frl::lock::Event joinWait;
ThreadDescriptor descriptor;
public:

MasterThreadBase()
: created( False ), running( False ), descriptor( InvalidThreadDescriptor )
{
}

virtual ~MasterThreadBase( void )
{
	scopeGuard.lock();
	FRL_EXCEPT_GUARD();
	this->kill();
	scopeGuard.unLock();
}

void create( Bool isDetached_ = False , UInt stackSize_ = 0 )
{
	lock::ScopeGuard guard( scopeGuard );
	FRL_EXCEPT_GUARD();
	if ( created )
		FRL_THROW( FRL_STR( "Thread already created." ) );
	T* pT = static_cast<T*> (this);
	if( pT->function == NULL )
		FRL_THROW( FRL_STR( "Pointer to threading function == NULL." ) );
	thread::create( descriptor, pT->runThread, pT, isDetached_, stackSize_ );
	created = True;
	if ( isDetached_ )
	{
		// FIXME
	}
}

Bool isRunning( void )
{
	return running;
}

void kill( void )
{
	FRL_EXCEPT_GUARD();
	if ( created || running )
	{
		startWait.signal();
		thread::kill( descriptor );
	}
	else
	{
		if( descriptor != InvalidThreadDescriptor )
		{
			#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
			::CloseHandle( descriptor );
			#endif
			descriptor = InvalidThreadDescriptor;
		}
	}
	created = running = frl::False;
}

void yield( void )
{
	thread::yield();
}

void join( void )
{
	lock::ScopeGuard guard( scopeGuard );
	FRL_EXCEPT_GUARD();
	if ( created && running )
		thread::join( descriptor );
	else
	{
		if( descriptor != InvalidThreadDescriptor )
		{
			#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
			::CloseHandle( descriptor );
			#endif
			descriptor = InvalidThreadDescriptor;
		}
	}
}

Bool join( frl::TimeOut msec_ )
{
	lock::ScopeGuard guard( scopeGuard );
	FRL_EXCEPT_GUARD();
	if ( created && running )
		return thread::join( descriptor, msec_ );
	else
	{
		if( descriptor != InvalidThreadDescriptor )
		{
			#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
			::CloseHandle( descriptor );
			#endif
			descriptor = InvalidThreadDescriptor;
		}
	}
	return False;
}

ThreadDescriptor self( void )
{
	return descriptor;
}

Bool operator == ( const MasterThreadBase &rvl ) throw()
{
	FRL_EXCEPT_GUARD();
	if( this == &rvl )
		return True;
	return thread::isEqual( this->descriptor, rvl.descriptor );
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

#if( FRL_COMPILER == FRL_COMPILER_MSVC )
friend private_::FuncSelect< ResultType, ParameterType, Type >;
#else
friend struct private_::FuncSelect< ResultType, ParameterType, Type >;
#endif

protected:
private_::ParamResultType< ResultType > result;
private_::ParamResultType< ParameterType > parameter;
const Type *type;
public:
typename private_::FuncSelect< ResultType, ParameterType, Type >::FRL_THREAD_FUNC function;

virtual ~ThreadBase() {}

#if defined WIN32
	static DWORD WINAPI runThread( void* parameter_ ) throw()
	{
		try
		{
			ThreadBase< ResultType, ParameterType, Type > *thread = reinterpret_cast< ThreadBase< ResultType, ParameterType, Type >* >( parameter_ );
			thread->startWait.wait();
			private_::FuncSelect< ResultType, ParameterType, Type >::run( thread );
			thread->running = thread->created = False;
		}
		catch ( ... ) {}
		return 0;
	}
#endif // WIN32

void create( typename private_::FuncSelect<ResultType,
				ParameterType, Type >::FRL_THREAD_FUNC function_,
				const Type &type_in ,
				Bool isDetached_  = False ,
				UInt stackSize_  = 0 )
{
	lock::ScopeGuard guard(  MasterThreadBase< ThreadBase< ResultType, ParameterType, Type > >::scopeGuard );
	FRL_EXCEPT_GUARD();
	type = &type_in;
	function = function_;
	 MasterThreadBase< ThreadBase< ResultType, ParameterType, Type > >::create( isDetached_, stackSize_ );
}
};

template< class ResultType, class ParameterType >
class ThreadBase< ResultType, ParameterType, EmptyType >
:	private NonCopyable,
	public MasterThreadBase< ThreadBase< ResultType, ParameterType, EmptyType > >,
	protected private_::FuncSelect< ResultType, ParameterType >
{

#if( FRL_COMPILER == FRL_COMPILER_MSVC )
friend private_::FuncSelect< ResultType, ParameterType, EmptyType >;
#else
friend struct private_::FuncSelect< ResultType, ParameterType, EmptyType >;
#endif

protected:
private_::ParamResultType< ResultType > result;
private_::ParamResultType< ParameterType > parameter;
public:
typename private_::FuncSelect< ResultType, ParameterType >::FRL_THREAD_FUNC function;

virtual ~ThreadBase() {}

 #if defined WIN32
	static DWORD WINAPI runThread( void* parameter_ ) throw()
	{
		try
		{
			ThreadBase< ResultType, ParameterType > *thread = reinterpret_cast< ThreadBase< ResultType, ParameterType >* >( parameter_ );
			thread->startWait.wait();
			private_::FuncSelect< ResultType, ParameterType >::run( thread );
			thread->running = thread->created = False;
		}
		catch ( ... ) {}
		return 0;
	}
#endif // WIN32

void create( typename private_::FuncSelect<ResultType,
				ParameterType >::FRL_THREAD_FUNC function_,
				Bool isDetached_  = False ,
				UInt stackSize_  = 0 )
{
	FRL_EXCEPT_GUARD();
	function = function_;
	 MasterThreadBase< ThreadBase< ResultType, ParameterType, EmptyType > >::create( isDetached_, stackSize_ );
}
};

template< class ResultType, class ParameterType, class Type = EmptyType >
class Thread : public ThreadBase< ResultType, ParameterType, Type >
{
public:
typedef ResultType ( *FRL_THREAD_FUNC ) ( ParameterType parameter_ );

Thread( void )
{
	ThreadBase< ResultType, ParameterType, Type >::function = NULL;
}

Thread( FRL_THREAD_FUNC function_, ParameterType parameter_, const Type &type_ )
{
	ThreadBase< ResultType, ParameterType, Type >::function = function_;
	ThreadBase< ResultType, ParameterType, Type >::parameter.value = parameter_;
	ThreadBase< ResultType, ParameterType, Type >::type = type_;
}

void start( ParameterType parameter_ )
{
	lock::ScopeGuard guard( ThreadBase< ResultType, ParameterType, Type >::scopeGuard );
	FRL_EXCEPT_GUARD();
	if( ! ThreadBase< ResultType, ParameterType, Type >::created )
		FRL_THROW( FRL_STR( "Thread not created. create thread before use function Thread::start()" ) );
	if ( ThreadBase< ResultType, ParameterType, Type >::running )
		FRL_THROW( FRL_STR( "Thread already created and running." ) );
	ThreadBase< ResultType, ParameterType, Type >::parameter.value = parameter_;
	ThreadBase< ResultType, ParameterType, Type >::running = True;
	ThreadBase< ResultType, ParameterType, Type >::startWait.signal();
}

virtual ~Thread( void ) {}

ResultType getWorkResult( void ) { return ThreadBase< ResultType, ParameterType, Type >::result.value; }
};

template< class Type >
class Thread< void, void, Type > : public ThreadBase< void, void, Type >
{
public:
typedef void ( *FRL_THREAD_FUNC ) ( void );

Thread( void )
{
	ThreadBase< void, void, Type >::function = NULL;
}

Thread( FRL_THREAD_FUNC function_, const Type &type_ )
{
	ThreadBase< void, void, Type >::function = function_;
	ThreadBase< void, void, Type >::type = type_;
}

void start( void )
{
	lock::ScopeGuard guard( ThreadBase< void, void, Type >::scopeGuard );
	FRL_EXCEPT_GUARD();
	if( ! ThreadBase< void, void, Type >::created )
		FRL_THROW( FRL_STR( "Thread not created. create thread before use function Thread::start()" ) );
	if ( ThreadBase< void, void, Type >::running )
		FRL_THROW( FRL_STR( "Thread already created and running." ) );
	ThreadBase< void, void, Type >::running = True;
	ThreadBase< void, void, Type >::startWait.signal();
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
	ThreadBase< ResultType, void, Type >::function = NULL;
}

Thread( FRL_THREAD_FUNC function_,  const Type &type_ )
{
	ThreadBase< ResultType, void, Type >::function = function_;
	ThreadBase< ResultType, void, Type >::type = type_;
}

void start( void )
{
	lock::ScopeGuard guard( ThreadBase< ResultType, void, Type >::scopeGuard );
	FRL_EXCEPT_GUARD();
	if( ! ThreadBase< ResultType, void, Type >::created )
		FRL_THROW( FRL_STR( "Thread not created. create thread before use function Thread::start()" ) );
	if ( ThreadBase< ResultType, void, Type >::running )
		FRL_THROW( FRL_STR( "Thread already created and running." ) );
	ThreadBase< ResultType, void, Type >::running = True;
	ThreadBase< ResultType, void, Type >::startWait.signal();
}

virtual ~Thread( void ) {}

ResultType getWorkResult( void )
{
	return ThreadBase< ResultType, void, Type >::result.value;
}
};

template< class ParameterType, class Type >
class Thread< void, ParameterType, Type>
	: public ThreadBase< void, ParameterType, Type >
{
public:
typedef void ( *FRL_THREAD_FUNC ) ( ParameterType parameter_ );

Thread( void )
{
	ThreadBase< void, ParameterType, Type >::function = NULL;
}

Thread( FRL_THREAD_FUNC function_, ParameterType parameter_,  const Type &type_ )
{
	ThreadBase< void, ParameterType, Type >::function = function_;
	ThreadBase< void, ParameterType, Type >::parameter.value = parameter_;
	ThreadBase< void, ParameterType, Type >::type = type_;
}

void start( ParameterType parameter_ )
{
	lock::ScopeGuard guard( ThreadBase< void, ParameterType, Type >::scopeGuard );
	FRL_EXCEPT_GUARD();
	if( ! ThreadBase< void, ParameterType, Type >::created )
		FRL_THROW( FRL_STR( "Thread not created. create thread before use function Thread::start()" ) );
	if ( ThreadBase< void, ParameterType, Type >::running )
		FRL_THROW( FRL_STR( "Thread already created and running." ) );
	ThreadBase< void, ParameterType, Type >::parameter.value = parameter_;
	ThreadBase< void, ParameterType, Type >::running = True;
	ThreadBase< void, ParameterType, Type >::startWait.signal();
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
	ThreadBase< ResultType, ParameterType, EmptyType >::function = NULL;
}

Thread( FRL_THREAD_FUNC function_, ParameterType parameter_ )
{
	ThreadBase< ResultType, ParameterType, EmptyType >::function = function_;
	ThreadBase< ResultType, ParameterType, EmptyType >::parameter.value = parameter_;
}

virtual ~Thread( void )
{

}

ResultType getWorkResult( void ) { return ThreadBase< ResultType, ParameterType, EmptyType >::result.value; }

}; // Thread< ResultType, ParametrType >

template< class ParameterType >
class Thread< void, ParameterType, EmptyType >
:	public ThreadBase< void, ParameterType, EmptyType >
{
public:
typedef void ( *FRL_THREAD_FUNC ) ( ParameterType parameter_ );

Thread( void )
{
	ThreadBase< void, ParameterType, EmptyType >::function = NULL;
}

Thread( FRL_THREAD_FUNC function_, ParameterType parameter_ )
{
	ThreadBase< void, ParameterType, EmptyType >::function = function_;
	ThreadBase< void, ParameterType, EmptyType >::parameter.value = parameter_;
}

virtual ~Thread( void ) {}

void getWorkResult( void ) { }

}; // class Thread< void, ParameterType >

template < class ResultType >
class Thread< ResultType, void, EmptyType >
:	public ThreadBase< ResultType, void,  EmptyType >
{
public:
typedef ResultType ( *FRL_THREAD_FUNC ) ( void );

Thread( void )
{
	ThreadBase< ResultType, void,  EmptyType >::function = NULL;
}

Thread( FRL_THREAD_FUNC function_)
{
	ThreadBase< ResultType, void,  EmptyType >::function = function_;
}

virtual ~Thread( void ) {}

void start( void )
{
	lock::ScopeGuard guard( ThreadBase< ResultType, void,  EmptyType >::scopeGuard );
	FRL_EXCEPT_GUARD();
	if( ! ThreadBase< ResultType, void,  EmptyType >::created )
		FRL_THROW( FRL_STR( "Thread not created. create thread before use function Thread::start()" ) );
	if ( ThreadBase< ResultType, void,  EmptyType >::running )
		FRL_THROW( FRL_STR( "Thread already created and running." ) );
	ThreadBase< ResultType, void,  EmptyType >::running = True;
	ThreadBase< ResultType, void,  EmptyType >::startWait.signal();
}

ResultType getWorkResult( void ) { return ThreadBase< ResultType, void,  EmptyType >::result.value; }

}; // class Thread< ResultType, void >

template< >
class Thread< void, void, EmptyType >
:	public ThreadBase< void, void, EmptyType >
{
public:
typedef void ( *FRL_THREAD_FUNC ) ( void );

Thread( void )
{
	ThreadBase< void, void, EmptyType >::function = NULL;
}

Thread( FRL_THREAD_FUNC function_ )
{
	ThreadBase< void, void, EmptyType >::function = function_;
}

void start( void )
{
	lock::ScopeGuard guard( ThreadBase< void, void, EmptyType >::scopeGuard );
	FRL_EXCEPT_GUARD();
	if( ! ThreadBase< void, void, EmptyType >::created )
		FRL_THROW( FRL_STR( "Thread not created. create thread before use function Thread::start()" ) );
	if ( ThreadBase< void, void, EmptyType >::running )
		FRL_THROW( FRL_STR( "Thread already created and running." ) );
	ThreadBase< void, void, EmptyType >::running = True;
	ThreadBase< void, void, EmptyType >::startWait.signal();
}

virtual ~Thread( void ) { }

void getWorkResult( void ) {}

};

} // namespace thread
} // FatRat Library

#endif // FRL_THREAD_H_
