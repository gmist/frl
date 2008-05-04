#ifndef frl_smart_ptr_owner_com_h_
#define frl_smart_ptr_owner_com_h_
#include "frl_types.h"
#include "frl_exception.h"

namespace frl
{
namespace smart_ptr
{
template< class T >
class OwnerCOM
{
public:
	FRL_EXCEPTION_CLASS( NullPtr );
protected:

	OwnerCOM( T *ptr )
	{
		if( ptr == NULL )
			FRL_THROW_S_CLASS( NullPtr );
		ptr->AddRef();
	}

	OwnerCOM( const OwnerCOM &rhv )
	{
	}

	T* addRef( T *ptr )
	{
		if( ptr == NULL )
			FRL_THROW_S_CLASS( NullPtr );
		ptr->AddRef();
		return ptr;
	}

	Bool release( T *ptr )
	{
		if( ptr == NULL )
			FRL_THROW_S_CLASS( NullPtr );
		ptr->Release();
		return false;
	}

	void swap( OwnerCOM &rhv )
	{
	}
};

} // namespace smart_ptr
} // FatRat Library

#endif // frl_smart_ptr_owner_com_h_
