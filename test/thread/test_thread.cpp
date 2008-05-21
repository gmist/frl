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
		test_thread.create( NullThreadFunc, true );
	}
	catch ( const frl::Exception *e)
	{
		frl::console_std::Out << e->getFullDescription();
		return false;
	}
	return true;

}

bool  test_ThreadJoin( void )
{
	try
	{
		frl::thread::Thread<void, void> test_thread;
		test_thread.create( ThreadFunc );
		global_i = 5;
		test_thread.start();
		test_thread.join();
	}
	catch ( frl::Exception& e)
	{
		frl::console_std::Out << e.getFullDescription();
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
		test_thread.create( LongTimeThread );
		test_thread.start();
		test_thread.kill();
		test_thread.join();
		if( global_i == 10010 || test_thread.self() != frl::thread::InvalidThreadDescriptor )
			return false;
		return true;
	}
	catch ( frl::Exception& e)
	{
		frl::console_std::Out << e.getFullDescription();
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
		testThread_1.create( NullThreadFunc );
		testThread_2.create( NullThreadFunc );
		return ( testThread_1 == testThread_1 ) && ( testThread_1 != testThread_2 );
	}
	catch ( frl::Exception& e)
	{
		frl::console_std::Out << e.getFullDescription();
		return false;
	}
	return true;
}


frl::Long g_value = 0L;

struct X
{
	void aa( void )
	{
		g_value = 1L;
	}
};

frl::Bool testClassVoidVoid()
{
	try
	{
		X x;
		frl::thread::Thread< void, void, X > thread1;
		thread1.create( &X::aa, x );
		thread1.start();
		thread1.join();
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


struct X1
{
	frl::Long aa( void )
	{
		return 12345L;
	}
};

frl::Bool testClassLongVoid()
{
	try
	{
		X1 x;
		frl::thread::Thread< frl::Long, void, X1 > thread1;
		thread1.create( &X1::aa, x );
		thread1.start();
		thread1.join();
		frl::Long res = thread1.getWorkResult();
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

struct X2
{
	void aa( frl::Long a )
	{
		g_value = a;
	}
};

frl::Bool testClassVoidLong()
{
	try
	{
		X2 x;
		frl::thread::Thread< void, frl::Long, X2 > thread1;
		thread1.create( &X2::aa, x );
		thread1.start( 12345L );
		thread1.join();
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
		frl::console_std::Out << e.getFullDescription();
		exit(0);
	}
	catch ( ... )
	{
		frl::console_std::Out << "UnknownError" << std::endl;
		exit(0);
	}
}
