#include <boost/scoped_array.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/bind.hpp>
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

void Document::LoadFromCurrenttDir( const String& fileName_ )
{
	FRL_EXCEPT_GUARD();
	String currDir;	
	fs::detail::get_current_path_api( currDir );
	currDir += FRL_STR("/");
	fileName = currDir + fileName_;
	size_t length = 0;
	try
	{
		if( ! fs::exists( fileName ) )
			FRL_THROW_S_CLASS( Document::FileNotFound );
		length = static_cast< size_t >( fs::file_size( fileName ) );
	}
	catch( Document::FileNotFound& ex )
	{
		throw ex;
	}
	catch( ... )
	{
		FRL_THROW_S_CLASS( Document::UnknownError );
	}

	if( length == 0 )
		FRL_THROW_S_CLASS( Document::EmptyFile );

	stream_std::InFile file( fileName.c_str() );
	if( ! file.is_open() )
		FRL_THROW_S_CLASS( Document::UnknownError );

	std::list< Char > data( length );
	std::copy(	std::istreambuf_iterator< Char >( file.rdbuf() ),
					std::istreambuf_iterator< Char >(),
					data.begin() );
	file.close();
	data.erase( std::remove_if( data.begin(), data.end(), private_::isCRLF ), data.end() ); // remove CRLF
	String buffer;
	buffer.resize( data.size() );
	std::copy( data.begin(), data.end(), buffer.begin() );
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
} // namespace poor_xml
} // FatRat Library
