#ifndef frl_smart_ptr_owner_ref_count_h_
#define frl_smart_ptr_owner_ref_count_h_
#include "frl_types.h"

namespace frl
{
namespace smart_ptr
{
template< class T >
class OwnerRefCount
{
public:
	FRL_EXCEPTION_CLASS( NullPtr );

private:
	volatile ULong *counts;
protected:

	OwnerRefCount( T *ptr )
	{
		if( ptr == NULL )
			FRL_THROW_S_CLASS( NullPtr );

		counts = new ULong( 1 );
	}

	OwnerRefCount( const OwnerRefCount &rhv )
	{
		counts = rhv.counts;
	}

	T* addRef( T *ptr )
	{
		if( ptr == NULL )
			FRL_THROW_S_CLASS( NullPtr );

		++*counts;
		return ptr;
	}

	Bool release( T *ptr )
	{
		if( ptr == NULL )
			FRL_THROW_S_CLASS( NullPtr );

		if( --*counts == 0 )
		{
			delete counts;
			counts = NULL;
			return true;
		}
		return false;
	}

	void swap( OwnerRefCount &rhv )
	{
		std::swap( counts, rhv.counts );
	}
};

} // namespace smart_ptr
} // FatRat Library

#endif // frl_smart_ptr_owner_ref_count_h_
