#include <boost/filesystem.hpp>
#include "io/fs/frl_fs_fn.h"
#include "stream_std/frl_fstream.h"
#include "poor_xml/frl_poor_xml_document.h"
#include "poor_xml/frl_poor_xml_parser.h"

namespace fs = boost::filesystem;

namespace frl
{
namespace poor_xml
{

namespace private_
{
bool isCRLF( const Char el )
{
	if( el == 0x0d || el == 0x0a || ! el )
			return true;
		return false;
}
} // namespace private_

Document::Document()
{
}

Document::~Document()
{
}

void Document::loadFromCurrenttDir( const String& fileName_ )
{
	FRL_EXCEPT_GUARD();
	String currDir = io::fs::getCurrentDirectory();
	io::fs::addSlashToEndPath( currDir );
	fileName = currDir + fileName_;
	String buffer;
	loadFileToString( fileName, buffer );
	buffer.erase( std::remove_if( buffer.begin(), buffer.end(), private_::isCRLF ), buffer.end() ); // remove CRLF
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

boost::shared_ptr< Node > Document::getRoot()
{
	return root;
}

void Document::loadFileToString( const String& file_name_, String &buffer_ )
{
	FRL_EXCEPT_GUARD();
	if( ! fs::exists( file_name_ ) )
		FRL_THROW_S_CLASS( Document::FileNotFound );
	size_t length = static_cast< size_t >( fs::file_size( file_name_ ) );
	if( length == 0 )
		FRL_THROW_S_CLASS( Document::EmptyFile );
	if( ! buffer_.empty() )
		buffer_.clear();

	static const size_t buffer_size = 4096;
	stream_std::InFile in( file_name_.c_str(), std::ios::binary );
	if( ! in.is_open() )
		FRL_THROW_S_CLASS( Document::UnknownError );
	std::vector< Char > buf( buffer_size );

	buffer_.reserve( length );
	while( in )
	{
		in.read( &buf[ 0 ], buffer_size );
		buffer_.append( &buf[ 0 ], in.gcount() );
	}
	in.close();
}



} // namespace poor_xml
} // FatRat Library
