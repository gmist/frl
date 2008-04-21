#include "frl_function.h"
#include "frl_unit_test.h"

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
	return frl::Function0< long >( f_0 )() == 17041L;
}

frl::Bool f_1_test()
{
	return frl::Function1< long, long >( f_1, 1 )() == 1L;
}

frl::Bool f_2_test()
{
	return frl::Function2< long, long, long>( f_2, 1, 2 )() == 21L;
}

frl::Bool f_0_testDelay()
{
	frl::Function0< long > f( f_0 );
	return f() == 17041L;
}

frl::Bool f_1_testDelay()
{
	frl::Function1< long, long > f( f_1, 1 );
	return f() == 1L;
}

frl::Bool f_0_testSemiDelay()
{
	frl::Function0< long > f( f_0 );
	return f() == 17041L;
}

frl::Bool f_1_testSemiDelay()
{
	frl::Function1< long, long > f( f_1 );
	return f( 1 ) == 1L;
}

frl::Bool f_2_testSemiDelay()
{
	frl::Function2< long, long, long> f( f_2 );
	return f( 1, 2 ) == 21L;
}

frl::Bool f_2_testDelay()
{
	frl::Function2< long, long, long> f( f_2, 1, 2 );
	return f() == 21L;
}

frl::Bool f_0_testFullDelay()
{
	frl::Function0< long > f;
	return f( f_0 ) == 17041L;
}

frl::Bool f_1_testFullDelay()
{
	frl::Function1< long, long > f;
	return f( f_1, 1 ) == 1L;
}

frl::Bool f_2_testFullDelay()
{
	frl::Function2< long, long, long> f;
	return f( f_2, 1, 2 ) == 21L;
}


int main( int argc, char* argv )
{
	frl::unit_test::ConsoleUnitTest fTest( FRL_STR("Function test.") );
	fTest.Add( f_0_test );
	fTest.Add( f_1_test );
	fTest.Add( f_2_test );
	
	fTest.Add( f_0_testDelay );
	fTest.Add( f_1_testDelay );
	fTest.Add( f_2_testDelay );
	
	fTest.Add( f_0_testSemiDelay );
	fTest.Add( f_1_testSemiDelay );
	fTest.Add( f_2_testSemiDelay );
	
	fTest.Add( f_0_testFullDelay );
	fTest.Add( f_1_testFullDelay );
	fTest.Add( f_2_testFullDelay );
	return 0;
}
