#ifndef FRL_UNIT_TEST_H_
#define FRL_UNIT_TEST_H_

#include "frl_platform.h"

#include "frl_string.h"
#include "unit_test/frl_unit_test_statistics.h"
#include "unit_test/frl_console_out_policy.h"
#include <boost/noncopyable.hpp>
#include <conio.h>

namespace frl
{
namespace unit_test
{
// Check function prototype
typedef frl::Bool ( *CHECK_FUNC ) ( void  );

// class for unit testing
template < class OutPolicy >
class UnitTest
	:	private boost::noncopyable
{
private:
	frl::String name; // test name
	OutPolicy outPolicy;	// out policy
	UnitTestStatistics statistics; // tests statistics

	// Default constructor
	UnitTest( void ) { }
public:

	// Constructor
	UnitTest( frl::Char *name_in )
	{
		if( name_in == NULL )
			name = FRL_STR( "Unknown" );
		else
			name = name_in;

		outPolicy.PrintUnitTestName( name );

		statistics.testsNumber = 0;
		statistics.okTestsNumber = 0;
		statistics.failTestsNumber = 0;
	}

	// Destructor
	~UnitTest( void )
	{
		outPolicy.PrintStatistics( statistics );
		frl::console_std::Out << FRL_STR( "Press any key for exit." );
		_getch();
	}

	// Add test
	void Add( CHECK_FUNC check_func_in, const frl::Char *testName_in = NULL, frl::Bool stopTestingOnFail = False  )
	{
		frl::String testName;
		if( testName_in == NULL )
			testName = FRL_STR( "Unknown" );
		else
			testName = testName_in;

		++statistics.testsNumber;

		if( check_func_in() )
		{
			 outPolicy.PrintTestOk( testName, statistics );
			 ++statistics.okTestsNumber;
		}
		else
		{
			outPolicy.PrintTestFail( testName, statistics );
			++statistics.failTestsNumber;
			if( stopTestingOnFail )
			{
				outPolicy.printIfExitOnFailTest();
				outPolicy.PrintStatistics( statistics );
				exit(0);
			}
		}
	}
}; // class UnitTest

typedef UnitTest< frl::unit_test::ConsoleOutPolicy > ConsoleUnitTest;

}// namespace unit_test
}// FatRat Library

#endif /*FRL_UNIT_TEST_H_*/
