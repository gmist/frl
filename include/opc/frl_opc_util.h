#ifndef frl_opc_util_h_
#define frl_opc_util_h_
#include "frl_platform.h"
#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "frl_types.h"
#include "frl_exception.h"

namespace frl
{
	namespace opc
	{
		namespace util
		{
			OPCHANDLE getUniqueServerHandle();	
			const FILETIME& getFileTime();
			String getUniqueName();

			void* allocMemory( size_t size );			

			template< typename Type >
			Type* allocMemory( size_t size )
			{
				if( size == 0 )
					return NULL;
				if( size == 1 )
					return reinterpret_cast< Type* >(CoTaskMemAlloc( sizeof(Type) ) );
				return reinterpret_cast< Type* >(CoTaskMemAlloc( size * sizeof(Type) ) );
			}

			template< typename Type >
			Type* allocMemory()
			{
				return reinterpret_cast< Type* >(CoTaskMemAlloc( sizeof(Type) ) );
			}

			template< typename Type >
			Type* reAllocMemory( size_t oldSize, size_t newSize )
			{
				FRL_EXCEPT_GUARD();
				if( newSize == oldSize == 0 )
					FRL_THROW( FRL_STR("Invalid argument") );
				return reinterpret_cast< Type* >(CoTaskMemRealloc( oldSize * sizeof(Type), newSize * sizeof(Type) ) );
			}

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

			char* duplicateString( const char *str );
			wchar_t* duplicateString( const wchar_t *str );
			char* duplicateString( const std::string &string );
			wchar_t* duplicateString( const std::wstring &string );
			void freeMemory( void *ptr );

			HRESULT getErrorString(  HRESULT dwError, LCID dwLocale, LPWSTR **ppString );
			Bool matchStringPattern( const String &str, const String& pattern, Bool caseSensintive = True );
		} // namespace util
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_util_h_ */
