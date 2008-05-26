#include "frl_string.h"
#include <algorithm>

namespace frl
{
// Return string length
#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
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
	return string.length();
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

std::wstring unicodeCompatibility( const std::wstring& str )
{		
	return str;
}

std::wstring unicodeCompatibility( const std::string& str )
{
	return string2wstring( str );
}

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
		if ( string[i] == simbol )
			++i;
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
		if ( string[i] == simbol )
			--i;
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
	for( size_t i =0; i < string.length(); ++i )
	{
		if( string[ i ] != simbol )
			tmp += string[ i ];
	}
	string = tmp;
	return True;
}

size_t getCountsSimbol( const String &string, Char simbol, size_t fromPos_ /*= 0*/, size_t toPos_ /*= 0 */ )
{
	if( toPos_ >= string.length() )
		return String::npos;
	if( fromPos_ > toPos_ )
		return String::npos;
	if( toPos_ != 0 && toPos_ == fromPos_ )
		return String::npos;
	size_t counts = 0;
	if( ( toPos_ == fromPos_ ) && ( fromPos_ == 0 ) )
	{
		for( size_t i= 0; i < string.length(); ++i )
		{
			if( string[i] == simbol )
				++counts;
		}
	}
	else
	{
		for( size_t i = fromPos_; i < toPos_; ++i )
		{
			if( string[i] == simbol )
				++counts;
		}
	}
	return counts;
}
} // FatRat Library
