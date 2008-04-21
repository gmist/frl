#ifndef frl_smart_ptr_owner_copy_h_
#define frl_smart_ptr_owner_copy_h_

namespace frl
{
namespace smart_ptr
{

template< class T >
class OwnerCopy
{
public:
	OwnerCopy() {}
	T* addRef( T &ptr)
	{
		return new T( ptr );
	}
	bool release() { return true; }
	void swap( OwnerCopy &rhv ) {}
};

} // namespace smart_ptr
} // FatRat Library

#endif // frl_smart_ptr_owner_copy_h_
