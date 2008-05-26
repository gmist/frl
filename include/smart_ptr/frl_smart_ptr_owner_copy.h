#ifndef frl_smart_ptr_owner_copy_h_
#define frl_smart_ptr_owner_copy_h_
#include "frl_types.h"
#include "frl_exception.h"

namespace frl
{
namespace smart_ptr
{

template< class T >
class OwnerCopy
{
public:
	FRL_EXCEPTION_CLASS( NullPtr );
protected:
	OwnerCopy( T *ptr )
	{
	}

	T* addRef( T *ptr)
	{
		if( ptr == NULL )
			FRL_THROW_S_CLASS( NullPtr );
		return new T( *ptr );
	}

	Bool release( T *ptr ) 
	{ 
		return true;
	}
	void swap( OwnerCopy &rhv ) {}
};

} // namespace smart_ptr
} // FatRat Library

#endif // frl_smart_ptr_owner_copy_h_
