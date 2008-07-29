#ifndef frl_poor_xml_parser_h_
#define frl_poor_xml_parser_h_
#include "frl_types.h"
#include "poor_xml/frl_poor_xml_node.h"
#include "frl_exception.h"

namespace frl{ namespace poor_xml{

class Parser
{
public:
	FRL_EXCEPTION_CLASS( PropertyNotFound );
	FRL_EXCEPTION_CLASS( InvalidProperty );
	FRL_EXCEPTION_CLASS( InvalidNode );
	FRL_EXCEPTION_CLASS( BadNodeName );
	FRL_EXCEPTION_CLASS( InvalidArgument );

	static String getProperty( const String &buffer, const String &propertyName );
	static NodesList getSubNodes( const String &buffer );
	static std::map< String, String > getProperties( const String &buffer );
	static String getFirstNodeName( const String & buffer, size_t &startPos, size_t &endPos );
	static size_t findNodeClose( const String &buffer, const String &nodeName, Bool &simple );
	static size_t findCloseBaracket( const String &buffer, size_t fromPos );
};

} // namespace poor_xml
} // FatRat Library

#endif // frl_poor_xml_parser_h_
