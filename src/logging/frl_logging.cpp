#include "logging/frl_logging.h"

namespace frl
{
namespace logging
{

Logger::Logger()
	:	level( frl::logging::LEVEL_TRACE )
{
}

Logger::Logger( const String& name_ )
	:	name( name_ ), level( frl::logging::LEVEL_TRACE )
{
}

Logger::~Logger()
{
	clearOutFormat();
	clearWriters();
}

void Logger::clearOutFormat()
{
	formaters.clear();
}

void Logger::clearWriters()
{
	destionations.clear();
}

void Logger::setFormat( const ListLogElements &list_ )
{
	clearOutFormat();
	formaters = list_;
}

void Logger::log( const LogParameter &param )
{
	if( level <= param.level )
	{
		for( ListLogWriters::iterator it = destionations.begin(); it != destionations.end(); ++it )
			(*it)->write( formaters, param );
	}
}

void Logger::setName( const String& newName )
{
	name = newName;
}

void Logger::setLevel( frl::logging::Level newLevel )
{
	level = newLevel;
}

frl::logging::Level Logger::getLevel()
{
	return level;
}

void Logger::addDestination( const FileWriter& )
{
	if( name.empty() )
		return;
	SmartPtr< FileWriter > ptr( new FileWriter( name + FRL_STR(".log") ) );
	destionations.push_back( (const SmartPtr< ILogWriter >&) ptr );
}

} // namespace logging

namespace private_ {

LogProxy::LogProxy( frl::logging::Logger &log, frl::logging::LogParameter &param_ )
	:	dest( log ), param( param_ )
{
}

LogProxy::LogProxy( const LogProxy &other ) : dest( other.dest ), param( other.param )
{
	ss << other.ss;
}

LogProxy::~LogProxy()
{
	param.msg = ss.str();
	dest.log( param );
}


LogProxy getLogProxy(	frl::logging::Logger &log,
									frl::logging::Level level,
									frl::ULong thread_id,
									const frl::String &file,
									const frl::String &function,
									frl::ULong line )
{
	frl::logging::LogParameter tmp;
	tmp.level = level;
	tmp.thread_id = thread_id;
	tmp.function = function;
	tmp.file = file;
	tmp.line = line;
	return LogProxy( log, tmp );
}

} // namespace private_
} // FatRat Library
