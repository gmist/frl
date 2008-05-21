#include "poor_xml/frl_poor_xml_document.h"
#include "poor_xml/frl_poor_xml_parser.h"
#include "io/fs/frl_fs_fn.h"
#include "frl_smart_ptr.h"

namespace frl
{
namespace poor_xml
{
Document::Document()
{
}

Document::~Document()
{
}

void Document::LoadFromCurrenttDir( const String& fileName_ )
{
	FRL_EXCEPT_GUARD();
	String currDir = io::fs::getCurrentDirectory();
	io::fs::addSlashToEndPath( currDir );
	fileName = currDir + fileName_;
	if( ! io::fs::isExist( fileName ) )
		FRL_THROW_S_CLASS( Document::FileNotFound );
	io::fs::FileOffset length = io::fs::length( fileName );
	if( length <= 0 )
		FRL_THROW_S_CLASS( Document::EmptyFile );
	SmartPtr< char, smart_ptr::OwnerRefCount, smart_ptr::ArrayStorage > data( new char[ ( size_t )length + 1 ] );
	io::fs::FileDescriptor file = io::fs::InvalidFileDescriptor;
	size_t counts;
	try
	{
		io::fs::open( file, fileName, io::fs::openReadOnly | io::fs::openBinary );
		counts = io::fs::read( file, smart_ptr::GetPtr( data ), (io::fs::FileRWCount)length );
		if( counts == 0 )
			FRL_THROW_S_CLASS( Document::UnknownError );
	}
	catch( ... )
	{
		if( file != io::fs::InvalidFileDescriptor )
			io::fs::close( file );
		FRL_THROW_S_CLASS( Document::UnknownError );
	}
	io::fs::close( file );
	String buffer;
	buffer.reserve( counts );
	for( size_t i = 0; i < counts; ++i )
	{
		if( data[i] == 0xa || data[i] == 0xd )
			continue;
		buffer+= data[i];
	}
	parseHeader( buffer );
	NodesList tmpMap = Parser::getSubNodes( buffer );
	if( tmpMap.size() > 1 )
		FRL_THROW_S_CLASS( Document::BrokenXML ); // must be one root node
	root = (*tmpMap.begin());
}

void Document::parseHeader( String &buffer )
{
	FRL_EXCEPT_GUARD();
	if( buffer.empty() )
		FRL_THROW_S_CLASS( Document::EmptyFile );

	String tmpBuffer = buffer;
	size_t headerBegin = buffer.find( FRL_STR("<?xml") );
	if( headerBegin != String::npos )
	{
		headerBegin += 5;
		size_t headerEnd = buffer.find( FRL_STR("?>") );
		if( headerEnd == String::npos )
			FRL_THROW_S_CLASS( Document::BrokenXML );
		headerEnd -= headerBegin;
		if( headerBegin >= headerEnd )
			FRL_THROW_S_CLASS( Document::BrokenXML );
		String tmp = buffer.substr( headerBegin, headerEnd );
		headerEnd += (headerBegin + 2);
		buffer = buffer.substr( headerEnd, buffer.length() - 1 );
		frl::removeSimbolsFromStart( tmp, FRL_STR(' ') );
		frl::removeSimbolsFromEnd( tmp, FRL_STR(' ') );
		try
		{
			version = Parser::getProperty( tmp, FRL_STR("version") );
		}
		catch( ... )
		{
			FRL_THROW_S_CLASS( Document::BrokenXML );
		}
		try
		{
			encoding = Parser::getProperty( tmp, FRL_STR("encoding") );
		}
		catch( ... )
		{
			return;
		}
	}
}

const String& Document::getVersion()
{
	return version;
}

const String& Document::getEncoding()
{
	return encoding;
}

SmartPtr< Node > Document::getRoot()
{
	return root;
}
} // namespace poor_xml
} // FatRat Library
