#ifndef FRL_TEST_BIND_CLASS_METHODS_H_
#define FRL_TEST_BIND_CLASS_METHODS_H_

#include "frl_types.h"
#include "frl_bind.h"
#include "test_functions.h"

struct X0
{
	long call() { return frl::Bind( f_0 )(); }
};

frl::Bool test_X0()
{
	X0 x;
	return frl::Bind( &X0::call, x )() == f_0();	
}

struct X1
{
	long call( long a1 )
	{
		return frl::Bind( f_1, frl::_1 )( a1 );
	}
};

frl::Bool test_X1()
{
	X1 x;	
	return frl::Bind( &X1::call, x, frl::_1 )( 1 ) == f_1( 1 );
}

struct X2
{
	long call( long a1, long a2 )
	{
		return frl::Bind( f_2, frl::_1, frl::_2 )( a1, a2 );
	}
};

frl::Bool test_X2()
{
	X2 x;	
	return frl::Bind( &X2::call, x, frl::_1, frl::_2 )( 1, 2 ) == f_2( 1, 2 );
}

struct X3
{
	long call( long a1, long a2, long a3 )
	{
		return frl::Bind( f_3, frl::_1, frl::_2, frl::_3 )( a1, a2, a3 );
	}
};

frl::Bool test_X3()
{
	X3 x;	
	return frl::Bind( &X3::call, x, frl::_1, frl::_2, frl::_3 )( 1, 2, 3 ) == f_3( 1, 2, 3 );
}

struct X4
{
	long call( long a1, long a2, long a3, long a4 )
	{
		return frl::Bind( f_4, frl::_1, frl::_2, frl::_3, frl::_4 )( a1, a2, a3, a4 );
	}
};

frl::Bool test_X4()
{
	X4 x;	
	return frl::Bind( &X4::call, x, frl::_1, frl::_2, frl::_3, frl::_4 )( 1, 2, 3, 4 ) == f_4( 1, 2, 3, 4 );
}

struct X5
{
	long call( long a1, long a2, long a3, long a4, long a5 )
	{
		return frl::Bind( f_5, frl::_1, frl::_2, frl::_3, frl::_4, frl::_5 )( a1, a2, a3, a4, a5 );
	}
};

frl::Bool test_X5()
{
	X5 x;	
	return frl::Bind( &X5::call, x, frl::_1, frl::_2, frl::_3, frl::_4, frl::_5 )( 1, 2, 3, 4, 5 ) == f_5( 1, 2, 3, 4, 5 );
}

struct X6
{
	long call( long a1, long a2, long a3, long a4, long a5, long a6 )
	{
		return frl::Bind( f_6, frl::_1, frl::_2, frl::_3, frl::_4, frl::_5, frl::_6 )( a1, a2, a3, a4, a5, a6 );
	}
};

frl::Bool test_X6()
{
	X6 x;	
	return frl::Bind( &X6::call, x, frl::_1, frl::_2, frl::_3, frl::_4, frl::_5, frl::_6 )( 1, 2, 3, 4, 5, 6 ) 
					== f_6( 1, 2, 3, 4, 5, 6 );
}

struct X7
{
	long call( long a1, long a2, long a3, long a4, long a5, long a6, long a7 )
	{
		return frl::Bind( f_7, frl::_1, frl::_2, frl::_3, frl::_4, frl::_5, frl::_6, frl::_7 )( a1, a2, a3, a4, a5, a6, a7 );
	}
};

frl::Bool test_X7()
{
	X7 x;	
	return frl::Bind( &X7::call, x, frl::_1, frl::_2, frl::_3, frl::_4, frl::_5, frl::_6, frl::_7 )( 1, 2, 3, 4, 5, 6, 7 )
					== f_7( 1, 2, 3, 4, 5, 6, 7 );
}

struct X8
{
	long call( long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8 )
	{
		return frl::Bind( f_8, frl::_1, frl::_2, frl::_3, frl::_4, frl::_5, frl::_6, frl::_7, frl::_8 )( a1, a2, a3, a4, a5, a6, a7, a8 );
	}
};

frl::Bool test_X8()
{
	X8 x;	
	return frl::Bind( &X8::call, x, frl::_1, frl::_2, frl::_3, frl::_4, frl::_5, frl::_6, frl::_7, frl::_8 )( 1, 2, 3, 4, 5, 6, 7, 8 )
					== f_8( 1, 2, 3, 4, 5, 6, 7, 8 );
}

struct X9
{
	long call( long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8, long a9 )
	{
		return frl::Bind( f_9, frl::_1, frl::_2, frl::_3, frl::_4, frl::_5, frl::_6, frl::_7, frl::_8, frl::_9 )( a1, a2, a3, a4, a5, a6, a7, a8, a9 );
	}
};

frl::Bool test_X9()
{
	X9 x;	
	return frl::Bind( &X9::call, x, frl::_1, frl::_2, frl::_3, frl::_4, frl::_5, frl::_6, frl::_7, frl::_8, frl::_9 )( 1, 2, 3, 4, 5, 6, 7, 8, 9 )
					== f_9( 1, 2, 3, 4, 5, 6, 7, 8, 9 );
}

#endif /* FRL_TEST_BIND_CLASS_METHODS_H_ */
