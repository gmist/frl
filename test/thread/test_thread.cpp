#if defined ( __linux__ )
	#include <unistd.h>
#endif 

#include "frl_types.h"
#include "thread/frl_thread.h"
#include "frl_unit_test.h"
#include "frl_exception.h"

unsigned int global_i;

void ThreadFunc( void )
{
	for( unsigned int i = 0; i < 1000; i++)
	{
		global_i++;
	}
}

void NullThreadFunc( void )
{
}

void LongTimeThread( void )
{
	unsigned long i;
	for( i = 0; i < 10000; i++ )
	{
		Sleep(10);
		global_i++;
	}
}

bool test_ThreadCreate( void )
{	
	try
	{
		frl::thread::Thread<void, void> test_thread;
		test_thread.Create( NullThreadFunc, true );	
	}
	catch ( const frl::Exception *e)
	{
		frl::console_std::Out << e->GetFullDescription();
		throw e;
		return false;
	}
	return true;
	
}

bool  test_ThreadJoin( void )
{
	try
	{
		frl::thread::Thread<void, void> test_thread;
		test_thread.Create( ThreadFunc );
		global_i = 5;
		test_thread.Start();
		test_thread.Join();
	}
	catch ( frl::Exception& e)
	{
		frl::console_std::Out << e.GetFullDescription();
		throw e;
		return false;
	}

	if( global_i == 1005 )
		return true;
	return false;
}



bool test_ThreadKill( void )
{
	try
	{
		frl::thread::Thread<void, void> test_thread;
		global_i = 10;
		test_thread.Create( LongTimeThread );
		test_thread.Start();
		test_thread.Kill();
		test_thread.Join();
		if( global_i == 10010 || test_thread.Self() != frl::thread::InvalidThreadDescriptor )
			return false;
		return true;
	}
	catch ( frl::Exception& e)
	{
		frl::console_std::Out << e.GetFullDescription();
		throw e;
		return false;
	}
	return false;
}

bool test_ThreadEqual( void )
{
	try
	{
		frl::thread::Thread< void, void > testThread_1;
		frl::thread::Thread< void, void > testThread_2;
		testThread_1.Create( NullThreadFunc );
		testThread_2.Create( NullThreadFunc );	
		return ( testThread_1 == testThread_1 ) && ( testThread_1 != testThread_2 );
	}
	catch ( frl::Exception& e)
	{
		frl::console_std::Out << e.GetFullDescription();
		throw e;
		return false;
	}
	return true;
	
}

int main( int argc, char *argv[] )
{	
	FRL_EXCEPT_GUARD();
	try
	{
		frl::unit_test::ConsoleUnitTest test_thread( FRL_STR("frl::thread::Thread" ));
		test_thread.Add( test_ThreadCreate, FRL_STR("Thread::Create()") );
		test_thread.Add( test_ThreadJoin, FRL_STR("Thread::Join()") );
		test_thread.Add( test_ThreadKill, FRL_STR("Thread::Kill()") );
		test_thread.Add( test_ThreadEqual, FRL_STR("Thread::operator ==, Thread::operator!=") );
	}
	catch ( frl::Exception& e)
	{
		frl::console_std::Out << e.GetFullDescription();
		exit(0);
	}
	catch ( ... )
	{
		frl::console_std::Out << "UnknownError" << std::endl;
		exit(0);
	}
}
