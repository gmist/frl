#ifndef frl_console_out_policy_h_
#define frl_console_out_policy_h_

#include "frl_platform.h"
#include "frl_string.h"
#include "unit_test/frl_unit_test_statistics.h"
#include "console_std/frl_iostream.h"

namespace frl
{
namespace unit_test
{
// class definition out policy for class UnitTest
class ConsoleOutPolicy
{
private:
	void PrintTestNumberAndName( const frl::String &name_in, const frl::unit_test::UnitTestStatistics &statistics_in )
	{
		console_std::ColorOut( console_std::foregroundColor::iDefault )
			<< statistics_in.testsNumber << FRL_STR( ". " );
		frl::console_std::Out << FRL_STR("Test: ") << name_in;
	}

	void PrintSeparator( void )
	{
		console_std::ColorOut( console_std::foregroundColor::iBlue )
			<< FRL_STR("---------------------------------------") << std::endl;
	}

public:
	ConsoleOutPolicy( void ) { }
	~ConsoleOutPolicy( void ) { }
	void PrintUnitTestName( const frl::String &name_in )
	{
		PrintSeparator();
		frl::console_std::Out << FRL_STR( "Unit test: " ) << name_in << std::endl;
	}

	void PrintStatistics( const frl::unit_test::UnitTestStatistics &statistics_in )
	{
		PrintSeparator();
		frl::console_std::ColorOut( console_std::foregroundColor::iDefault ) 
			<< FRL_STR( "Unit test results:" ) << std::endl;
		
		frl::console_std::Out << FRL_STR( "Tests number: " );
		console_std::ColorOut( console_std::foregroundColor::iDefault )
			<< statistics_in.testsNumber << std::endl;
		
		frl::console_std::Out << FRL_STR( "Tests ok: " );
		console_std::ColorOut( console_std::foregroundColor::iGreen )
			<< statistics_in.okTestsNumber << std::endl;
		
		frl::console_std::Out << FRL_STR( "Tests fail: " );
		console_std::ColorOut( console_std::foregroundColor::iRed )
			<< statistics_in.failTestsNumber << std::endl;
		PrintSeparator();
	}

	void PrintTestOk( const frl::String &name_in, const frl::unit_test::UnitTestStatistics &statistics_in )
	{
		PrintTestNumberAndName( name_in, statistics_in );
		console_std::ColorOut( console_std::foregroundColor::iGreen )
			<< FRL_STR( " - OK" ) << std::endl;
	}

	void PrintTestFail( const frl::String &name_in, const frl::unit_test::UnitTestStatistics &statistics_in )
	{
		PrintTestNumberAndName( name_in, statistics_in );
		console_std::ColorOut ( console_std::foregroundColor::iRed )
			<< FRL_STR( " - FAIL" ) << std::endl;	
	}

	void printIfExitOnFailTest()
	{
		PrintSeparator();
		frl::console_std::ColorOut(console_std::foregroundColor::iRed ) << FRL_STR("Force exit form testing !!!") << std::endl;
	}
}; // class ConsoleOutPolicy
}; // namespace unit_test
}; // FatRat Library

#endif // frl_console_out_policy_h_
