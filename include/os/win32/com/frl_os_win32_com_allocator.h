#ifndef frl_os_win32_com_allocator_h_
#define frl_os_win32_com_allocator_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "frl_types.h"
#include "frl_exception.h"

namespace frl{ namespace os{ namespace win32{ namespace com{

class Allocator
{
public:
	void* operator new( size_t size );
	void operator delete( void* ptr );
}; // class Allocator

template< typename Type >
Type* allocMemory( size_t size )
{
	if( size == 0 )
		return NULL;
	return reinterpret_cast< Type* >(CoTaskMemAlloc( size * sizeof(Type) ) );
}

template< typename Type >
Type* allocMemory()
{
	return reinterpret_cast< Type* >(CoTaskMemAlloc( sizeof(Type) ) );
}

void freeMemory( void *ptr );

template< typename Type >
void zeroMemory( Type* ptr, size_t size )
{
	FRL_EXCEPT_GUARD();
	if( size == 0 )
		FRL_THROW( FRL_STR("Invalid argument") );
	::ZeroMemory( ptr, sizeof(Type) * size );
}

template< typename Type >
void zeroMemory( Type* ptr )
{
	::ZeroMemory( ptr, sizeof(Type) );
}

Bool isValidType( VARTYPE vtType );

} // namespace com
} // namespace win32
} // namespace os
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_os_win32_com_allocator_h_
