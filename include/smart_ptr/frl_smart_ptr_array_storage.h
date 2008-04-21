#ifndef frl_smart_ptr_array_storage_h_
#define frl_smart_ptr_array_storage_h_

namespace frl
{
namespace smart_ptr
{

template< class T >
class ArrayStorage
{
	template< class B >
	friend B* GetPtr( const ArrayStorage< B > &ptr );

protected:
	T *ptr;
	void free()
	{
		delete [] ptr;
	}
public:
	ArrayStorage( T* newObj )
	{
		ptr = newObj;
	}

	void swap( ArrayStorage &rhv )
	{
		std::swap( ptr, rhv.ptr );
	}
};

template< class B >
inline
B* GetPtr( const frl::smart_ptr::ArrayStorage< B > &ptr )
{
	return ptr.ptr;
}

} // namespace smart_ptr
} // FatRat Library

#endif // frl_smart_ptr_array_storage_h_
