#ifndef frl_poor_xml_document_h_
#define frl_poor_xml_document_h_
#include <boost/noncopyable.hpp>
#include "frl_types.h"
#include "frl_exception.h"
#include "poor_xml/frl_poor_xml_node.h"

namespace frl{ namespace poor_xml{

class Document : private boost::noncopyable
{
private:
	boost::shared_ptr< Node > root;
	String version;
	String encoding;
	void parseHeader( String &buffer );
	String fileName;
	void loadFileToString( const String& file_name_, String &buffer_ );
public:
	FRL_EXCEPTION_CLASS( FileNotFound );
	FRL_EXCEPTION_CLASS( EmptyFile );
	FRL_EXCEPTION_CLASS( UnknownError );
	FRL_EXCEPTION_CLASS( BrokenXML );

	Document();
	~Document();
	void loadFromCurrenttDir( const String& fileName );
	const String& getVersion();
	const String& getEncoding();
	boost::shared_ptr< Node > getRoot();
};

} // namespace poor_xml
} // FatRat Library

#endif // frl_poor_xml_document_h_
