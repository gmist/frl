#ifndef FRL_TEST_BIND_FUNCTION_H_
#define FRL_TEST_BIND_FUNCTION_H_

#include "frl_types.h"
#include "frl_bind.h"

long f_0()
{
	return 17041L;
}

long f_1(long a)
{
	return a;
}

long f_2(long a, long b)
{
	return a + 10 * b;
}

long f_3(long a, long b, long c)
{
	return a + 10 * b + 100 * c;
}

long f_4(long a, long b, long c, long d)
{
	return a + 10 * b + 100 * c + 1000 * d;
}

long f_5(long a, long b, long c, long d, long e)
{
	return a + 10 * b + 100 * c + 1000 * d + 10000 * e;
}

long f_6(long a, long b, long c, long d, long e, long f)
{
	return a + 10 * b + 100 * c + 1000 * d + 10000 * e + 100000 * f;
}

long f_7(long a, long b, long c, long d, long e, long f, long g)
{
	return a + 10 * b + 100 * c + 1000 * d + 10000 * e + 100000 * f + 1000000 * g;
}

long f_8(long a, long b, long c, long d, long e, long f, long g, long h)
{
	return a + 10 * b + 100 * c + 1000 * d + 10000 * e + 100000 * f + 1000000 * g + 10000000 * h;
}

long f_9(long a, long b, long c, long d, long e, long f, long g, long h, long i)
{
	return a + 10 * b + 100 * c + 1000 * d + 10000 * e + 100000 * f + 1000000 * g + 10000000 * h + 100000000 * i;
}


frl::Bool f_0_test()
{
	return frl::Bind( f_0, frl::_1 )() == 17041L;
}

frl::Bool f_1_test()
{
	return frl::Bind( f_1, frl::_1 )(1) == 1L;
}

frl::Bool f_2_test()
{
	return frl::Bind( &f_2, frl::_1, frl::_2 ) (1, 2) == 21L;	
}

frl::Bool f_3_test()
{
	return frl::Bind( f_3, frl::_1, frl::_2, frl::_3 ) (1, 2, 3) == 321L;
}

frl::Bool f_4_test()
{
	return frl::Bind( f_4, frl::_1, frl::_2, frl::_3, frl::_4 ) (1, 2, 3, 4 ) == 4321L;
}

frl::Bool f_5_test()
{
	return frl::Bind( f_5, frl::_1, frl::_2, frl::_3, frl::_4, frl::_5 ) (1, 2, 3, 4, 5 ) == 54321L;
}

frl::Bool f_6_test()
{
	return frl::Bind( f_6, frl::_1, frl::_2, frl::_3, frl::_4, frl::_5, frl::_6 ) (1, 2, 3, 4, 5, 6 ) == 654321L;
}

frl::Bool f_7_test()
{
	return frl::Bind( f_7, frl::_1, frl::_2, frl::_3, frl::_4, frl::_5, frl::_6, frl::_7 ) (1, 2, 3, 4, 5, 6, 7 ) == 7654321L;
}

frl::Bool f_8_test()
{
	return frl::Bind( f_8, frl::_1, frl::_2, frl::_3, frl::_4, frl::_5, frl::_6, frl::_7, frl::_8 ) (1, 2, 3, 4, 5, 6, 7, 8 ) == 87654321L;
}

frl::Bool f_9_test()
{
	return frl::Bind( f_9, frl::_1, frl::_2, frl::_3, frl::_4, frl::_5, frl::_6, frl::_7, frl::_8, frl::_9 ) (1, 2, 3, 4, 5, 6, 7, 8, 9 ) == 987654321L;
}

#endif /* FRL_TEST_BIND_FUNCTION_H_ */
