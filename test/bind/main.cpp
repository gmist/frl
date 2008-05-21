#include "test_functions.h"
#include "test_class_methods.h"
#include "frl_unit_test.h"


int main( int argc, char *argv[] )
{
	frl::unit_test::ConsoleUnitTest uBind( FRL_STR( "frl::Bind" ) );
	uBind.Add( f_0_test, FRL_STR("Zero argumet Bind test") );
	uBind.Add( f_1_test, FRL_STR("One argumet Bind test") );
	uBind.Add( f_2_test, FRL_STR("Two arguments Bind test") );
	uBind.Add( f_3_test, FRL_STR("Three arguments Bind test") );
	uBind.Add( f_4_test, FRL_STR("Four arguments Bind test") );
	uBind.Add( f_5_test, FRL_STR("Five arguments Bind test") );
	uBind.Add( f_6_test, FRL_STR("Six arguments Bind test") );
	uBind.Add( f_7_test, FRL_STR("Seven arguments Bind test") );
	uBind.Add( f_8_test, FRL_STR("Eight arguments Bind test") );
	uBind.Add( f_9_test, FRL_STR("Nine arguments Bind test") );

	uBind.Add( test_X0, FRL_STR("Zero argument class method Bind") );
	uBind.Add( test_X1, FRL_STR("One argument class method Bind") );
	uBind.Add( test_X2, FRL_STR("Two arguments class method Bind") );
	uBind.Add( test_X3, FRL_STR("Three arguments class method Bind") );
	uBind.Add( test_X4, FRL_STR("Four arguments class method Bind") );
	uBind.Add( test_X5, FRL_STR("Five arguments class method Bind") );
	uBind.Add( test_X6, FRL_STR("Six arguments class method Bind") );
	uBind.Add( test_X7, FRL_STR("Seven arguments class method Bind") );
	uBind.Add( test_X8, FRL_STR("Eigth arguments class method Bind") );
	uBind.Add( test_X9, FRL_STR("Nine arguments class method Bind") );
	return 0;
}
