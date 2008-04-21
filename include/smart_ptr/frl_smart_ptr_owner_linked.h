#ifndef frl_smart_ptr_owner_linked_h_
#define frl_smart_ptr_owner_linked_h_

namespace frl
{
namespace smart_ptr
{
template< class T >
class OwnerLinked
{
private:
	mutable const OwnerLinked< T > *prev;
	mutable const OwnerLinked< T > *next;
public:
public:
	OwnerLinked() 
	{ 
		prev = next = NULL;
	}

	OwnerLinked( const OwnerLinked& rhs )
	{
		rhs.next = this;
		prev = &rhs;
		next = NULL;
		
		if( prev )
			prev->next = this;
	}

	T* addRef( T &ptr )
	{
		return &ptr;
	}
	
	bool release()
	{
		if( prev )
			prev->next = next;
		if( next )
			next->prev = prev;
		return next == NULL && prev == NULL;
	}

	void swap( OwnerLinked& other )
	{
	
		// решение проблемы #2
		bool p_neighbor = ( prev == &other );
		bool n_neighbor = ( next == &other );
		
		std::swap( prev, other.prev );
		std::swap( next, other.next );

		if( p_neighbor ) 
		{
			prev = &other;
			other.next = this;
		}
		else if ( n_neighbor )
		{
			prev = this;
			other.next = &other;
		}

		if( prev )
			prev->next = this;
		if( next )
			next->prev = this;
		
		if( other.prev )
			other.prev->next = &other;
		if( other.next )
			other.next->prev = &other;
	}
}; // class OwnerLinked
} // namespace smart_ptr
} // FatRat Library


#endif // frl_smart_ptr_owner_linked_h_
