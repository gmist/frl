#include "opc/frl_opc_util.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "frl_lexical_cast.h"
#include "frl_exception.h"
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "os/win32/com/frl_os_win32_com_allocator.h"
#include "stream_std/frl_sstream.h"

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

const FILETIME& getFileTime()
{
	static FILETIME fileTime;
	GetSystemTimeAsFileTime( &fileTime );
	return fileTime;
}

frl::String getUniqueName()
{
	static unsigned long counter = 0;
	++counter;
	stream_std::StringStream ss;
	ss << counter <<  FRL_STR("_Name_FRL_LIB");
	return ss.str();
}

char* duplicateString( const char *str )
{
	char* ret = NULL;
	if( str != NULL )
	{
		size_t size = strlen( str ) + 1;
		ret = os::win32::com::allocMemory< char >( size );
		#if( FRL_COMPILER == FRL_COMPILER_MSVC )
		strcpy_s( ret, size, str );
		#else
		strcpy( ret, str );
		#endif
	}
	return ret;
}

wchar_t* duplicateString( const wchar_t *str )
{
	wchar_t* ret = NULL;
	if( str != NULL )
	{
		size_t size = wcslen( str ) + 2;
		ret = os::win32::com::allocMemory< wchar_t >( size );
		#if( FRL_COMPILER == FRL_COMPILER_MSVC )
		wcscpy_s( ret, size, str );
		#else
		wcscpy( ret, str );
		#endif
		
	}
	return ret;
}

char* duplicateString( const std::string &string )
{
	char* ret = NULL;
	if( !string.empty() )
	{
		size_t size = string.length() + 1;
		ret = os::win32::com::allocMemory< char >( size );
		#if( FRL_COMPILER == FRL_COMPILER_MSVC )
		strcpy_s( ret, size, string.c_str() );
		#else
		strcpy( ret, string.c_str() );
		#endif
	}
	return ret;
}

wchar_t* duplicateString( const std::wstring &string )
{
	wchar_t* ret = NULL;
	if( !string.empty() )
	{
		size_t size = string.length() + 2;
		ret = os::win32::com::allocMemory< wchar_t >( size );
		#if( FRL_COMPILER == FRL_COMPILER_MSVC )
		wcscpy_s( ret, size, string.c_str() );
		#else
		wcscpy( ret, string.c_str() );
		#endif
	}
	return ret;
}

HRESULT getErrorString( HRESULT dwError, LCID lcid, LPWSTR **ppString )
{
	switch ( dwError )
	{
#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
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
#else
		case OPC_E_INVALIDHANDLE:
			**ppString = duplicateString( string2wstring( FRL_STR("The value of the handle is invalid.") ) );
			return S_OK;

		case OPC_E_BADTYPE:
			**ppString = duplicateString( string2wstring( FRL_STR("The server cannot convert the data between the specified \
												  format and/or requested data type and the canonical data type.") ) );
			return S_OK;

		case OPC_E_PUBLIC:
			**ppString = duplicateString( string2wstring( FRL_STR("The requested operation cannot be done on a public group.") ) );
			return S_OK;

		case OPC_E_BADRIGHTS:
			**ppString = duplicateString( string2wstring( FRL_STR("The item's access rights do not allow the operation.") ) );
			return S_OK;

		case OPC_E_UNKNOWNITEMID:
			**ppString = duplicateString( string2wstring( FRL_STR("The item ID is not defined in the server address space \
												  or no longer exists in the server address space.") ) );
			return S_OK;

		case OPC_E_INVALIDITEMID:
			**ppString = duplicateString( string2wstring( FRL_STR("The item ID does not conform to the server's syntax.") ) );
			return S_OK;

		case OPC_E_INVALIDFILTER:
			**ppString = duplicateString( string2wstring( FRL_STR( "The filter string was not valid." ) ) );
			return S_OK;

		case OPC_E_UNKNOWNPATH:
			**ppString = duplicateString( string2wstring( FRL_STR( "The item's access path is not known to the server." ) ) );
			return S_OK;

		case OPC_E_RANGE:
			**ppString = duplicateString( string2wstring( FRL_STR( "The value was out of range." ) ) );
			return S_OK;

		case OPC_E_DUPLICATENAME:
			**ppString = duplicateString( string2wstring( FRL_STR( "Duplicate name not allowed." ) ) );
			return S_OK;

		case OPC_S_UNSUPPORTEDRATE:
			**ppString = duplicateString( string2wstring( FRL_STR( "The server does not support the requested data \
												   rate but will use the closest available rate." ) ) );
			return S_OK;

		case OPC_S_CLAMP:
			**ppString = duplicateString( string2wstring( FRL_STR( "A value passed to write was accepted but the output was clamped." ) ) );
			return S_OK;

		case OPC_S_INUSE:
			**ppString = duplicateString( string2wstring( FRL_STR( "The operation cannot be performed because the object is bering referenced." ) ) );
			return S_OK;

		case OPC_E_INVALIDCONFIGFILE:
			**ppString = duplicateString( string2wstring( FRL_STR( "The server's configuration file is an invalid format." ) ) );
			return S_OK;

		case OPC_E_NOTFOUND:
			**ppString = duplicateString( string2wstring( FRL_STR( "The requested object (e.g. a public group) was not found." ) ) );
			return S_OK;

		case OPC_E_INVALID_PID:
			**ppString = duplicateString( string2wstring( FRL_STR( "The specified property ID is not valid for the item." ) ) );
			return S_OK;

		case OPC_E_DEADBANDNOTSET:
			**ppString = duplicateString( string2wstring( FRL_STR( "The item deadband has not been set for this item." ) ) );
			return S_OK;

		case OPC_E_DEADBANDNOTSUPPORTED:
			**ppString = duplicateString( string2wstring( FRL_STR( "The item does not support deadband." ) ) );
			return S_OK;

		case OPC_E_NOBUFFERING:
			**ppString = duplicateString( string2wstring( FRL_STR( " The server does not support buffering of data items \
												   that are collected at a faster rate than the group update rate." ) ) );
			return S_OK;

		case OPC_E_INVALIDCONTINUATIONPOINT:
			**ppString = duplicateString( string2wstring( FRL_STR( "The continuation point is not valid." ) ) );
			return S_OK;

		case OPC_S_DATAQUEUEOVERFLOW:
			**ppString = duplicateString( string2wstring( FRL_STR( "Not every detected change has been returned since the \
												   server's buffer reached its limit and had to purge out the oldest data." ) ) );
			return S_OK;

		case OPC_E_RATENOTSET:
			**ppString = duplicateString( string2wstring( FRL_STR( "There is no sampling rate set for the specified item." ) ) );
			return S_OK;

		case OPC_E_NOTSUPPORTED:
			**ppString = duplicateString( string2wstring( FRL_STR( "The server does not support writing of quality and/or timestamp." ) ) );
			return S_OK;

		case OPCCPX_E_TYPE_CHANGED:
			**ppString = duplicateString( string2wstring( FRL_STR( "The dictionary and/or type description for the item has changed." ) ) );
			return S_OK;

		case OPCCPX_E_FILTER_DUPLICATE:
			**ppString = duplicateString( string2wstring( FRL_STR( "A data filter item with the specified name already exists." ) ) );
			return S_OK;

		case OPCCPX_E_FILTER_INVALID:
			**ppString = duplicateString( string2wstring( FRL_STR( "The data filter value does not conform to the server's syntax." ) ) );
			return S_OK;

		case OPCCPX_E_FILTER_ERROR:
			**ppString = duplicateString( string2wstring( FRL_STR( "An error occurred when the filter value was applied to the source data." ) ) );
			return S_OK;

		case OPCCPX_S_FILTER_NO_DATA:
			**ppString = duplicateString( string2wstring( FRL_STR( " The item value is empty because the data filter has excluded all fields." ) ) );
			return S_OK;
#endif
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
	if( cMsg.rfind( FRL_STR("\r\n") ) == cMsg.length()-2 )
	{
		cMsg = cMsg.substr(0, cMsg.length() - 2 );
	}
	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
	**ppString = util::duplicateString( cMsg );
	#else
	**ppString = util::duplicateString( string2wstring( cMsg ) );
	#endif
	return S_OK;
}


// Based on "Alarms and Events Custom Interface Standard. Version 1.10. Final Release. OCTOBER 2, 2002"
Bool matchStringPattern( const String &str, const String& pattern, Bool caseSensitive )
{
	if( pattern.empty() )
		return True;
	if( str.empty() )
		return False;

	String tmpStr, tmpPattern;
	if( ! caseSensitive )
	{
		tmpStr = lexicalCast< String, String >( str, lexicalMutators::lower() );
		tmpPattern = lexicalCast< String, String >( pattern, lexicalMutators::lower() );
		if( tmpStr == tmpPattern )
			return True;
	}
	else
	{
		if( str == pattern )
			return True;

		tmpStr = str;
		tmpPattern = pattern;
	}

	Char charSetSimb, stringSimb, l;
	size_t patternPos = 0;
	size_t sourcePos = 0;
	while( sourcePos < tmpStr.length() && patternPos < tmpPattern.length() )
	{
		stringSimb = tmpPattern[patternPos++];
		if( patternPos > tmpPattern.length() )
		{
			return( sourcePos >= tmpStr.length() ); // if end of string true
		}

		switch( stringSimb )
		{
		// match zero or more char.
		case FRL_STR('*'):
			{
				while( sourcePos < tmpStr.length() ) 
				{   
					if( matchStringPattern(	tmpStr.substr( sourcePos++, tmpStr.length() - 1 ),
						pattern.substr( patternPos, tmpPattern.length() - 1 ),
						caseSensitive) )
					{
						return True;
					}
				}
				return matchStringPattern( tmpStr, tmpPattern.substr( patternPos, tmpPattern.length() - 1 ), caseSensitive);
			}
			break;

		// match any one char
		case FRL_STR('?'):
			{
				// if end of source string
				if( sourcePos >= tmpStr.length() ) 
				{
					return False;  
				}
				++sourcePos;
				break;
			}

		// match char set 
		case FRL_STR('['): 
			{
				charSetSimb = tmpStr[sourcePos++];;

				if( sourcePos > tmpStr.length())
					return False; // syntax

				l = FRL_STR('\0'); 

				// match a char if NOT in set []
				if( tmpPattern[patternPos] == FRL_STR('!') ) 
				{
					++patternPos;

					stringSimb = tmpPattern[patternPos++];

					while( patternPos < tmpPattern.length() ) 
					{
						if (stringSimb == FRL_STR(']')) // if end of char set, then 
						{
							break; // no match found 
						}

						if (stringSimb == FRL_STR('-')) 
						{
							// check a range of chars? 
							stringSimb = tmpPattern[patternPos];

							// get high limit of range 
							if (patternPos > tmpPattern.length() || stringSimb == FRL_STR(']') )
							{
								return False; // syntax 
							}

							if( charSetSimb >= l && charSetSimb <= stringSimb) 
							{
								return False; // if in range, return false
							}
						} 

						l = stringSimb;

						if (charSetSimb == stringSimb) // if char matches this element 
						{
							return False; // return false 
						}

						stringSimb = tmpPattern[patternPos++];
					} 
				}

				// match if char is in set []
				else 
				{
					stringSimb = tmpPattern[patternPos++];

					while(patternPos < tmpPattern.length() ) 
					{
						if (stringSimb == FRL_STR(']')) // if end of char set, then no match found 
						{
							return False;
						}

						if (stringSimb == FRL_STR('-')) 
						{   
							// check a range of chars? 
							stringSimb = tmpPattern[patternPos];

							// get high limit of range 
							if (patternPos > tmpPattern.length() || stringSimb == FRL_STR(']') )
							{
								return False; // syntax 
							}

							if (charSetSimb >= l  &&  charSetSimb <= stringSimb) 
							{
								break; // if in range, move on 
							}
						} 

						l = stringSimb;

						if (charSetSimb == stringSimb) // if char matches this element move on 
						{
							break;           
						}

						stringSimb = tmpPattern[patternPos++];
					} 

					while (patternPos < tmpPattern.length() && stringSimb != FRL_STR(']') ) // got a match in char set skip to end of set
					{
						stringSimb = tmpPattern[patternPos++];             
					}
				}
			}
			break; 

		// match digit.
		case FRL_STR('#'):
			{
				charSetSimb = tmpStr[sourcePos++]; 

				if( charSetSimb < 48 || charSetSimb > 57 )
				{
					return False; // not a digit
				}
			}
			break;

		// match exact char.
		default: 
			{
				charSetSimb = tmpStr[sourcePos++]; 

				if (charSetSimb != stringSimb) // check for exact char
				{
					return False; // not a match
				}

				// check if end of pattern and still string data left.
				if (patternPos >= tmpPattern.length() && sourcePos < tmpStr.length()-1)
				{
					return False;
				}
			}
			break;

		}
	}
	return True;
}

struct daOPCProp
{
	DWORD id;
	VARTYPE type;
	const wchar_t* desc;
};

daOPCProp propertySelectTable[] = 
{
	{ OPC_PROPERTY_DATATYPE, VT_I2, OPC_PROPERTY_DESC_DATATYPE },
	{ OPC_PROPERTY_VALUE, VT_VARIANT, OPC_PROPERTY_DESC_VALUE },
	{ OPC_PROPERTY_QUALITY, VT_I2, OPC_PROPERTY_DESC_QUALITY },
	{ OPC_PROPERTY_TIMESTAMP, VT_DATE, OPC_PROPERTY_DESC_TIMESTAMP },
	{ OPC_PROPERTY_ACCESS_RIGHTS, VT_I4, OPC_PROPERTY_DESC_ACCESS_RIGHTS },
	{ OPC_PROPERTY_SCAN_RATE, VT_R4, OPC_PROPERTY_DESC_SCAN_RATE },
	{ OPC_PROPERTY_EU_TYPE, VT_I4, OPC_PROPERTY_DESC_EU_TYPE },
	{ OPC_PROPERTY_EU_INFO, VT_BSTR | VT_ARRAY, OPC_PROPERTY_DESC_EU_INFO },
	{ OPC_PROPERTY_EU_UNITS, VT_BSTR, OPC_PROPERTY_DESC_EU_UNITS },
	{ OPC_PROPERTY_DESCRIPTION, VT_BSTR, OPC_PROPERTY_DESC_DESCRIPTION },
	{ OPC_PROPERTY_HIGH_EU, VT_R8, OPC_PROPERTY_DESC_HIGH_EU },
	{ OPC_PROPERTY_LOW_EU, VT_R8, OPC_PROPERTY_DESC_LOW_EU },
	{ OPC_PROPERTY_HIGH_IR, VT_R8, OPC_PROPERTY_DESC_HIGH_IR },
	{ OPC_PROPERTY_LOW_IR, VT_R8, OPC_PROPERTY_DESC_LOW_IR },
	{ OPC_PROPERTY_CLOSE_LABEL, VT_BSTR, OPC_PROPERTY_DESC_CLOSE_LABEL },
	{ OPC_PROPERTY_OPEN_LABEL, VT_BSTR, OPC_PROPERTY_DESC_OPEN_LABEL },
	{ OPC_PROPERTY_TIMEZONE, VT_I4, OPC_PROPERTY_DESC_TIMEZONE },
	{ OPC_PROPERTY_CONDITION_STATUS, VT_BSTR, OPC_PROPERTY_DESC_CONDITION_STATUS },
	{ OPC_PROPERTY_ALARM_QUICK_HELP, VT_BSTR, OPC_PROPERTY_DESC_ALARM_QUICK_HELP },
	{ OPC_PROPERTY_ALARM_AREA_LIST, VT_BSTR | VT_ARRAY, OPC_PROPERTY_DESC_ALARM_AREA_LIST },
	{ OPC_PROPERTY_PRIMARY_ALARM_AREA, VT_BSTR, OPC_PROPERTY_DESC_PRIMARY_ALARM_AREA },
	{ OPC_PROPERTY_CONDITION_LOGIC, VT_BSTR, OPC_PROPERTY_DESC_CONDITION_LOGIC },
	{ OPC_PROPERTY_LIMIT_EXCEEDED, VT_BSTR, OPC_PROPERTY_DESC_LIMIT_EXCEEDED },
	{ OPC_PROPERTY_DEADBAND, VT_R8, OPC_PROPERTY_DESC_DEADBAND },
	{ OPC_PROPERTY_HIHI_LIMIT, VT_R8, OPC_PROPERTY_DESC_HIHI_LIMIT },
	{ OPC_PROPERTY_HI_LIMIT, VT_R8, OPC_PROPERTY_DESC_HI_LIMIT },
	{ OPC_PROPERTY_LO_LIMIT, VT_R8, OPC_PROPERTY_DESC_LO_LIMIT },
	{ OPC_PROPERTY_LOLO_LIMIT, VT_R8, OPC_PROPERTY_DESC_LOLO_LIMIT },
	{ OPC_PROPERTY_CHANGE_RATE_LIMIT, VT_R8, OPC_PROPERTY_DESC_CHANGE_RATE_LIMIT },
	{ OPC_PROPERTY_DEVIATION_LIMIT, VT_R8, OPC_PROPERTY_DESC_DEVIATION_LIMIT },
	{ OPC_PROPERTY_SOUND_FILE, VT_BSTR, OPC_PROPERTY_DESC_SOUND_FILE },
	{ OPC_PROPERTY_TYPE_SYSTEM_ID, VT_BSTR, OPC_PROPERTY_DESC_TYPE_SYSTEM_ID },
	{ OPC_PROPERTY_DICTIONARY_ID, VT_BSTR, OPC_PROPERTY_DESC_DICTIONARY_ID },
	{ OPC_PROPERTY_DICTIONARY, VT_VARIANT, OPC_PROPERTY_DESC_DICTIONARY },
	{ OPC_PROPERTY_TYPE_ID, VT_BSTR, OPC_PROPERTY_DESC_TYPE_ID },
	{ OPC_PROPERTY_TYPE_DESCRIPTION, VT_VARIANT, OPC_PROPERTY_DESC_TYPE_DESCRIPTION },
	{ OPC_PROPERTY_CONSISTENCY_WINDOW, VT_BSTR, OPC_PROPERTY_DESC_CONSISTENCY_WINDOW },
	{ OPC_PROPERTY_WRITE_BEHAVIOR, VT_BSTR, OPC_PROPERTY_DESC_WRITE_BEHAVIOR },
	{ OPC_PROPERTY_UNCONVERTED_ITEM_ID, VT_BSTR, OPC_PROPERTY_DESC_UNCONVERTED_ITEM_ID },
	{ OPC_PROPERTY_UNFILTERED_ITEM_ID, VT_BSTR, OPC_PROPERTY_DESC_UNFILTERED_ITEM_ID },
	{ OPC_PROPERTY_DATA_FILTER_VALUE, VT_BSTR, OPC_PROPERTY_DESC_DATA_FILTER_VALUE },
	{ 0, VT_EMPTY, NULL }
};

const wchar_t* getPropertyDesc( DWORD propID )
{
	for( DWORD i = 0; propertySelectTable[i].id != 0; ++i )
	{
		if( propertySelectTable[i].id == propID )
			return propertySelectTable[i].desc;
	}
	return NULL;
}

VARTYPE getPropertyType( DWORD propID )
{
	for( DWORD i = 0; propertySelectTable[i].id != 0; ++i )
	{
		if( propertySelectTable[i].id == propID )
			return propertySelectTable[i].type;
	}
	return VT_EMPTY;
}

}	// namespace util
} // namespace opc
} // FatRat Library


#endif // FRL_PLATFORM_WIN32
