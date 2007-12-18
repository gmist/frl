#include "opc/frl_opc_util.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "frl_lexical_cast.h"
#include "frl_exception.h"
#include "../dependency/vendors/opc_foundation/opcerror.h"

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

			HRESULT getErrorString( HRESULT dwError, LCID lcid, LPWSTR **ppString )
			{
				switch ( dwError )
				{
					case OPC_E_INVALIDHANDLE:
						**ppString = duplicateString( FRL_STR("The value of the handle is invalid.") );
						return S_OK;

					case OPC_E_BADTYPE:
						**ppString = duplicateString( FRL_STR("The server cannot convert the data between the specified \
																					format and/or requested data type and the canonical data type.") );
						return S_OK;

					case OPC_E_PUBLIC:
						**ppString = duplicateString( FRL_STR("The requested operation cannot be done on a public group.") );
						return S_OK;

					case OPC_E_BADRIGHTS:
						**ppString = duplicateString( FRL_STR("The item's access rights do not allow the operation.") );
						return S_OK;

					case OPC_E_UNKNOWNITEMID:
						**ppString = duplicateString( FRL_STR("The item ID is not defined in the server address space \
																					or no longer exists in the server address space.") );
						return S_OK;

					case OPC_E_INVALIDITEMID:
						**ppString = duplicateString( FRL_STR("The item ID does not conform to the server's syntax.") );
						return S_OK;

					case OPC_E_INVALIDFILTER:
						**ppString = duplicateString( FRL_STR( "The filter string was not valid." ) );
						return S_OK;

					case OPC_E_UNKNOWNPATH:
						**ppString = duplicateString( FRL_STR( "The item's access path is not known to the server." ) );
						return S_OK;

					case OPC_E_RANGE:
						**ppString = duplicateString( FRL_STR( "The value was out of range." ) );
						return S_OK;

					case OPC_E_DUPLICATENAME:
						**ppString = duplicateString( FRL_STR( "Duplicate name not allowed." ) );
						return S_OK;

					case OPC_S_UNSUPPORTEDRATE:
						**ppString = duplicateString( FRL_STR( "The server does not support the requested data \
																					rate but will use the closest available rate." ) );
						return S_OK;

					case OPC_S_CLAMP:
						**ppString = duplicateString( FRL_STR( "A value passed to write was accepted but the output was clamped." ) );
						return S_OK;

					case OPC_S_INUSE:
						**ppString = duplicateString( FRL_STR( "The operation cannot be performed because the object is bering referenced." ) );
						return S_OK;

					case OPC_E_INVALIDCONFIGFILE:
						**ppString = duplicateString( FRL_STR( "The server's configuration file is an invalid format." ) );
						return S_OK;

					case OPC_E_NOTFOUND:
						**ppString = duplicateString( FRL_STR( "The requested object (e.g. a public group) was not found." ) );
						return S_OK;

					case OPC_E_INVALID_PID:
						**ppString = duplicateString( FRL_STR( "The specified property ID is not valid for the item." ) );
						return S_OK;

					case OPC_E_DEADBANDNOTSET:
						**ppString = duplicateString( FRL_STR( "The item deadband has not been set for this item." ) );
						return S_OK;

					case OPC_E_DEADBANDNOTSUPPORTED:
						**ppString = duplicateString( FRL_STR( "The item does not support deadband." ) );
						return S_OK;

					case OPC_E_NOBUFFERING:
						**ppString = duplicateString( FRL_STR( " The server does not support buffering of data items \
																					that are collected at a faster rate than the group update rate." ) );
						return S_OK;

					case OPC_E_INVALIDCONTINUATIONPOINT:
						**ppString = duplicateString( FRL_STR( "The continuation point is not valid." ) );
						return S_OK;

					case OPC_S_DATAQUEUEOVERFLOW:
						**ppString = duplicateString( FRL_STR( "Not every detected change has been returned since the \
																					server's buffer reached its limit and had to purge out the oldest data." ) );
						return S_OK;

					case OPC_E_RATENOTSET:
						**ppString = duplicateString( FRL_STR( "There is no sampling rate set for the specified item." ) );
						return S_OK;

					case OPC_E_NOTSUPPORTED:
						**ppString = duplicateString( FRL_STR( "The server does not support writing of quality and/or timestamp." ) );
						return S_OK;

					case OPCCPX_E_TYPE_CHANGED:
						**ppString = duplicateString( FRL_STR( "The dictionary and/or type description for the item has changed." ) );
						return S_OK;

					case OPCCPX_E_FILTER_DUPLICATE:
						**ppString = duplicateString( FRL_STR( "A data filter item with the specified name already exists." ) );
						return S_OK;

					case OPCCPX_E_FILTER_INVALID:
						**ppString = duplicateString( FRL_STR( "The data filter value does not conform to the server's syntax." ) );
						return S_OK;

					case OPCCPX_E_FILTER_ERROR:
						**ppString = duplicateString( FRL_STR( "An error occurred when the filter value was applied to the source data." ) );
						return S_OK;

					case OPCCPX_S_FILTER_NO_DATA:
						**ppString = duplicateString( FRL_STR( " The item value is empty because the data filter has excluded all fields." ) );
						return S_OK;
				}

				LANGID langID = LANGIDFROMLCID(lcid);
				switch( lcid )
				{
				case LOCALE_SYSTEM_DEFAULT:
					{
						langID = GetSystemDefaultLangID();
						break;
					}

				case LOCALE_USER_DEFAULT:
					{
						langID = GetUserDefaultLangID();
						break;
					}

				case LOCALE_INVARIANT:
					{
						langID = LANGIDFROMLCID(LOCALE_NEUTRAL);
						break;
					}
				}

				String cMsg = sys::util::getCodeErrorDescription( langID, dwError );
				if( cMsg.empty() )
				{
					if( lcid == LOCALE_SYSTEM_DEFAULT || lcid == LOCALE_USER_DEFAULT )
					{
						cMsg = sys::util::getCodeErrorDescription( LANGIDFROMLCID(lcid), dwError );
						if( cMsg.empty() )
						{
							cMsg = sys::util::getCodeErrorDescription( LANGIDFROMLCID(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT)), dwError );
							if( cMsg.empty() )
								return E_INVALIDARG;
						}
					}
					else
					{
						if (PRIMARYLANGID(langID) == LANG_ENGLISH)
						{	
							cMsg = sys::util::getCodeErrorDescription( LANGIDFROMLCID(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT)), dwError );
							if( cMsg.empty() )
								return E_INVALIDARG;
						}
						else
							return E_INVALIDARG;
					}
				}

				**ppString = util::duplicateString( cMsg );
				return S_OK;
			}
		}	// namespace util
	} // namespace opc
} // // namespace FatRat Library


#endif /* FRL_PLATFORM_WIN32 */
