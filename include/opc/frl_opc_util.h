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
			FILETIME getFileTime();
			String getUniqueName();

			template< typename Type >
			Type* allocMemory( size_t size )
			{
				if( size == 0 )
					FRL_THROW( FRL_STR("Invalid argument") );
				return reinterpret_cast< Type* >(CoTaskMemAlloc( size * sizeof(Type) ) );
			}

			template< typename Type >
			Type* allocMemory()
			{
				return reinterpret_cast< Type* >(CoTaskMemAlloc( sizeof(Type) ) );
			}

			template< typename Type >
			void zeroMemory( Type* ptr, size_t size )
			{
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
		} // namespace util
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_util_h_ */
