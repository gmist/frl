#ifndef frl_logging_h_
#define frl_logging_h_
#include "logging/frl_logging_writers.h"
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>

namespace frl { namespace logging {

class Logger : private boost::noncopyable
{
private:
	LogElementList formaters;
	ListLogWriters destionations;
	String name;
	Level level;
public:
	Logger();
	Logger( const String& name_ );
	~Logger();

	template< typename T >
	void addDestination( const T& )
	{
		destionations.push_back( boost::make_shared<T>() );
	}

	void addDestination( const FileWriter& );

	void clearOutFormat();
	void clearWriters();
	void setFormat( const LogElementList &list_ );
	void log( const LogParameter &param);
	void setName( const String& newName );
	void setLevel( frl::logging::Level newLevel );
	Level getLevel();
};
} // namespace logging

namespace private_ {
struct LogProxy
{
	stream_std::OutString ss;
	frl::logging::Logger &dest;
	frl::logging::LogParameter param;

	LogProxy( frl::logging::Logger &log, frl::logging::LogParameter &param_ );
	LogProxy( const LogProxy &other );
	~LogProxy();

	template< typename T >
	stream_std::OutString& operator << ( const T &rhv )
	{
		ss << rhv;
		return ss;
	}

}; // struct LogProxy

LogProxy getLogProxy(	frl::logging::Logger &log,
									frl::logging::Level level,
									frl::ULong thread_id,
									const frl::String &file,
									const frl::String &function,
									frl::ULong line );

} // namespace private_

#define FRL_LOG_TRACE( xXxLog ) \
	frl::private_::getLogProxy( xXxLog, \
	frl::logging::LEVEL_TRACE, \
	1,\
	FRL_FILE_NAME,\
	FRL_FUNCTION_NAME, \
	__LINE__ )

#define FRL_LOG_MSG( xXxLog ) \
	frl::private_::getLogProxy( xXxLog, \
	frl::logging::LEVEL_MSG, \
	1,\
	FRL_FILE_NAME,\
	FRL_FUNCTION_NAME, \
	__LINE__ )


#define FRL_LOG_WARN( xXxLog ) \
	frl::private_::getLogProxy( xXxLog, \
	frl::logging::LEVEL_WARNING, \
	1,\
	FRL_FILE_NAME,\
	FRL_FUNCTION_NAME, \
	__LINE__ )

#define FRL_LOG_ERROR( xXxLog ) \
	frl::private_::getLogProxy( xXxLog, \
	frl::logging::LEVEL_ERROR, \
	1,\
	FRL_FILE_NAME,\
	FRL_FUNCTION_NAME, \
	__LINE__ )


#define FRL_LOG_INFO( xXxLog ) \
	frl::private_::getLogProxy( xXxLog, \
	frl::logging::LEVEL_INFO, \
	1,\
	FRL_FILE_NAME,\
	FRL_FUNCTION_NAME, \
	__LINE__ )

} // FatRat Library

#endif // frl_logging_h_
