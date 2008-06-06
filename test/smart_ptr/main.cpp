#include "frl_unit_test.h"
#include "frl_smart_ptr.h"
#include <vector>
#include <list>


static int counter = 0;
static int comRefCounter = 0;

class X
{
public:
	X()
	{
		++counter;
	}
	X( const X& X )
	{
		++counter;
	}
	~X()
	{
		--counter;
	}

	int a()
	{
		return 1;
	}
	
	int b()
	{
		return 2;
	}

};

class X_COM
{
volatile long refCount;
public:
	X_COM() : refCount( 0 )
	{
		++counter;
	}
	
	~X_COM()
	{
		--counter;
	}
	
	void AddRef()
	{
		++comRefCounter;
		++refCount;
	}
	
	void Release()
	{
		--comRefCounter;
		--refCount;
		if( refCount == 0 )
			delete this;
	}
	
};

frl::Bool SimplyOwnerCOM()
{
	counter = 0;
	{
		frl::ComPtr< X_COM > ptr1( new X_COM );
		frl::ComPtr< X_COM > ptr2 = ptr1;
		frl::ComPtr< X_COM > ptr3 = ptr2;
		frl::ComPtr< X_COM > ptr4 = ptr1;
		if( comRefCounter != 4 || counter != 1 )
			return frl::False;
		frl::ComPtr< X_COM > ptr5( new X_COM );
		ptr4 = ptr5;
		if( comRefCounter != 5 || counter != 2 )
			return frl::False;
		ptr4 = frl::ComPtr< X_COM >( new X_COM() );
		if( comRefCounter != 5 || counter != 3 )
			return frl::False;
	}
	return ( counter == 0 && comRefCounter == 0 );
}

int main( int argc, char* argv )
{
	frl::unit_test::ConsoleUnitTest smartPtrTest( FRL_STR( "frl::SmartPtr" ) );
	smartPtrTest.Add( SimplyOwnerCOM, FRL_STR("SimplyOwnerCOM") );
	return 0;
}
