#ifndef frl_types_h_
#define frl_types_h_

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
	typedef unsigned short		UShort;
	typedef short						Short;
	typedef unsigned int			UInt;
	typedef int							Int;
	typedef float						Float;
	typedef double					Double;

	// Redefinitions variables type of "long"
	#if ( FRL_COMPILER == FRL_COMPILER_MSVC )
		typedef unsigned __int64		ULong;
		typedef __int64					Long;
	#elif( FRL_COMPILER == FRL_COMPILER_MINGW )
		typedef unsigned long int	ULong;
		typedef long int				Long;
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

	#if ( FRL_PLATFORM == FRL_PLATFORM_LINUX )
	typedef unsigned long TimeOut;
	#endif // __linux__

	#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
	typedef DWORD TimeOut;
	#endif // WIN32


	namespace thread
	{
		#if ( FRL_PLATFORM == FRL_PLATFORM_LINUX )
		typedef void* ( *FRL_LPTHREAD_START_ROUTINE ) ( void *thread_param );

		// Переопределяем переменную идентификатор нити
		typedef pthread_t ThreadDescriptor;

		// Define invalid thread descriptor
		const ThreadDescriptor InvalidThreadDescriptor = 0;
		#endif // FRL_PLATFORM_LINUX

		#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
		typedef LPTHREAD_START_ROUTINE FRL_LPTHREAD_START_ROUTINE;

		// Переопределяем переменную идентификатор нити
		typedef HANDLE ThreadDescriptor;

		// Define invalid thread descriptor
		const ThreadDescriptor InvalidThreadDescriptor = INVALID_HANDLE_VALUE;
		# endif // FRL_PLATFORM_WIN32
	} // namespace tread

	namespace io
	{
		// Definitions types variables for works with file system
		namespace fs
		{
			#if ( FRL_PLATFORM ==  FRL_PLATFORM_LINUX )
				typedef frl::Int FileDerscriptor;	// File handle
				typedef frl::Long FileOffset;		// File offset (position)
				typedef size_t FileRWCount; // Number read-write simbols in read-write operations

				const Descriptor InvalidFileDescriptor = -1;	// Invalid file handle
				const Offset InvalidFileOffset = -1;		// Invalid file offset (position)
			#endif // FRL_PLATFORM_LINUX

			#if ( FRL_PLATFORM ==  FRL_PLATFORM_WIN32 )
				typedef HANDLE FileDescriptor;	// File handle
				typedef Long FileOffset;
				typedef DWORD FileRWCount;

				const FileDescriptor InvalidFileDescriptor = INVALID_HANDLE_VALUE;
				const FileOffset InvalidFileOffset = -1;
			#endif // FRL_PLATFORM_WIN32
		}	// namespace fs
	} // namespace io

	namespace time
	{
		typedef frl::Long Time;
	} // namespace time

} // FatRat Library

#endif // frl_types_h_
