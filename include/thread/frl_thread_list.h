#ifndef FRL_THREAD_LIST_H_
#define FRL_THREAD_LIST_H_

#include "frl_thread.h"
#include "frl_lock.h"
#include <list>
#include <algorithm>

namespace frl
{
namespace thread
{		
template< class ResultType, class ParameterType >
class ThreadListBase
{
protected:
	std::list< Thread< ResultType, ParameterType > * > threadList;
	frl::lock::Mutex mutex;
public:

	ThreadListBase( void )
	{
	}
	
	~ThreadListBase( void )
	{
		frl::lock::ScopeGuard scopeGuard( mutex );
		FRL_EXCEPT_GUARD();
		for( std::list< Thread< ResultType, ParameterType >* >::iterator iter = threadList.begin(); iter != threadList.end(); ++ iter )
			delete *iter;
	}

	void Create( const Thread< ResultType, ParameterType > *thread_, Bool isDetached_ = False, UInt stackSize_ =0 )
	{
		frl::lock::ScopeGuard scopeGuard( mutex );
		FRL_EXCEPT_GUARD();
		std::list< Thread< ResultType, ParameterType >* >::iterator iter = std::find( threadList.begin(), threadList.end(), thread_ );
		if( iter == threadList.end() )
			FRL_THROW( FRL_STR( "Creating thread not found in thread list." ) );
		(*iter)->Create( isDetached_, stackSize_ );
	}

	void Start( const Thread< ResultType, ParameterType > *thread_ )
	{
		frl::lock::ScopeGuard scopeGuard( mutex );
		FRL_EXCEPT_GUARD();
		std::list< Thread< ResultType, ParameterType >* >::iterator iter = std::find( threadList.begin(), threadList.end(), thread_ );
		if( iter == threadList.end() )
			FRL_THROW( FRL_STR( "Running thread not found in thread list." ) );
		(*iter)->Start( );
	}

	void StartAll( void )
	{
		frl::lock::ScopeGuard scopeGuard( mutex );
		FRL_EXCEPT_GUARD();				
		for( std::list< Thread< ResultType, ParameterType >* >::iterator iter = threadList.begin(); iter != threadList.end(); ++ iter )
			(*iter)->start();
	}

	void Remove(  Thread< ResultType, ParameterType > *thread_ )
	{
		frl::lock::ScopeGuard scopeGuard( mutex );
		FRL_EXCEPT_GUARD();
		std::list< Thread< ResultType, ParameterType >* >::iterator iter = std::find( threadList.begin(), threadList.end(), thread_ );
		if( iter == threadList.end() )
			FRL_THROW( FRL_STR( "Receding thread not found in thread list." ) );
		threadList.remove( thread_ );
		delete thread_;
	}

	void Join( const Thread< ResultType, ParameterType > *thread_ )
	{
		frl::lock::ScopeGuard scopeGuard( mutex );
		FRL_EXCEPT_GUARD();
		std::list< Thread< ResultType, ParameterType >* >::iterator iter = std::find( threadList.begin(), threadList.end(), thread_ );
		if( iter == threadList.end() )
			FRL_THROW( FRL_STR( "Joinable thread not found in thread list." ) );
		(*iter)->Join();
		Remove( *iter );
	}

	Bool Join( const Thread< ResultType, ParameterType > *thread_, TimeOut msec_ )
	{
		frl::lock::ScopeGuard scopeGuard( mutex );
		FRL_EXCEPT_GUARD();
		std::list< Thread< ResultType, ParameterType >* >::iterator iter = std::find( threadList.begin(), threadList.end(), thread_ );
		if( iter == threadList.end() )
			FRL_THROW( FRL_STR( "Joinable thread not found in thread list." ) );
		return (*iter)->join( msec_ );
	}

	void JoinAll( void )
	{
		frl::lock::ScopeGuard scopeGuard( mutex );
		FRL_EXCEPT_GUARD();			
		for( std::list< Thread< ResultType, ParameterType >* >::iterator iter = threadList.begin(); iter != threadList.end(); ++iter )
		{
			(*iter)->Join();
		}
	}

	unsigned int Size( void )
	{
		return (unsigned int)threadList.size();
	}
	
};

template < class ResultType, class ParameterType >
class ThreadList
	:	public ThreadListBase< ResultType, ParameterType >
{
public:
	typedef ResultType ( *FRL_THREAD_FUNC ) ( ParameterType parameter_ );
	const Thread< ResultType, ParameterType >* AddNew( FRL_THREAD_FUNC function_, ParameterType parameter_  )
	{
		frl::lock::ScopeGuard scopeGuard( mutex );	
		FRL_EXCEPT_GUARD();
		Thread< ResultType, ParameterType > *thread = new Thread< ResultType, ParameterType >( function_, parameter_ );
		threadList.push_back( thread );
		std::list< Thread< ResultType, ParameterType >* >::iterator iter = std::find( threadList.begin(), threadList.end(), thread );
		if( iter == threadList.end() )
			FRL_THROW( FRL_STR( "New thread not found in thread list." ) );
		return (*iter);
	}
}; // class ThreadList< resultType ,ParameterType >

template < class ResultType >
class ThreadList< ResultType, void >
	:	public ThreadListBase< ResultType, void >
{
public:
	typedef ResultType ( *FRL_THREAD_FUNC ) ( void );
	const Thread< ResultType, void >* AddNew( FRL_THREAD_FUNC function_ )
	{
		frl::lock::ScopeGuard scopeGuard( mutex );
		FRL_EXCEPT_GUARD();
		Thread< ResultType, void > *thread = new Thread< ResultType, void >( function_ );				
		threadList.push_back( thread );
		std::list< Thread< ResultType, void >* >::iterator iter = std::find( threadList.begin(), threadList.end(), thread );
		if( iter == threadList.end() )
			FRL_THROW( FRL_STR( "New thread not found in thread list." ) );
		return (*iter);
	}
}; // // class ThreadList< ResultType ,void >

template < class ParameterType >
class ThreadList< void, ParameterType >
	:	public ThreadListBase< void, ParameterType >
{
public:
	typedef void ( *FRL_THREAD_FUNC ) ( ParameterType parameter_ );
	const Thread< void, ParameterType >* AddNew( FRL_THREAD_FUNC function_, ParameterType parameter_ )
	{
		frl::lock::ScopeGuard scopeGuard( mutex );
		FRL_EXCEPT_GUARD();
		Thread< void, ParameterType > *thread = new Thread< void, ParameterType >( function_, parameter_ );				
		threadList.push_back( thread );
		std::list< Thread< void, ParameterType >* >::iterator iter = std::find( threadList.begin(), threadList.end(), thread );
		if( iter == threadList.end() )
			FRL_THROW( FRL_STR( "New thread not found in thread list." ) );
		return (*iter);
	}
}; // class ThreadList< void ,ParameterType >

template < >
class ThreadList< void, void >
	:	public ThreadListBase< void, void >
{
public:
	typedef void ( *FRL_THREAD_FUNC ) ( void );
	const Thread< void, void >* AddNew( FRL_THREAD_FUNC function_ )
	{
		frl::lock::ScopeGuard scopeGuard( mutex );
		FRL_EXCEPT_GUARD();
		Thread< void, void > *thread = new Thread< void, void >( function_ );				
		threadList.push_back( thread );
		std::list< Thread< void, void >* >::iterator iter = std::find( threadList.begin(), threadList.end(), thread );
		if( iter == threadList.end() )
			FRL_THROW( FRL_STR( "New thread not found in thread list." ) );
		return (*iter);
	}
}; // class ThreadList< void ,void >

}// namespace thread

}	// FatRat Library

#endif /*FRL_THREAD_LIST_H_*/
