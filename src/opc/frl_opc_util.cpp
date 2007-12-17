#include "opc/frl_opc_util.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "frl_lexical_cast.h"
#include "frl_exception.h"

namespace frl
{
	namespace opc
	{
		namespace util
		{
			OPCHANDLE getUniqueServerHandle()
			{
				static OPCHANDLE handle = 0;
				return ++handle;
			}

			FILETIME getFileTime()
			{
				FILETIME fileTime;
				GetSystemTimeAsFileTime( &fileTime );
				return fileTime;
			}

			Bool isValidType( VARTYPE vtType )
			{
				if (vtType & VT_ARRAY)
				{
					if (((vtType & VT_TYPEMASK) | VT_ARRAY) != vtType)
					{
						return False;
					}
				}

				switch (vtType & VT_TYPEMASK)
				{
				case VT_EMPTY:
				case VT_I1:
				case VT_UI1:
				case VT_I2:
				case VT_UI2:
				case VT_I4:
				case VT_UI4:
				case VT_I8:
				case VT_UI8:
				case VT_R4:
				case VT_R8:
				case VT_CY:
				case VT_BOOL:
				case VT_DATE:
				case VT_BSTR:
				case VT_VARIANT:
					{
						return True;
					}
				}
				return False;
			}

			frl::String getUniqueName()
			{
				static unsigned long counter = 0;
				counter++;
				String result = lexicalCast< unsigned long, String >::get( counter );
				result += FRL_STR("_Name");
				return result;
			}

			char* duplicateString( const char *str )
			{
				char* ret = NULL;
				if( str != NULL )
				{
					size_t size = strlen( str ) + 1;
					ret = util::allocMemory< char >( size );
					strcpy_s( ret, size, str );
				}
				return ret;
			}

			wchar_t* duplicateString( const wchar_t *str )
			{
				wchar_t* ret = NULL;
				if( str != NULL )
				{
					size_t size = wcslen( str ) + 1;
					ret = util::allocMemory< wchar_t >( size );
					wcscpy_s( ret, size, str );
				}
				return ret;
			}

			char* duplicateString( const std::string &string )
			{
				char* ret = NULL;
				if( !string.empty() )
				{
					size_t size = string.length() + 1;
					ret = util::allocMemory< char >( size );
					strcpy_s( ret, size, string.c_str() );
				}
				return ret;
			}

			wchar_t* duplicateString( const std::wstring &string )
			{
				wchar_t* ret = NULL;
				if( !string.empty() )
				{
					size_t size = string.length() + 1;
					ret = util::allocMemory< wchar_t >( size );
					wcscpy_s( ret, size, string.c_str() );
				}
				return ret;
			}

			void* allocMemory( size_t size )
			{
				FRL_EXCEPT_GUARD();
				if( size == 0 )
					FRL_THROW( FRL_STR("Invalid argument") );
				return CoTaskMemAlloc( size );
			}

			void freeMemory( void *ptr )
			{
				CoTaskMemFree( ptr );
				ptr = NULL;
			}
		}	// namespace util
	} // namespace opc
} // // namespace FatRat Library


#endif /* FRL_PLATFORM_WIN32 */
