#ifndef frl_iostream_h_console_std_
#define frl_iostream_h_console_std_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>
#include "frl_platform.h"
#include "frl_types.h"
#include "frl_exception.h"
#include "stream_std/frl_ostream.h"
#include "stream_std/frl_sstream.h"

#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#endif

namespace frl{ namespace console_std{

// Переопределяем переменные
#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
	static std::wostream& Out = std::wcout;
	static std::wistream& In = std::wcin;
	static std::wostream& Err = std::wcerr;
	static std::wostream& Log = std::wclog;
#else
	static std::ostream& Out = std::cout;
	static std::istream& In = std::cin;
	static std::ostream& Err = std::cerr;
	static std::ostream& Log = std::clog;
#endif
		
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
	namespace backgroundColor
	{
		const static DWORD blue = BACKGROUND_BLUE;
		const static DWORD green = BACKGROUND_GREEN;
		const static DWORD red = BACKGROUND_RED;
		const static DWORD intensified = BACKGROUND_INTENSITY;
		const static DWORD onDefault = blue & green & red;
	}

	namespace foregroundColor
	{
		const static DWORD blue = FOREGROUND_BLUE;
		const static DWORD green = FOREGROUND_GREEN;
		const static DWORD red = FOREGROUND_RED;
		const static DWORD intensified = FOREGROUND_INTENSITY;
		const static DWORD onDefault = blue | green | red;
		const static DWORD iBlue = blue | intensified;
		const static DWORD iRed = red | intensified;
		const static DWORD iGreen = green | intensified;
		const static DWORD iDefault = onDefault | intensified;
	}
#endif

class ColorOut : private boost::noncopyable
{
private:
	frl::Long defaultAttributes;
	void GetDefaultAttributes();
	void SetAttributes( frl::Long attrib );
public:
	// Constructors
	ColorOut();
	ColorOut( frl::Long foreground, frl::Long backgraund = backgroundColor::onDefault );
	// Destructor
	~ColorOut();
	// override operator <<
	template < typename T >
	frl::stream_std::Out&
	operator << ( const T& val )
	{
		console_std::Out << val;
		return console_std::Out;
	}
};	// class ColorOut

} // namespace console_std
} // FatRat Library

#endif // frl_iostream_h_console_std_
