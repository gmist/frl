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
private:
	volatile ULong *counts;
public:

	OwnerRefCount() : counts( new ULong(1) )
	{
	}

	OwnerRefCount( const OwnerRefCount &rhv )
	{
		counts = rhv.counts;
	}

	T* addRef( T& ptr )
	{
		++*counts;
		return &ptr;
	}

	bool release()
	{
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
