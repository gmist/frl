#ifndef frl_smart_ptr_simply_storage_h_
#define frl_smart_ptr_simply_storage_h_

namespace frl
{
namespace smart_ptr
{

template< class T >
class SimplyStorage
{
	template< class B >
	friend B* GetPtr( const SimplyStorage< B > &ptr );

protected:
	T *ptr;
	void free()
	{
		delete ptr;
	}
	
	SimplyStorage()
	{
		ptr = new T;
	}

	SimplyStorage( T* newObj )
	{
		ptr = newObj;
	}

	void swap( SimplyStorage &rhv )
	{
		std::swap( ptr, rhv.ptr );
	}
};

template< class B >
inline
	B* GetPtr( const frl::smart_ptr::SimplyStorage< B > &ptr )
{
	return ptr.ptr;
}

} // namespace smart_ptr
} // FatRat Library

#endif // frl_smart_ptr_simply_storage_h_
