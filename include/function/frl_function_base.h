#ifndef frl_function_base_h_
#define frl_function_base_h_
#include "frl_exception.h"
#include "frl_auto_value.h"

namespace frl
{
namespace function
{

class FunctionBase
{
protected:
	AutoValue< Bool, False > isInit;
public:
	FRL_EXCEPTION_CLASS( FunctionNotInitialize );
	void checkInit() const
	{
		FRL_EXCEPT_GUARD();
		if( ! isInit )
			FRL_THROW_S_CLASS( FunctionNotInitialize );
	}
};

}
} // FatRat Library

#endif // frl_function_base_h_
