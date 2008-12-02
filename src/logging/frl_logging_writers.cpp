#include <boost/foreach.hpp>
#include "logging/frl_logging_writers.h"

namespace frl{ namespace logging{

namespace private_
{
String getStr( const logging::LogElementList &elements, const frl::logging::LogParameter &param )
{
	String tmp;
	BOOST_FOREACH( const LogElement& el, elements )
	{
		tmp += el->proccess( param );
	}
	return tmp;
}
} // namespace private_

boost::mutex ConsoleWriter::guard;

ConsoleWriter::~ConsoleWriter()
{
}

void ConsoleWriter::write( const LogElementList &elements, const LogParameter &param )
{
	boost::mutex::scoped_lock scopeGuard( guard );
	console_std::Out << private_::getStr( elements, param );
}

void FileWriter::closeFile()
{
	if( desc != io::fs::InvalidFileDescriptor )
		io::fs::close( desc );
}

FileWriter::FileWriter()
	:	desc( io::fs::InvalidFileDescriptor)
{
}

FileWriter::FileWriter( const String &fileName_ )
	:	desc( io::fs::InvalidFileDescriptor ),
		fileName( fileName_ )
{
	if( fileName.empty() )
		return;
	openFile();
}

FileWriter::FileWriter( const FileWriter &other )
	:	ILogWriter( other ),
		desc( io::fs::InvalidFileDescriptor )
{
	if( other.fileName.empty() )
		return;
	fileName = FRL_STR("new");
	openFile();
}

void FileWriter::openFile()
{
	frl::Bool is_specified_dir = frl::False;
	if( fileName.size() >= 2 )
	{
		if( fileName[0] == FRL_STR('\\') || fileName[1] == FRL_STR(':') )
			is_specified_dir = frl::True;
	}

	if( ! is_specified_dir )
	{
		String dir = io::fs::getCurrentDirectory();
		io::fs::addSlashToEndPath( dir );
		fileName = dir + fileName;
	}

	try
	{
		if( ! io::fs::isExist( fileName) )
		{
			io::fs::open( desc, fileName, io::fs::openWriteOnly | io::fs::openCreate );
			#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE  )
			unsigned short signature = 0xFEFF;
			io::fs::write( desc, &signature, 2 );
			#endif
		}
		else
		{
			io::fs::open( desc, fileName, io::fs::openWriteOnly | io::fs::openCreate | io::fs::openAppend );
		}
	}
	catch( frl::Exception& )
	{
		desc = io::fs::InvalidFileDescriptor;
	}
}

FileWriter::~FileWriter()
{
	closeFile();
}

void FileWriter::write( const LogElementList &elements, const LogParameter &param )
{
	if( desc != io::fs::InvalidFileDescriptor )
	{
		boost::mutex::scoped_lock guard( writeGuard );
		String tmp = private_::getStr( elements, param );
		if( tmp.length() )
			io::fs::write( desc, tmp.c_str(), ( io::fs::FileRWCount )( tmp.length() * sizeof(Char) ) );
	}
}

DebugWindowWriter::~DebugWindowWriter()
{
}

void DebugWindowWriter::write( const LogElementList &elements, const LogParameter &param )
{
	String tmp = private_::getStr( elements, param );
	if( tmp.length() )
		::OutputDebugString( tmp.c_str() );
}

} // namespace logging
} // FatRat Library
