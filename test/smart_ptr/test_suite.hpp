#ifndef smart_ptr_test_suite_h_
#define smart_ptr_test_suite_h_
#include <boost/test/unit_test.hpp>
#include "frl_smart_ptr.h"
#include "x_com.hpp"

BOOST_AUTO_TEST_SUITE( smart_ptr )

BOOST_AUTO_TEST_CASE( simply_owner_com )
{
	using namespace frl::private_test;
	counter = 0;
	comRefCounter = 0;
	{
		frl::ComPtr< X_COM > ptr1( new X_COM );
		frl::ComPtr< X_COM > ptr2 = ptr1;
		frl::ComPtr< X_COM > ptr3 = ptr2;
		frl::ComPtr< X_COM > ptr4 = ptr1;

		BOOST_CHECK_EQUAL( comRefCounter, 4 );
		BOOST_CHECK_EQUAL( counter, 1 );

		frl::ComPtr< X_COM > ptr5( new X_COM );
		ptr4 = ptr5;

		BOOST_CHECK_EQUAL( comRefCounter, 5 );
		BOOST_CHECK_EQUAL( counter, 2 );

		ptr4 = frl::ComPtr< X_COM >( new X_COM() );
		BOOST_CHECK_EQUAL( comRefCounter, 5 );
		BOOST_CHECK_EQUAL( counter, 3 );
	}
	BOOST_CHECK_EQUAL( counter, 0 );
	BOOST_CHECK_EQUAL( comRefCounter, 0 );
}

BOOST_AUTO_TEST_SUITE_END()

#endif // smart_ptr_test_suite_h_
