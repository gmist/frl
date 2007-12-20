#include "frl_string.h"
#include <algorithm>

namespace frl
{
	// Return string length
	#if ( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		size_t stringLength(const frl::Char *string )
		{
			if( string == NULL )
				return 0;
			return wcslen( string );
		}
	#else // ! UNICODE	
		size_t stringLength( const frl::Char *string)
		{
			if( string == NULL )
				return 0;
			return strlen( string );
		}
	#endif

	// Return string length
	size_t stringLength( const frl::String &string )
	{
		return static_cast<size_t> ( string.length() );
	}

	// Converting string to wstring
	std::wstring string2wstring( const std::string &str )
	{
		std::wstring wstr;
		wstr.resize( str.size() );
		transform( str.begin(), str.end(), wstr.begin(), btowc );
		return wstr;
	}

	// Converting wstring to string
	std::string wstring2string( const std::wstring &wstr )
	{
		std::string str;
		str.resize( wstr.size() );
		transform( wstr.begin(), wstr.end(), str.begin(), wctob );
		return str;
	}

	// Return unicode independ string
	#if ( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		std::wstring unicodeIndependString( std::string str )
		{
			return string2wstring( str );
		}
		std::wstring unicodeIndependString( std::wstring str )
		{
			return str;
		}
	#else // ! UNICODE
		std::string unicodeIndependString( std::wstring str )
		{
			return wstring2string( str );
		}
		std::string unicodeIndependString( std::string str )
		{
			return str;
		}
	#endif // Return unicode independ string

	// Always return multibyte string
	std::string multiByteCompatibility( const std::string &str )
	{
		return str;		
	}

	std::string multiByteCompatibility( const std::wstring &str )
	{
		return wstring2string( str );
	}

	Bool removeSimbolsFromStart( String &string, Char simbol)
	{
		if ( string.empty() )
			return False;

		size_t i = 0;
		while ( i < string.length() )
		{
			if ( string[i] == L' ' )
				i++;
			else
				break;
		}
		string = string.substr( i, string.length() - i );
		return True;
	}

	Bool removeSimbolsFromEnd( String &string, Char simbol )
	{
		if ( string.empty() )
			return False;

		size_t i = string.length()-1;
		while ( i > 0 )
		{
			if ( string[i] == L' ' )
				i--;
			else
				break;
		}
		string = string.substr( 0, i + 1 );
		return True;
	}

	Bool removeSimbols( String &string, Char simbol )
	{
		if ( string.empty() )
			return False;
		String tmp;
		for( size_t i =0; i < string.length(); i++ )
		{
			if( string[ i ] != simbol )
				tmp += string[ i ];
		}
		string = tmp;
		return True;
	}

	String toUpper( const String &str, const std::locale &loc ) 
	{ 
		String result;
		result.resize( str.length() );
		std::transform(str.begin(), str.end(), result.begin(), private_::ToUpperF( loc ) ); 
		return result; 
	} 

	String toLower( const String &str, const std::locale &loc ) 
	{ 
		String result;
		result.resize( str.length() );
		std::transform(str.begin(), str.end(), result.begin(), private_::ToLowerF( loc ) ); 
		return result; 
	}

} // Fat Rat Library
