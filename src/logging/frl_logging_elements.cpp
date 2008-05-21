#include "logging/frl_logging_elements.h"

namespace frl
{
namespace logging
{
LexicalElement::LexicalElement( String msg ) :	text( msg )
{
}

LexicalElement::~LexicalElement()
{

}

frl::String LexicalElement::proccess( const LogParameter& )
{
	return text;
}

DateElement::~DateElement()
{

}

frl::String DateElement::proccess( const LogParameter& )
{
	return sysTime.getDateYYYYMMDD();
}

TimeElement::~TimeElement()
{

}

frl::String TimeElement::proccess( const LogParameter& )
{
	return sysTime.getTimeHHMMSS();
}

LevelElement::~LevelElement()
{

}

frl::String LevelElement::proccess( const LogParameter &param )
{
	switch( param.level )
	{
	case LEVEL_TRACE:
		return FRL_STR("Trace");
	case LEVEL_MSG:
		return FRL_STR("Message");
	case LEVEL_WARNING:
		return FRL_STR("Warning");
	case LEVEL_ERROR:
		return FRL_STR("Error");
	case LEVEL_INFO:
		return FRL_STR("Info");
	default:
		return FRL_STR("");
	}
}

frl::String MsgElement::proccess( const LogParameter &param )
{
	return param.msg;
}

MsgElement::~MsgElement()
{

}
frl::String EndlElement::proccess( const LogParameter& )
{
	return FRL_NEW_LINE;
}

EndlElement::~EndlElement()
{

}

frl::String FileElement::proccess( const LogParameter &param )
{
	return param.file;
}

FileElement::~FileElement()
{

}
frl::String LineElement::proccess( const LogParameter &param )
{
	return lexicalCast< ULong, String >( param.line );
}

LineElement::~LineElement()
{

}

frl::String FunctionElement::proccess( const LogParameter &param )
{
	return param.function;
}

FunctionElement::~FunctionElement()
{

}
frl::String ThreadIDElement::proccess( const LogParameter &param )
{
	return lexicalCast< ULong, String >( param.thread_id );
}

ThreadIDElement::~ThreadIDElement()
{

}
} // namespace logging
} // FatRat Library
