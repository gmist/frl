#ifndef frl_smart_ptr_owner_com_h_
#define frl_smart_ptr_owner_com_h_
#include "frl_types.h"
#include "frl_exception.h"
#include <boost/detail/shared_count.hpp>

namespace frl
{
template< class T >
class ComPtr
{
private:
	T* ptr;

public:
	FRL_EXCEPTION_CLASS( NullPtr );

	ComPtr() : ptr( NULL )
	{	
	}

	explicit ComPtr( const T *ptr_ )
	:	ptr( const_cast< T* >( ptr_ ) )
	{
		if( ptr != NULL )
			ptr->AddRef();
	}
		
	ComPtr( const ComPtr< T > &other )
	:	ptr( other.ptr )
	{
		if( ptr != NULL )	
			ptr->AddRef();
	}
	
	~ComPtr()
	{
		if( ptr != NULL )
		{
			if( ptr->Release() == 0 )
			{
				ptr = NULL;
			}
		}
	}
	
	ComPtr& operator = ( const ComPtr< T > &rhv )
	{
		if( this == & rhv )
			return *this;
		ComPtr tmp( rhv );
		tmp.swap( *this );
		return *this;
	}
	
	T& operator * () const
	{
		if( ptr == NULL )
			FRL_THROW_S_CLASS( NullPtr );
		return ptr;
	}

	T* operator ->() const
	{
		if( ptr == NULL )
			FRL_THROW_S_CLASS( NullPtr );
		return ptr;
	}
	
	T* get() const
	{
		if( ptr == NULL )
			FRL_THROW_S_CLASS( NullPtr );
		return ptr;
	}
	
	void swap( ComPtr &other )
	{
		std::swap( ptr, other.ptr );
	}
}; // class ComPtr

} // FatRat Library

#endif // frl_smart_ptr_owner_com_h_
