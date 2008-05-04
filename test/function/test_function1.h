#ifndef test_function1_h_
#define test_function1_h_
#include "functions.h"
#include "struct_functions.h"

frl::Bool f_1_test_1()
{
	return frl::Function1< long, long >( f_1, 1 )() == 1L;
}

frl::Bool f_1_test_2()
{
	frl::Function1< long, long > f( f_1, 1 );
	return f() == 1L;
}

frl::Bool f_1_test_3()
{
	frl::Function1< long, long > f( f_1 );
	return f( 1 ) == 1L;
}

frl::Bool f_1_test_4()
{
	frl::Function1< long, long > f;
	return f( f_1, 1 ) == 1L;
}

frl::Bool f_1_struct_test_1()
{
	X1 x;
	return frl::Function1< long, X1, long >( x, &X1::f_1, 1 )() == 1L;
}

frl::Bool f_1_struct_test_2()
{
	X1 x;
	frl::Function1< long, X1, long > f( x, &X1::f_1, 1 );
	return f() == 1L;
}

frl::Bool f_1_struct_test_3()
{
	X1 x;
	frl::Function1< long, X1, long > f( x, &X1::f_1 );
	return f( 1 ) == 1L;
}

frl::Bool f_1_struct_test_4()
{
	frl::Function1< long, X1, long > f;
	X1 x;
	return f( x, &X1::f_1, 1 ) == 1L;
}

#endif // test_function1_h_
