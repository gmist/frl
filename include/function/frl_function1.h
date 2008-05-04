#ifndef frl_function1_h_
#define frl_function1_h_
#include "function/frl_function_base.h"
#include "frl_empty_type.h"

namespace frl
{
template< class R, class T, class P1 = frl::EmptyType >
class Function1 : public function::FunctionBase
{
public:
	typedef R ( T::*FunctionDef )( P1 );

	Function1() : t( NULL )
	{
	}

	Function1( T &tIn, FunctionDef funcIn )
	: t( &tIn ), func( funcIn )
	{
		this->isInit = False;
	}

	Function1( T &t_in, FunctionDef funct_in, const P1 &param_in )
	: t( &t_in ), func( funct_in ), param( param_in )
	{
		this->isInit = True;
	}

	R operator() () const
	{
		FRL_EXCEPT_GUARD();
		this->checkInit();
		return (t->*func)( param );
	}

	R operator() ( const P1 &param1In )
	{
		FRL_EXCEPT_GUARD();
		if( t == NULL )
		{
			this->isInit = False;
			this->checkInit();
		}
		param = param1In;
		return ( t->*func )( param );
	}

	R operator() ( T &tIn, FunctionDef funcIn, const P1 &param1In )
	{
		t = &tIn;
		func = funcIn;
		param = param1In;
		this->isInit = True;
		return ( t->*func )( param );
	}
	
	Function1& operator = ( const Function1 &rhv )
	{
		if( this == &rhv )
			return *this;
		if( rhv.isInit )
		{
			t = rhv.t;
			func = rhv.func;
			param = rhv.param;
			this->isInit = True;
		}
		else
		{
			this->isInit = False;
		}
	}

private:
	T *t;
	FunctionDef func;
	P1 param;
}; // Function1< R, T, P1 > for class methods

template< class R, class P1 >
class Function1< R, P1, EmptyType > : public function::FunctionBase
{
public:
	typedef R ( *FunctionDef )( P1 );

	Function1()
	{
	}

	Function1( FunctionDef funcIn )
	: func( funcIn )
	{
		this->isInit = False;
	}

	Function1( FunctionDef funct_in, const P1 &param_in )
	:	func( funct_in ), param( param_in )
	{
		this->isInit = True;
	}

	R operator() () const
	{
		FRL_EXCEPT_GUARD();
		this->checkInit();
		return (*func)( param );
	}

	R operator() ( const P1 &param1In )
	{
		FRL_EXCEPT_GUARD();
		param = param1In;
		return ( *func )( param );
	}

	R operator() ( FunctionDef funcIn, const P1 &param1In )
	{
		func = funcIn;
		param = param1In;
		this->isInit = True;
		return(*func)( param );
	}
	
	Function1& operator = ( const Function1 &rhv )
	{
		if( this == &rhv )
			return *this;
		if( rhv.isInit )
		{
			func = rhv.func;
			param = rhv.param;
			this->isInit = True;
		}
		else
		{
			this->isInit = False;
		}
	}

private:
	FunctionDef func;
	P1 param;
}; // Function1< R, EmptyType > for free functions

}

#endif // frl_function1_h_
