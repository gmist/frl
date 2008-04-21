#ifndef frl_bind_class_methods_h_
#define frl_bind_class_methods_h_

#include "bind/frl_list.h"

namespace frl
{
namespace bind
{
template < typename R, typename TYPE >
class BindT0
{
private:
	typedef R ( TYPE::*fn )();
	fn fn_;
	TYPE *t_;

public:
	BindT0( fn fn_in, TYPE &t_in ) : fn_( fn_in ), t_ ( &t_in ) { }
	R operator () () { return ( *t_.*fn_ )(); }
};

template
< 
	typename R, typename TYPE,
	typename Arg1,
	typename P1
 >
class BindT1
	:	public bind::List1PH<P1>
{
private:
	typedef R ( TYPE::*fn )( Arg1 );
	fn fn_;
	TYPE *t_;

public:
	BindT1( fn fn_in, TYPE &t_in ) : fn_( fn_in ), t_ ( &t_in ) { }
	R operator () ( Arg1 a1 ) 
	{ 
		frl::bind::List1< Arg1 > list( a1 );
		return ( *t_.*fn_ )( list[this->typeArg1] );
	}
};

template
< 
	typename R, typename TYPE,
	typename Arg1, typename Arg2, 
	typename P1, typename P2
 >
class BindT2 : private bind::List2PH< P1, P2 >
{
private:
	typedef R ( TYPE::*fn )( Arg1, Arg2 );
	fn fn_;
	TYPE *t_;

public:
	BindT2( fn fn_in, TYPE &t_in ) : fn_( fn_in ), t_ ( &t_in ) { }
	R operator () ( Arg1 a1, Arg2 a2 ) 
	{ 
		frl::bind::List2< Arg1,Arg2 > list( a1, a2 );
		return ( *t_.*fn_ )( list[this->typeArg1], list[this->typeArg2] );
	}
};

template
< 
	typename R, typename TYPE,
	typename Arg1, typename Arg2,
	typename Arg3,
	typename P1, typename P2,
	typename P3
 >
class BindT3 : private bind::List3PH< P1, P2, P3 >
{
private:
	typedef R ( TYPE::*fn )( Arg1, Arg2, Arg3 );
	fn fn_;
	TYPE *t_;

public:
	BindT3( fn fn_in, TYPE &t_in ) : fn_( fn_in ), t_ ( &t_in ) { }
	R operator () ( Arg1 a1, Arg2 a2, Arg3 a3 ) 
	{ 
		frl::bind::List3< Arg1,Arg2, Arg3 > list( a1, a2, a3 );
		return ( *t_.*fn_ )( list[this->typeArg1], list[this->typeArg2], list[this->typeArg3] );
	}
};
template
< 
	typename R, typename TYPE,
	typename Arg1, typename Arg2,
	typename Arg3, typename Arg4,
	typename P1, typename P2,
	typename P3, typename P4
 >
class BindT4 : private bind::List4PH< P1, P2, P3, P4 >
{
private:
	typedef R ( TYPE::*fn )( Arg1, Arg2, Arg3, Arg4 );
	fn fn_;
	TYPE *t_;

public:
	BindT4( fn fn_in, TYPE &t_in ) : fn_( fn_in ), t_ ( &t_in ) { }
	R operator () ( Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4 ) 
	{ 
		frl::bind::List4< Arg1,Arg2, Arg3, Arg4 > list( a1, a2, a3, a4 );
		return ( *t_.*fn_ )( list[this->typeArg1], list[this->typeArg2], list[this->typeArg3], list[this->typeArg4] );
	}
};

template
< 
	typename R, typename TYPE,
	typename Arg1, typename Arg2,
	typename Arg3, typename Arg4,
	typename Arg5,
	typename P1, typename P2,
	typename P3, typename P4,
	typename P5
 >
class BindT5 : private bind::List5PH< P1, P2, P3, P4, P5 >
{
private:
	typedef R ( TYPE::*fn )( Arg1, Arg2, Arg3, Arg4, Arg5 );
	fn fn_;
	TYPE *t_;

public:
	BindT5( fn fn_in, TYPE &t_in ) : fn_( fn_in ), t_ ( &t_in ) { }
	R operator () ( Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5 ) 
	{ 
		frl::bind::List5< Arg1,Arg2, Arg3, Arg4, Arg5 > list( a1, a2, a3, a4, a5 );
		return ( *t_.*fn_ )( list[this->typeArg1], list[this->typeArg2], list[this->typeArg3], list[this->typeArg4], list[this->typeArg5] );
	}
};

template
< 
	typename R, typename TYPE,
	typename Arg1, typename Arg2,
	typename Arg3, typename Arg4,
	typename Arg5, typename Arg6,
	typename P1, typename P2,
	typename P3, typename P4,
	typename P5, typename P6
 >
class BindT6 : private bind::List6PH< P1, P2, P3, P4, P5, P6 >
{
private:
	typedef R ( TYPE::*fn )( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6 );
	fn fn_;
	TYPE *t_;

public:
	BindT6( fn fn_in, TYPE &t_in ) : fn_( fn_in ), t_ ( &t_in ) { }
	R operator () ( Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6 ) 
	{ 
		frl::bind::List6< Arg1,Arg2, Arg3, Arg4, Arg5, Arg6 > list( a1, a2, a3, a4, a5, a6 );
		return ( *t_.*fn_ )( list[this->typeArg1], list[this->typeArg2], list[this->typeArg3],
								list[this->typeArg4], list[this->typeArg5], list[this->typeArg6] );
	}
};

template
<
	 typename R, typename TYPE,
	typename Arg1, typename Arg2,
	typename Arg3, typename Arg4,
	typename Arg5, typename Arg6,
	typename Arg7,
	typename P1, typename P2,
	typename P3, typename P4,
	typename P5, typename P6,
	typename P7
 >
class BindT7 : private bind::List7PH< P1, P2, P3, P4, P5, P6, P7 >
{
private:
	typedef R ( TYPE::*fn )( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7 );
	fn fn_;
	TYPE *t_;

public:
	BindT7( fn fn_in, TYPE &t_in ) : fn_( fn_in ), t_ ( &t_in ) { }
	R operator () ( Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6, Arg7 a7 ) 
	{ 
		frl::bind::List7< Arg1,Arg2, Arg3, Arg4, Arg5, Arg6, Arg7 > list( a1, a2, a3, a4, a5, a6, a7 );
		return ( *t_.*fn_ )( list[this->typeArg1], list[this->typeArg2], list[this->typeArg3], list[this->typeArg4],
								list[this->typeArg5], list[this->typeArg6], list[this->typeArg7] );
	}
};

template
<
	 typename R, typename TYPE,
	typename Arg1, typename Arg2,
	typename Arg3, typename Arg4,
	typename Arg5, typename Arg6,
	typename Arg7, typename Arg8,
	typename P1, typename P2,
	typename P3, typename P4,
	typename P5, typename P6,
	typename P7, typename P8
 >
class BindT8 : private bind::List8PH< P1, P2, P3, P4, P5, P6, P7, P8 >
{
private:
	typedef R ( TYPE::*fn )( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8 );
	fn fn_;
	TYPE *t_;

public:
	BindT8( fn fn_in, TYPE &t_in ) : fn_( fn_in ), t_ ( &t_in ) { }
	R operator () ( Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6, Arg7 a7, Arg8 a8 ) 
	{ 
		frl::bind::List8< Arg1,Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8 > list( a1, a2, a3, a4, a5, a6, a7, a8 );
		return ( *t_.*fn_ )( list[this->typeArg1], list[this->typeArg2], list[this->typeArg3], list[this->typeArg4],
								list[this->typeArg5], list[this->typeArg6], list[this->typeArg7], list[this->typeArg8] );
	}
};

template
< 
	typename R, typename TYPE,
	typename Arg1, typename Arg2,
	typename Arg3, typename Arg4,
	typename Arg5, typename Arg6,
	typename Arg7, typename Arg8,
	typename Arg9,
	typename P1, typename P2,
	typename P3, typename P4,
	typename P5, typename P6,
	typename P7, typename P8,
	typename P9
 >
class BindT9 : private bind::List9PH< P1, P2, P3, P4, P5, P6, P7, P8, P9 >
{
private:
	typedef R ( TYPE::*fn )( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9 );
	fn fn_;
	TYPE *t_;

public:
	BindT9( fn fn_in, TYPE &t_in ) : fn_( fn_in ), t_ ( &t_in ) { }
	R operator () ( Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6, Arg7 a7, Arg8 a8, Arg9 a9 ) 
	{ 
		frl::bind::List9< Arg1,Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9 > list( a1, a2, a3, a4, a5, a6, a7, a8, a9 );
		return ( *t_.*fn_ )( list[this->typeArg1], list[this->typeArg2], list[this->typeArg3], list[this->typeArg4], list[this->typeArg5],
								list[this->typeArg6], list[this->typeArg7], list[this->typeArg8], list[this->typeArg9] );
	}
};
} // namespace bind

template< typename R, typename TYPE >
bind::BindT0< R, TYPE > Bind( R( TYPE::*fn )(), const TYPE &t )
{
	return bind::BindT0<R, TYPE>( fn, (TYPE&)t );
}

template
< 
	typename R, typename TYPE,
	typename Arg1, typename P1
 >
bind::BindT1< R, TYPE, Arg1, P1 > Bind( R ( TYPE::*fn ) ( Arg1), const TYPE &t, const P1& )
{
	return bind::BindT1< R, TYPE, Arg1, P1 > ( fn, (TYPE&)t );
}

template
< 
	typename R, typename TYPE,
	typename Arg1, typename Arg2,
	typename P1, typename P2
 >
bind::BindT2< R, TYPE, Arg1,Arg2, P1, P2 > Bind( R ( TYPE::*fn ) ( Arg1, Arg2 ), const TYPE &t, const P1&, const P2& )
{
	return bind::BindT2< R, TYPE, Arg1,Arg2,  P1, P2 > ( fn, (TYPE&)t );
}

template
< 
	typename R, typename TYPE,
	typename Arg1, typename Arg2, typename Arg3,
	typename P1, typename P2, typename P3
	 >
bind::BindT3< R, TYPE, Arg1,Arg2, Arg3, P1, P2, P3 > 
Bind( R ( TYPE::*fn ) ( Arg1, Arg2, Arg3 ), const TYPE &t, const P1&, const P2&, const P3& )
{
	return bind::BindT3< R, TYPE, Arg1,Arg2, Arg3, P1, P2, P3 > ( fn, (TYPE&)t );
}

template 
< 
	typename R, typename TYPE,
	typename Arg1, typename Arg2, typename Arg3,
	typename Arg4,
	typename P1, typename P2, typename P3,
	typename P4
 >
bind::BindT4< R, TYPE, Arg1,Arg2, Arg3, Arg4, P1, P2, P3, P4 > 
Bind( R ( TYPE::*fn ) ( Arg1, Arg2, Arg3, Arg4 ), const TYPE &t, const P1&, const P2&, const P3&, const P4& )
{
	return bind::BindT4< R, TYPE, Arg1,Arg2, Arg3, Arg4, P1, P2, P3, P4 > ( fn, (TYPE&)t );
}

template 
< 
	typename R, typename TYPE,
	typename Arg1, typename Arg2, typename Arg3,
	typename Arg4, typename Arg5,
	typename P1, typename P2, typename P3,
	typename P4, typename P5
 >
bind::BindT5< R, TYPE, Arg1,Arg2, Arg3, Arg4, Arg5, P1, P2, P3, P4, P5 > 
Bind( R ( TYPE::*fn ) ( Arg1, Arg2, Arg3, Arg4, Arg5 ),
		const TYPE &t, const P1&, const P2&, const P3&, const P4&, const P5& )
{
	return bind::BindT5< R, TYPE, Arg1,Arg2, Arg3, Arg4, Arg5, P1, P2, P3, P4, P5 > ( fn, (TYPE&)t );
}

template 
< 
	typename R, typename TYPE,
	typename Arg1, typename Arg2, typename Arg3,
	typename Arg4, typename Arg5, typename Arg6,
	typename P1, typename P2, typename P3,
	typename P4, typename P5, typename P6
 >
bind::BindT6< R, TYPE, Arg1,Arg2, Arg3, Arg4, Arg5, Arg6, P1, P2, P3, P4, P5, P6 > 
Bind( R ( TYPE::*fn ) ( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6 ),
		const TYPE &t, const P1&, const P2&, const P3&, const P4&, const P5&, const P6& )
{
	return bind::BindT6< R, TYPE, Arg1,Arg2, Arg3, Arg4, Arg5, Arg6,
									P1, P2, P3, P4, P5, P6 > ( fn, (TYPE&)t );
}

template 
< 
	typename R, typename TYPE,
	typename Arg1, typename Arg2, typename Arg3,
	typename Arg4, typename Arg5, typename Arg6,
	typename Arg7,
	typename P1, typename P2, typename P3,
	typename P4, typename P5, typename P6,
	typename P7
 >
bind::BindT7< R, TYPE, Arg1,Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, P1, P2, P3, P4, P5, P6, P7 > 
Bind( R ( TYPE::*fn ) ( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7 ),
const TYPE &t, const P1&, const P2&, const P3&, const P4&, const P5&, const P6&, const P7& )
{
	return bind::BindT7< R, TYPE, Arg1,Arg2, Arg3, Arg4, Arg5, Arg6, Arg7,
									P1, P2, P3, P4, P5, P6, P7 > ( fn, (TYPE&)t );
}

template
< 
	typename R, typename TYPE,
	typename Arg1, typename Arg2, typename Arg3,
	typename Arg4, typename Arg5, typename Arg6,
	typename Arg7, typename Arg8,
	typename P1, typename P2, typename P3,
	typename P4, typename P5,
	typename P6, typename P7,
	typename P8
 >
bind::BindT8< R, TYPE, Arg1,Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, P1, P2, P3, P4, P5, P6, P7, P8 > 
Bind( R ( TYPE::*fn ) ( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8 ),
const TYPE &t, const P1&, const P2&, const P3&, const P4&, const P5&, const P6&, const P7&, const P8& )
{
	return bind::BindT8< R, TYPE, Arg1,Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8,
									P1, P2, P3, P4, P5, P6, P7, P8 > ( fn, (TYPE&)t );
}

template
< 
	typename R, typename TYPE,
	typename Arg1, typename Arg2, typename Arg3,
	typename Arg4, typename Arg5, typename Arg6,
	typename Arg7, typename Arg8, typename Arg9,
	typename P1, typename P2, typename P3,
	typename P4, typename P5, typename P6,
	typename P7, typename P8, typename P9
 >
bind::BindT9< R, TYPE, Arg1,Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, P1, P2, P3, P4, P5, P6, P7, P8, P9 > 
Bind( R ( TYPE::*fn ) ( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9 ),
		const TYPE &t, const P1&, const P2&, const P3&, const P4&, const P5&, const P6&, const P7&, const P8&, const P9& )
{
	return bind::BindT9< R, TYPE, Arg1,Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9,
									P1, P2, P3, P4, P5, P6, P7, P8, P9 > ( fn, (TYPE&)t );
}

} // FatRat Library

#endif // frl_bind_class_methods_h_
