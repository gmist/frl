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
		frl::thread::Thread< void, void > test_thread;
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


frl::Long g_value = 0L;

frl::Bool testClassVoidVoid()
{
	int vlaue = 0;
	try
	{	
		struct X
		{
			void aa( void )
			{
				g_value = 1L;
			}
		};
	
		X x;
		frl::thread::Thread< void, void, X > thread1;
		thread1.Create( &X::aa, x );
		thread1.Start();
		thread1.Join();
		if( g_value == 1L )
		{
			return frl::True;
		}
		return frl::False;
	}
	catch( frl::Exception &ex )
	{
		ex.~Exception();
		return frl::False;
	}
	return frl::False;
}


frl::Bool testClassLongVoid()
{
	try
	{	
		struct X
		{
			frl::Long aa( void )
			{
				return 12345L;
			}
		};
		X x;
		frl::thread::Thread< frl::Long, void, X > thread1;
		thread1.Create( &X::aa, x );
		thread1.Start();
		thread1.Join();
		frl::Long res = thread1.GetWorkResult();
		if( res == 12345L )
		{
			return frl::True;
		}
		return frl::False;
	}
	catch( frl::Exception &ex )
	{
		ex.~Exception();
		return frl::False;
	}
	return frl::False;
}

frl::Bool testClassVoidLong()
{
	try
	{	
		struct X
		{
			void aa( frl::Long a )
			{
				g_value = a;
			}
		};
		X x;
		frl::thread::Thread< void, frl::Long, X > thread1;
		thread1.Create( &X::aa, x );
		thread1.Start( 12345L );
		thread1.Join();
		if( g_value == 12345L )
		{
			return frl::True;
		}
		return frl::False;
	}
	catch( frl::Exception &ex )
	{
		ex.~Exception();
		return frl::False;
	}
	return frl::False;
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
		test_thread.Add( testClassVoidVoid, FRL_STR("Thread class (void, void)") );
		test_thread.Add( testClassLongVoid, FRL_STR("Thread class (void, void)") );
		test_thread.Add( testClassVoidLong, FRL_STR("Thread class (void, void)") );
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
