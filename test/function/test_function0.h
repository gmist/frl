#ifndef test_function0_h_
#define test_function0_h_
#include "functions.h"
#include "struct_functions.h"

frl::Bool f_0_test_1()
{
	return frl::Function0< long >( f_0 )() == 17041L;
}

frl::Bool f_0_test_2()
{
	frl::Function0< long > f( f_0 );
	return f() == 17041L;
}

frl::Bool f_0_test_3()
{
	frl::Function0< long > f;
	return f( f_0 ) == 17041L;
}

frl::Bool f_0_struct_test_1()
{
	X0 x;
	return frl::Function0< long, X0 >( x, &X0::f_0 )() == 17041L;
}

frl::Bool f_0_struct_test_2()
{
	X0 x;
	frl::Function0< long, X0 > f( x, &X0::f_0 );
	return f() == 17041L;
}

frl::Bool f_0_struct_test_3()
{
	X0 x;
	frl::Function0< long, X0 > f;
	return f( x, &X0::f_0 ) == 17041L;
}

#endif // test_function0_h_
