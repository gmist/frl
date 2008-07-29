#ifndef frl_logging_writers_h_
#define frl_logging_writers_h_
#include <boost/thread/mutex.hpp>
#include "logging/frl_logging_elements.h"
#include "console_std/frl_iostream.h"	
#include "io/fs/frl_fs_fn.h"

namespace frl{ namespace logging{

namespace private_
{
String getStr( const logging::LogElementList &elements, const frl::logging::LogParameter &param );
}

class ILogWriter
{
public:
	virtual ~ILogWriter(){};
	virtual void write( const LogElementList &elements, const LogParameter &param ) = 0;
};

typedef std::list< boost::shared_ptr< frl::logging::ILogWriter > > ListLogWriters;

class ConsoleWriter : public ILogWriter
{
static boost::mutex guard;
public:
	virtual ~ConsoleWriter();
	virtual void write( const LogElementList &elements, const LogParameter &param );
};

class FileWriter : public ILogWriter
{
private:
	io::fs::FileDescriptor desc;
	String fileName;
	boost::mutex writeGuard;
	void closeFile();
	void openFile();
public:
	FileWriter();
	FileWriter( const String &fileName_ );
	FileWriter( const FileWriter &other );
	virtual ~FileWriter();
	virtual void write( const LogElementList&elements, const LogParameter &param );

};

class DebugWindowWriter	:	public ILogWriter
{
public:
	virtual ~DebugWindowWriter();
	virtual void write( const LogElementList &elements, const LogParameter &param );
};

} // namespace logging
} // FatRat Library

#endif // frl_logging_writers_h_
