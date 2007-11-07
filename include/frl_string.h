#ifndef FRL_STRING_H_
#define FRL_STRING_H_

#include "frl_types.h"
#include <algorithm>
#include <sstream>
#include <stdlib.h>

namespace frl
{

	// Return string length
	size_t stringLength(const frl::Char *str );
	size_t stringLength( const frl::String &string );

	// Converting string to wstring
	std::wstring string2wstring( const std::string &str );

	// Converting wstring to string
	std::string wstring2string( const std::wstring &wstr );

	// Return unicode independ string
	#if ( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		std::wstring unicodeIndependString( std::string str );
		std::wstring unicodeIndependString( std::wstring str );
	#else
		std::string unicodeIndependString( std::wstring str );
		std::string unicodeIndependString( std::string str );
	#endif

	// Always return multibyte string
		std::string multiByteCompatibility( const std::string &str );
		std::string multiByteCompatibility( const std::wstring &str );

	// Convert number to string
	template < class T >
	String decNumber2String( T number )
	{
		#if ( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			std::wstringstream ss;
		#else
			std::stringstream ss;
		#endif
		ss << std::dec << number;
		return ss.str();
	}
	
	template< class T >
	String octNumber2String( T number )
	{
		#if ( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			std::wstringstream ss;
		#else
			std::stringstream ss;
		#endif
		ss << std::oct << number;
		return ss.str();
	}

	template< class T >
	String hexNumber2String( T number )
	{
		#if ( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
				std::wstringstream ss;
		#else
				std::stringstream ss;
		#endif
		ss << std::hex << number;
		return ss.str();
	}

	Bool removeSimbolsFromStart( String &string, Char simbol );	
	Bool removeSimbolsFromEnd( String &string, Char simbol );
	Bool removeSimbols( String &string, Char simbol );
	
} // FatRat Library

#endif /* FRL_STRING_H_ */
