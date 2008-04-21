#include "frl_smart_ptr.h"
#include "console_std/frl_iostream.h"
#include <vector>
#include <list>

class X
{
public:
	X()
	{
		frl::console_std::Out << FRL_STR("X()") << std::endl;
	}
	X( const X& X )
	{
		frl::console_std::Out << FRL_STR("copy X()") << std::endl;
	}
	~X()
	{
		frl::console_std::Out << FRL_STR("~X()") << std::endl;
	}
	int a()
	{
		frl::console_std::Out << FRL_STR("a()") << std::endl;
		return 1;
	}
	
	int b()
	{
		frl::console_std::Out << FRL_STR("b()") << std::endl;
		return 2;
	}
};

int main( int argc, char* argv )
{
	{
		frl::console_std::Out << FRL_STR("Test 1") << std::endl;
		frl::SmartPtr< X > ptr1;
		ptr1->a();
		frl::SmartPtr< X > ptr2( ptr1 );
		ptr2->b();
		frl::SmartPtr< X > ptr3;
		ptr3 = ptr1;
		ptr1 = ptr3;
	}
	
	{
		frl::console_std::Out << FRL_STR("Test 2") << std::endl;
		std::vector< frl::SmartPtr< X > > vec1;
		vec1.push_back( frl::SmartPtr< X >() );
		vec1.push_back( frl::SmartPtr< X >() );
		vec1.push_back( frl::SmartPtr< X >() );
		
		std::vector< frl::SmartPtr< X > > vec2;
		vec2 = vec1;
		std::vector< frl::SmartPtr< X > > vec3;
		vec3 = vec2;
		vec3.push_back( frl::SmartPtr< X >() );
	}
	
	{
		frl::console_std::Out << FRL_STR("Test 3") << std::endl;
		std::list< frl::SmartPtr< X > > vec1;
		vec1.push_back( frl::SmartPtr< X >() );
		vec1.push_back( frl::SmartPtr< X >() );
		vec1.push_back( frl::SmartPtr< X >() );
		
		std::list< frl::SmartPtr< X > >::iterator beg = vec1.begin();
		vec1.erase( beg );
		frl::console_std::Out << FRL_STR("----") << std::endl;
	}
	
	{
		frl::console_std::Out << FRL_STR("Test 4") << std::endl;
		frl::SmartPtr< X, frl::smart_ptr::OwnerCopy > ptr1;
		frl::SmartPtr< X, frl::smart_ptr::OwnerCopy > ptr2;
		ptr2 = ptr1;
	}
	
	{
		frl::console_std::Out << FRL_STR("Test 5") << std::endl;
		std::vector< frl::SmartPtr< X > > vec1;
		{
			frl::SmartPtr< X > ptr1;
			vec1.push_back( ptr1 );
			frl::console_std::Out << FRL_STR("-----") << std::endl;
		}		
		vec1.pop_back();
	}
	
	{
		frl::console_std::Out << FRL_STR("Test 6") << std::endl;
		frl::SmartPtr< X, frl::smart_ptr::OwnerLinked > ptr1( new X );
		frl::SmartPtr< X, frl::smart_ptr::OwnerLinked > ptr2 = ptr1;
		frl::SmartPtr< X, frl::smart_ptr::OwnerLinked > ptr3 = ptr2;
		frl::SmartPtr< X, frl::smart_ptr::OwnerLinked > ptr4 = ptr1;
		ptr4->a();
		ptr4 = new X();
		ptr4->b();
	}
	
	{
		frl::console_std::Out << FRL_STR("Test 7") << std::endl;
		std::vector< frl::SmartPtr< X, frl::smart_ptr::OwnerLinked >  > vec1;
		{
			frl::SmartPtr< X, frl::smart_ptr::OwnerLinked > ptr1;
			vec1.push_back( ptr1 );
			frl::console_std::Out << FRL_STR("-----") << std::endl;
		}		
		vec1.pop_back();
	}
	
	return 0;
}
