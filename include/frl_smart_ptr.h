#ifndef frl_smart_ptr_h_
#define frl_smart_ptr_h_
#include "smart_ptr/frl_smart_ptr_simply_storage.h"
#include "smart_ptr/frl_smart_ptr_owner_com.h"

namespace frl
{

template
<
	class T,
	template< class > class OwnerShip = smart_ptr::OwnerCOM,
	template< class > class Storage = smart_ptr::SimplyStorage
>
class SmartPtr : public Storage< T >, public OwnerShip< T >
{

template< class B, template<class> class O, template<class> class S >
friend B* GetPtr( const frl::SmartPtr< B, O, S > &ptr );

public:

	typedef Storage< T > SP;
	typedef OwnerShip< T > OP;

	SmartPtr()	: SP(), OP( SP::ptr )
	{
	}

	SmartPtr( T* newObj )
	: SP( newObj ), OP( SP::ptr )
	{

	}

	SmartPtr( const SmartPtr &rhv )
		:	SP( rhv ), OP( rhv )
	{
		SP::ptr = OP::addRef( rhv.ptr );
	}

	~SmartPtr()
	{
		if( OP::release( SP::ptr ) )
			SP::free();
	}

	T& operator * () const
	{
		return *SP::ptr;
	}

	T* operator ->() const
	{
		return SP::ptr;
	}

	SmartPtr& operator = ( const SmartPtr& rhv )
	{
		if( rhv.ptr == SP::ptr )
			return *this;

		SmartPtr tmp( rhv );
		tmp.swap( *this );
		return *this;
	}

	void swap( SmartPtr &rhv )
	{
		SP::swap( rhv );
		OP::swap( rhv );
	}
};

namespace smart_ptr
{
	template< class T, template<class> class O, template<class> class S >
	inline
	T* GetPtr( const frl::SmartPtr< T, O, S > &ptr )
	{
		return GetPtr<T>( *( typename SmartPtr<T, O, S>::SP*)(&ptr) );
	}
}

} // FatRat Library

#endif // frl_smart_ptr_h_
