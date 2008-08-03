#ifndef frl_platform_h_
#define frl_platform_h_

#include "frl_config.h"

namespace frl{

// Define platform type
#define FRL_PLATFORM_WIN32	0x1000
#define FRL_PLATFORM_LINUX	0x2000
#define FRL_PLATFORM_APPLE	0x4000

// Define compiler type
#define FRL_COMPILER_MSVC	0x1000
#define	FRL_COMPILER_GNUC		0x2000
#define FRL_COMPILER_MINGW	0x4000

// Define architecture type
#define FRL_ARCHITECTURE_32 0x1000
#define FRL_ARCHITECTURE_64 0x1000

// Define character type
#define FRL_CHARACTER_MULTIBYTE 0
#define FRL_CHARACTER_UNICODE		1

// Select platform type
#if defined( __WIN32__ ) || defined( _WIN32 ) || defined( WIN32 )
	#define FRL_PLATFORM FRL_PLATFORM_WIN32
#elif defined( __APPLE_CC__ )
	#define FRL_PLATFORM FRL_PLATFORM_APPLE
#else
	#define FRL_PLATFORM FRL_PLATFORM_LINUX
#endif // Select platform type

// Select compiler type
#if defined( _MSC_VER )
	#define FRL_COMPILER FRL_COMPILER_MSVC
	#define FRL_COMP_VER _MSC_VER

#elif defined ( __MINGW32__ )
	#define  FRL_COMPILER FRL_COMPILER_MINGW
	#define FRL_COMP_VER (((__GNUC__)*100) + \
	(__GNUC_MINOR__*10) + \
	__GNUC_PATCHLEVEL__)
#elif defined( __GNUC__ )
	#define FRL_COMPILER FRL_COMPILER_GNUC
	#define FRL_COMP_VER (((__GNUC__)*100) + \
	(__GNUC_MINOR__*10) + \
	__GNUC_PATCHLEVEL__)
#else
		#pragma error "No known compiler!!!"
#endif // Select compiler type

// Select architecture type
#if defined( __x86_64__ )
	#define FRL_ARCHITECTURE FRL_ARCHITECTURE_64
#else
	#define FRL_ARCHITECTURE FRL_ARCHITECTURE_32
#endif // Select architecture type

// Select  character set type
#if defined( UNICODE ) || defined( _UNICODE )
	#define FRL_CHARACTER FRL_CHARACTER_UNICODE
#else
	#define FRL_CHARACTER FRL_CHARACTER_MULTIBYTE
#endif

// Platform settings:

// Platform == Windows
#if FRL_PLATFORM == FRL_PLATFORM_WIN32
	
	#if( FRL_COMPILER == FRL_COMPILER_MINGW )
		#define FRL_MODULE_EXPORT
		#define FRL_MODULE_IMPOR
		#define FRL_MODULE_PRIVATE
	#else			
		#define FRL_MODULE_EXPORT __declspec( dllexport )	
		#define FRL_MODULE_IMPORT __declspec( dllimport )
		#define FRL_MODULE_PRIVATE
	#endif
	
	#ifdef _DEBUG
		#define FRL_DEBUG_MODE 1
	#else
		#define FRL_DEBUG_MODE 0
	#endif

	#if( FRL_COMPILER == FRL_COMPILER_MINGW )
		#define EXT_HASH
	#else
		#if( _MSC_VER < 1500 )
			#define snprintf _snprintf
			#define vsnprintf _vsnprintf
		#endif
	#endif

	#if FRL_DEBUG_MODE
		#define FRL_PLATFORM_LIB "frl_d.dll"
	#else
		#define FRL_PLATFORM_LIB "frl.dll"
	#endif

#endif // Platform == Windows

// Platform == Linux || Apple
#if FRL_PLATFORM == FRL_PLATFORM_LINUX || FRL_PLATFORM == FRL_PLATFORM_APPLE
	// Enable GCC 4.0 symbol visibility 
	#if FRL_COMP_VER >= 400
		#define FRL_MODULE_EXPORT  __attribute__ ((visibility("default")))
		#define FRL_MODULE_IMPORT
		#define FRL_MODULE_PRIVATE __attribute__ ((visibility("hidden")))
	#else
		#define FRL_MODULE_EXPORT
		#define FRL_MODULE_IMPOR
		#define FRL_MODULE_PRIVATE
	#endif
	
	#ifdef DEBUG
		#define FRL_DEBUG_MODE 1
	#else
		#define FRL_DEBUG_MODE 0
	#endif

	#if FRL_PLATFORM == FRL_PLATFORM_APPLE
		#if FRL_DEBUG_MODE
			#define FRL_PLATFORM_LIB "frl_d.bundle"
		#else // if ! FRL_DEBUG_MODE
			#define FRL_PLATFORM_LIB "frl.bundle"
		#endif // FRL_DEBUG_MODE
	#else // if FRL_PLATFORM_LINUX
		#if FRL_DEBUG_MODE
			#define	FRL_PLATFORM_LIB "libfrl_d.so"
		#else // ! FRL_DEBUG_MODE
			#define FRL_PLATFORM_LIB "libfrl.so"
		#endif
	#endif
#endif // Platform == Linux || Apple

// Redefinition __FUNCTION__
#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
	#define FRL_FUNCTION_NAME __FUNCTIONW__
#else
	#define FRL_FUNCTION_NAME __FUNCTION__
#endif

#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
	#define FRL_FILE_NAME __FILEW__
#else
	#define FRL_FILE_NAME __FILE__
#endif


// Definition new line consecution character
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE ) 
		const wchar_t FRL_NEW_LINE[] = L"\r\n";
	#else
		const char FRL_NEW_LINE[] = "\r\n";
	#endif
#else // ! FRL_PLATFORM_WIN32
	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE ) 
		const wchar_t FRL_NEW_LINE[] = L"\n";
	#else
		const char FRL_NEW_LINE[] = "\n";
	#endif
#endif // Definition new line consecution character

} // FatRat Library

#endif // frl_platform_h_
