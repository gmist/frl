#include "frl_function.h"
#include "frl_unit_test.h"
#include "test_function0.h"
#include "test_function1.h"
#include "test_function2.h"

int main( int argc, char* argv )
{
	frl::unit_test::ConsoleUnitTest fTest( FRL_STR("Function test.") );
	fTest.Add( f_0_test_1 );
	fTest.Add( f_0_test_2 );
	fTest.Add( f_0_test_3 );
	
	fTest.Add( f_0_struct_test_1 );
	fTest.Add( f_0_struct_test_2 );
	fTest.Add( f_0_struct_test_3 );
	
	fTest.Add( f_1_test_1 );
	fTest.Add( f_1_test_2 );
	fTest.Add( f_1_test_3 );
	fTest.Add( f_1_test_4 );
	
	fTest.Add( f_1_struct_test_1 );
	fTest.Add( f_1_struct_test_2 );
	fTest.Add( f_1_struct_test_3 );
	fTest.Add( f_1_struct_test_4 );
	
	fTest.Add( f_2_test_1 );
	fTest.Add( f_2_test_3 );
	fTest.Add( f_2_test_2 );
	fTest.Add( f_2_test_4 );
	
	fTest.Add( f_2_struct_test_1 );
	fTest.Add( f_2_struct_test_2 );
	fTest.Add( f_2_struct_test_3 );
	fTest.Add( f_2_struct_test_4 );

	return 0;
}
