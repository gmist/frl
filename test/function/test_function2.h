#ifndef test_function2_h_
#define test_function2_h_
#include "functions.h"
#include "struct_functions.h"

frl::Bool f_2_test_1()
{
	return frl::Function2< long, long, long>( f_2, 1, 2 )() == 21L;
}

frl::Bool f_2_test_2()
{
	frl::Function2< long, long, long> f( f_2 );
	return f( 1, 2 ) == 21L;
}

frl::Bool f_2_test_3()
{
	frl::Function2< long, long, long> f( f_2, 1, 2 );
	return f() == 21L;
}

frl::Bool f_2_test_4()
{
	frl::Function2< long, long, long> f;
	return f( f_2, 1, 2 ) == 21L;
}

frl::Bool f_2_struct_test_1()
{
	X2 x;
	return frl::Function2< long, X2, long, long>( x, &X2::f_2, 1, 2 )() == 21L;
}

frl::Bool f_2_struct_test_2()
{
	X2 x;
	frl::Function2< long, X2, long, long> f( x, &X2::f_2 );
	return f( 1, 2 ) == 21L;
}

frl::Bool f_2_struct_test_3()
{
	X2 x;
	frl::Function2< long, X2, long, long> f( x, &X2::f_2, 1, 2 );
	return f() == 21L;
}

frl::Bool f_2_struct_test_4()
{	
	frl::Function2< long, X2, long, long> f;
	X2 x;
	return f( x, &X2::f_2, 1, 2 ) == 21L;
}

#endif // test_function2_h_
