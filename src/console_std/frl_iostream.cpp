#include "console_std/frl_iostream.h"

namespace frl{ namespace console_std{

ColorOut::ColorOut()
{
	FRL_EXCEPT_GUARD();
	GetDefaultAttributes();
}

ColorOut::ColorOut( frl::Long foreground, frl::Long backgraund )
{
	FRL_EXCEPT_GUARD();
	GetDefaultAttributes();
	SetAttributes( foreground | backgraund );
}

ColorOut::~ColorOut()
{
	FRL_EXCEPT_GUARD();
	SetAttributes( defaultAttributes );
}

void ColorOut::GetDefaultAttributes()
{
	FRL_EXCEPT_GUARD();
	#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
		CONSOLE_SCREEN_BUFFER_INFO bi;
		if ( ! GetConsoleScreenBufferInfo( GetStdHandle(STD_OUTPUT_HANDLE), &bi ) )
			FRL_THROW_SYSAPI( FRL_STR("Error on get console screen buffer information.") );
		defaultAttributes = bi.wAttributes;
	#endif
}

void ColorOut::SetAttributes( frl::Long attrib )
{
	FRL_EXCEPT_GUARD();
	#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
		if( ! SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)( attrib ) ) )
			FRL_THROW_SYSAPI( FRL_STR("Error on set console screen buffer information.") );
	#endif
}

} // namespace console_std
} // FatRat Library
