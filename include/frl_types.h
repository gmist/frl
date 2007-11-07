#ifndef FRL_TYPES_H_
#define FRL_TYPES_H_

#include "frl_platform.h"
#include <string>
#include <stdio.h>
#include <fstream>

#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
	#include <Windows.h>
#endif // FRL_PLATFORM_WIN32

namespace frl
{
	// Redefinitions types of variables
	typedef unsigned short	UShort;
	typedef short						Short;
	typedef unsigned int			UInt;
	typedef int							Int;
	typedef float						Float;
	typedef double					Double;

	// Redefinitions variables type of "long"
	#if ( FRL_COMPILER == FRL_COMPILER_MSVC )
		typedef unsigned __int64		ULong;
		typedef __int64					Long;
	#elif ( FRL_COMPILER == FRL_COMPILER_GNUC )
		typedef unsigned long long	ULong;
		typedef long long				Long;
	#else
		#pragma error "frl::Long and frl::ULong not defined for this compiler or architecture!"
	#endif

	// Redefinitions variables types of *char*
	#if ( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		typedef wchar_t Char;
		typedef wchar_t SChar;
		typedef wchar_t UChar;
		#define FRL_STR(str) L##str
	#else // ! UNICODE
		typedef char Char;
		typedef signed char SChar;
		typedef unsigned char UChar;
		#define FRL_STR(str) str
	#endif	// UNICODE

	// Redefinition variables type of string
	typedef std::basic_string< frl::Char > String;

	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		typedef std::wfstream StdFile;
		typedef std::wifstream StdInFile;
		typedef std::wofstream StdOutFile;
	#else // ! UNICODE		
		typedef std::fstream StdFile;
		typedef std::ifstream StdInFile;
		typedef std::ofstream StdOutFile;
	#endif

	// Redefinitions variables type of bool
	typedef bool Bool;
	const frl::Bool True = true;
	const frl::Bool False = false;

} // Fat Rat Library

#endif /* FRL_TYPES_H_ */
