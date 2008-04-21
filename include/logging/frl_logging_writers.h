#ifndef frl_logging_writers_h_
#define frl_logging_writers_h_
#include "logging/frl_logging_elements.h"
#include "console_std/frl_iostream.h"	
#include "frl_lock.h"
#include "io/fs/frl_fs_fn.h"
#include "frl_smart_ptr.h"

namespace frl
{
namespace logging
{

namespace private_
{
String getStr( const logging::ListLogElements &elements, const frl::logging::LogParameter &param );
}

class ILogWriter
{
public:
	virtual ~ILogWriter(){};
	virtual void write( const ListLogElements &elements, const LogParameter &param ) = 0;
};

typedef std::list< frl::SmartPtr< frl::logging::ILogWriter > > ListLogWriters;

class ConsoleWriter : public ILogWriter
{
static lock::SemaphoreMutex guard;
public:
	virtual ~ConsoleWriter();
	virtual void write( const ListLogElements &elements, const LogParameter &param );
};

class FileWriter : public ILogWriter
{
private:
	io::fs::FileDescriptor desc;
	String fileName;
	lock::Mutex writeGuard;
	void closeFile();
	void openFile();
public:
	FileWriter();
	FileWriter( const String &fileName_ );
	FileWriter( const FileWriter &other );
	virtual ~FileWriter();
	virtual void write( const ListLogElements&elements, const LogParameter &param );

};

class DebugWindowWriter	:	public ILogWriter
{
public:
	virtual ~DebugWindowWriter();
	virtual void write( const ListLogElements &elements, const LogParameter &param );
};

} // namespace logging
} // FatRat Library

#endif // frl_logging_writers_h_
