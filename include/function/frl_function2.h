#ifndef frl_function2_h_
#define frl_function2_h_
#include "function/frl_function_base.h"
#include "frl_empty_type.h"

namespace frl
{
template< class R, class T, class P1, class P2 = EmptyType >
class Function2 : public function::FunctionBase
{
public:
	typedef R ( T::*FunctionDef )( P1, P2 );

	Function2() : t ( NULL )
	{
	}

	Function2( T &t_in, FunctionDef funcIn )
	: t( &t_in ),  func( funcIn )
	{
	}

	Function2( T &t_in, FunctionDef funcIn, const P1 &param1In, const P2 &param2In )
	: t( &t_in ),  func( funcIn ), param1( param1In ), param2( param2In )
	{
		this->isInit = True;
	}

	R operator() () const
	{
		FRL_EXCEPT_GUARD();
		this->checkInit();
		return (t->*func)( param1, param2 );
	}

	R operator() ( const P1 &param1In, const P2 &param2In )
	{
		FRL_EXCEPT_GUARD();
		if( t == NULL )
		{
			this->isInit = False;
			this->checkInit();
		}
		param1 = param1In;
		param2 = param2In;
		return (t->*func)( param1, param2 );
	}

	R operator() ( T &tIn, FunctionDef funcIn, const P1 &param1In, const P2 &param2In )
	{
		t = &tIn;
		func = funcIn;
		param1 = param1In;
		param2 =  param2In;
		this->isInit = True;
		return (t->*func)( param1, param2 );
	}
	
	Function2& operator = ( const Function2 &rhv )
	{
		if( this == &rhv )
			return *this;
		if( rhv.isInit )
		{
			t = rhv.t;
			func = rhv.func;
			param1 = rhv.param1;
			param2 = rhv.param2;
			this->isInit = True;
		}
		else
		{
			this->isInit = False;
		}
		return *this;
	}
	
private:
	T *t;
	FunctionDef func;
	P1 param1;
	P2 param2;
}; // Function2< R, T, P1, P2 > for class methods


template< class R, class P1, class P2 >
class Function2< R, P1, P2, EmptyType > : public function::FunctionBase
{
public:
	typedef R ( *FunctionDef )( P1, P2 );

	Function2()
	{
	}

	Function2( FunctionDef funcIn )
	: func( funcIn )
	{
	}

	Function2( FunctionDef funcIn, const P1 &param1In, const P2 &param2In )
	: func( funcIn ), param1( param1In ), param2( param2In )
	{
		this->isInit = True;
	}

	R operator() () const
	{
		FRL_EXCEPT_GUARD();
		this->checkInit();
		return ( *func )( param1, param2 );
	}

	R operator() ( const P1 &param1In, const P2 &param2In )
	{
		FRL_EXCEPT_GUARD(); // TODO check init
		param1 = param1In;
		param2 = param2In;
		return ( *func )( param1, param2 );;
	}

	R operator() ( FunctionDef funcIn, const P1 &param1In, const P2 &param2In )
	{
		func = funcIn;
		param1 = param1In;
		param2 =  param2In;
		this->isInit = True;
		return ( *func )( param1, param2 );
	}

	Function2& operator = ( const Function2 &rhv )
	{
		if( this == &rhv )
			return *this;
		if( rhv.isInit )
		{
			func = rhv.func;
			param1 = rhv.param1;
			param2 = rhv.param2;
			this->isInit = True;
		}
		else
		{
			this->isInit = False;
		}
		return *this;
	}

private:	
	FunctionDef func;
	P1 param1;
	P2 param2;
}; // Function2< R, P1, P2 > for free methods

}

#endif // frl_function2_h_
