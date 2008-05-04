#ifndef frl_function0_h_
#define frl_function0_h_
#include "function/frl_function_base.h"
#include "frl_empty_type.h"

namespace frl
{
template< class R, class T = frl::EmptyType >
class Function0 : public function::FunctionBase
{
public:
	typedef R ( T::*FunctionDef )( void );

	Function0() : t( NULL )
	{
	}

	Function0( T &t_in, FunctionDef func_in )
	: t( &t_in ), func( func_in )
	{
		FunctionBase::isInit = True;
	}
	
	R operator() () const
	{
		FRL_EXCEPT_GUARD();
		this->checkInit();
		return (t->*func)();
	}
	
	R operator() ( T &tIn, FunctionDef funcIn )
	{ 
		t = &tIn;
		func = funcIn;
		this->isInit = True;
		return (t->*func)();
	}

	Function0& operator = ( const Function0& rhv )
	{
		if( this == &rhv )
			return *this;
		if( rhv.isInit )
		{
			func = rhv.func;
			t = rhv.t;
			isInit = True;
		}
		else
		{
			isInit = False;
		}
		return *this;
	}
private:
	T *t;
	FunctionDef func;
}; // class Function0< R, T > for class method

template< class R >
class Function0< R, EmptyType > : public function::FunctionBase
{
public:
	typedef R ( *FunctionDef )( void );

	Function0()
	{
	}

	Function0( FunctionDef func_in )
	: func( func_in )
	{
		FunctionBase::isInit = True;
	}

	R operator() () const
	{
		return func();
	}
	
	R operator() ( FunctionDef funcIn )
	{ 
		func = funcIn;
		this->isInit = True;
		return func();
	}

	Function0& operator = ( const Function0& rhv )
	{
		if( this == &rhv )
			return *this;
		if( rhv.isInit )
		{
			func = rhv.func;
			FunctionBase::isInit = True;
		}
		else
		{
			FunctionBase::isInit = False;
		}
		return *this;
	}

private:
	FunctionDef func;
}; // class Function0< R, EmptyType > for free function

} // FatRat Library

#endif // frl_function0_h_
