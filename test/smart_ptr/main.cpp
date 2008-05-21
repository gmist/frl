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

frl::Bool SimplyRefCount()
{
	counter = 0;
	{		
		frl::SmartPtr< X > ptr1;		
		frl::SmartPtr< X > ptr2( ptr1 );
		frl::SmartPtr< X > ptr3 = ptr1;
		if( counter != 1 )
			return frl::False;
		frl::SmartPtr< X > ptr4;
		if( counter != 2 )
			return frl::False;
		ptr3 = ptr4;
		ptr1 = ptr2;
	}
	return counter == 0;
}

frl::Bool SimplyRefCountArray()
{
	counter = 0;
	{
		std::vector< frl::SmartPtr< X > > vec1;
		vec1.push_back( frl::SmartPtr< X >() );
		vec1.push_back( frl::SmartPtr< X >() );
		vec1.push_back( frl::SmartPtr< X >() );
		
		if( counter != 3 )
			return frl::False;

		std::vector< frl::SmartPtr< X > > vec2;
		vec2 = vec1;
		std::vector< frl::SmartPtr< X > > vec3;
		vec3 = vec2;
		if( counter != 3 )
			return frl::False;

		vec3.push_back( frl::SmartPtr< X >() );
		if( counter != 4 )
			return frl::False;
	}
	return counter == 0;
}

frl::Bool SimplyRefCountArrayItertor()
{
	counter = 0;
	{
		std::list< frl::SmartPtr< X > > vec1;
		vec1.push_back( frl::SmartPtr< X >() );
		vec1.push_back( frl::SmartPtr< X >() );
		vec1.push_back( frl::SmartPtr< X >() );
		if( counter != 3 )
			return frl::False;

		std::list< frl::SmartPtr< X > >::iterator beg = vec1.begin();
		vec1.erase( beg );
		if( counter != 2 )
			return frl::False;
	}
	return counter == 0;
}

frl::Bool SimplyRefCountArrayPop()
{
	counter = 0;
	{
		std::vector< frl::SmartPtr< X > > vec1;
		{
			frl::SmartPtr< X > ptr1;
			vec1.push_back( ptr1 );
			if( counter != 1 )
				return frl::False;
		}
		{
			vec1.pop_back();
		}
		if( counter == 0 )
			return frl::True;
	}
	return frl::False;
}

frl::Bool SimplyOwnerCopy()
{
	counter = 0;
	{
		frl::SmartPtr< X, frl::smart_ptr::OwnerCopy > ptr1;
		frl::SmartPtr< X, frl::smart_ptr::OwnerCopy > ptr2;
		if( counter != 2 )
			return frl::False;
		ptr2 = ptr1;
		if( counter != 2 )
			return frl::False;
	}
	return counter == 0;
}

frl::Bool SimplyOwnerLinked()
{
	counter = 0;
	{
		frl::SmartPtr< X, frl::smart_ptr::OwnerLinked > ptr1( new X );
		frl::SmartPtr< X, frl::smart_ptr::OwnerLinked > ptr2 = ptr1;
		frl::SmartPtr< X, frl::smart_ptr::OwnerLinked > ptr3 = ptr2;
		frl::SmartPtr< X, frl::smart_ptr::OwnerLinked > ptr4 = ptr1;
		if( counter != 1 )
			return frl::False;
		frl::SmartPtr< X, frl::smart_ptr::OwnerLinked > ptr5;
		ptr4 = ptr5;
		if( counter != 2 )
			return frl::False;
		ptr4 = new X();
		if( counter != 3 )
			return frl::False;
	}
	return counter == 0;
}

frl::Bool SimplyOwnerLinkedArrayPop()
{
	counter = 0;
	{
		std::vector< frl::SmartPtr< X, frl::smart_ptr::OwnerLinked >  > vec1;
		{
			frl::SmartPtr< X, frl::smart_ptr::OwnerLinked > ptr1;
			vec1.push_back( ptr1 );
		}
		if( counter != 1 )
			return frl::False;
		{
			vec1.pop_back();
		}
		if( counter == 0 )
			return frl::True;;
	}
	return frl::False;
}

frl::Bool SimplyOwnerLinkedArray()
{
	counter = 0;
	{
		std::vector< frl::SmartPtr< X, frl::smart_ptr::OwnerLinked > > vec1;
		vec1.push_back( frl::SmartPtr< X, frl::smart_ptr::OwnerLinked >() );
		vec1.push_back( frl::SmartPtr< X, frl::smart_ptr::OwnerLinked >() );
		vec1.push_back( frl::SmartPtr< X, frl::smart_ptr::OwnerLinked >() );

		if( counter != 3 )
			return frl::False;

		std::vector< frl::SmartPtr< X, frl::smart_ptr::OwnerLinked > > vec2;
		vec2 = vec1;
		std::vector< frl::SmartPtr< X, frl::smart_ptr::OwnerLinked > > vec3;
		vec3 = vec2;
		if( counter != 3 )
			return frl::False;

		vec3.push_back( frl::SmartPtr< X, frl::smart_ptr::OwnerLinked >() );
		if( counter != 4 )
			return frl::False;
	}
	return counter == 0;
}

frl::Bool SimplyOwnerCOM()
{
	counter = 0;
	{
		frl::SmartPtr< X_COM, frl::smart_ptr::OwnerCOM > ptr1( new X_COM );
		frl::SmartPtr< X_COM, frl::smart_ptr::OwnerCOM > ptr2 = ptr1;
		frl::SmartPtr< X_COM, frl::smart_ptr::OwnerCOM > ptr3 = ptr2;
		frl::SmartPtr< X_COM, frl::smart_ptr::OwnerCOM > ptr4 = ptr1;
		if( comRefCounter != 4 || counter != 1 )
			return frl::False;
		frl::SmartPtr< X_COM, frl::smart_ptr::OwnerCOM > ptr5;
		ptr4 = ptr5;
		if( comRefCounter != 5 || counter != 2 )
			return frl::False;
		ptr4 = new X_COM();
		if( comRefCounter != 5 || counter != 3 )
			return frl::False;
	}
	return ( counter == 0 && comRefCounter == 0 );
}



int main( int argc, char* argv )
{
	frl::unit_test::ConsoleUnitTest smartPtrTest( FRL_STR( "frl::SmartPtr" ) );
	smartPtrTest.Add( SimplyRefCount, FRL_STR("SimplyRefCount") );
	smartPtrTest.Add( SimplyRefCountArray, FRL_STR("SimplyRefCountArray") );
	smartPtrTest.Add( SimplyRefCountArrayItertor, FRL_STR("SimplyRefCountArrayItertor") );
	smartPtrTest.Add( SimplyRefCountArrayPop, FRL_STR("SimplyRefCountArrayPop") );
	smartPtrTest.Add( SimplyOwnerCopy, FRL_STR("SimplyOwnerCopy") );
	smartPtrTest.Add( SimplyOwnerLinked, FRL_STR("SimplyOwnerLinked") );
	smartPtrTest.Add( SimplyOwnerLinkedArrayPop, FRL_STR("SimplyOwnerLinkedArrayPop") );
	smartPtrTest.Add( SimplyOwnerLinkedArray, FRL_STR("SimplyOwnerLinkedArray") );
	smartPtrTest.Add( SimplyOwnerCOM, FRL_STR("SimplyOwnerCOM") );
	return 0;
}
