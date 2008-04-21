#ifndef frl_function_h_
#define frl_function_h_

#include "frl_empty_type.h"

namespace frl
{


template< class R, class T = frl::EmptyType >
class Function0
{
public:
	typedef R ( T::*FunctionDef )( void );
	Function0() {};
	Function0( T &t_in, FunctionDef func_in ) : t( &t_in ), func( func_in ) {}

	R operator() () const
	{
		return (t->*func)();
	}
	
	R operator() ( T &tIn, FunctionDef funcIn ) const
	{
		return (tIn->funcIn)();
	}
private:
	T *t;
	FunctionDef func;
};

template< class R >
class Function0< R, EmptyType >
{
public:
	typedef R ( *FunctionDef )( void );
	Function0() {};
	Function0( FunctionDef func_in ) : func( func_in ){}
	R operator() () const
	{
		return func();
	}
	
	R operator() ( FunctionDef funcIn ) const
	{
		return funcIn();
	}
private:
	FunctionDef func;
};

template< class R, class T, class P1 = frl::EmptyType >
class Function1
{
public:
	typedef R ( T::*FunctionDef )( P1 );
	Function1() {}
	Function1( T &tIn, FunctionDef funcIn ) : t( &tIn ), func( &funcIn ) {}
	Function1( T &t_in, FunctionDef funct_in, const P1 &param_in ) : t( &t_in ), func( funct_in ), param( param_in ) {}
	R operator() () const
	{
		return t->func( param );
	}
	R operator() ( const P1 &param1In ) const
	{
		return (t->*func)( param1In );
	}
	R operator() ( T &tIn, const P1 &param1In ) const
	{
		return (tIn.*func)( param1In );
	}
private:
	T *t;
	FunctionDef func;
	P1 param;
};

template< class R, class P1 >
class Function1< R, P1, EmptyType >
{
public:
	typedef R ( *FunctionDef )( P1 );
	Function1() {}
	Function1( FunctionDef funcIn ) : func( funcIn ) {}
	Function1( FunctionDef funcIn, const P1 &param1In )
	: func( funcIn ), param1( param1In ) {};
	R operator() () const
	{
		return func( param1 );
	}
	R operator() ( const P1 &param1In ) const
	{ 
		return func( param1In );
	}
	R operator() ( FunctionDef funcIn, const P1 &param1In ) const
	{ 
		return funcIn( param1In );
	}
private:
	FunctionDef func;
	P1 param1;	
};

template< class R, class T, class P1, class P2 = EmptyType >
class Function2
{
public:
	typedef R ( T::*FunctionDef )( P1, P2 );
	Function2() {}
	Function2( T &t_in, FunctionDef funcIn )
		: t( t_in ),  func( funcIn )
	{
	}
	
	Function2( T &t_in, FunctionDef funcIn, const P1 &param1In, const P2 &param2In )
	: t( t_in ),  func( funcIn ), param1( param1In ), param2( param2In )
	{
	}

	R operator() () const
	{
		return t->func( param1, param2 );
	}
	
	R operator() ( const P1 &param1In, const P2 &param2In ) const
	{
		return (t->*func)( param1In, param2In );
	}
	
	R operator() ( T &tIn, const P1 &param1In, const P2 &param2In ) const
	{
		return (tIn.*func)( param1In, param2In );
	}
private:	
	T *t;
	FunctionDef func;
	P1 param1;
	P2 param2;
};

template< class R, class P1, class P2 >
class Function2< R, P1, P2, EmptyType >
{
public:
	typedef R ( *FunctionDef )( P1, P2 );
	Function2() {}
	Function2( FunctionDef funcIn ) : func( funcIn )
	{
	}
	
	Function2( FunctionDef funcIn, const P1 &param1In, const P2 &param2In )
	: func( funcIn ), param1( param1In ), param2( param2In )
	{
	}
	R operator() ()
	{
		return func( param1, param2 );
	}
	
	R operator() ( const P1 &param1In, const P2 &param2In )
	{
		return func( param1In, param2In );
	}
	
	R operator() ( FunctionDef funcIn, const P1 &param1In, const P2 &param2In )
	{
		return funcIn( param1In, param2In );
	}
private:
	FunctionDef func;
	P1 param1;
	P2 param2;
};

} // FatRat Library

#endif // frl_function_h_
