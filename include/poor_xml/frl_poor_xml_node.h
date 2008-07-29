#ifndef frl_poor_xml_node_h_
#define frl_poor_xml_node_h_
#include <map>
#include <list>
#include <boost/shared_ptr.hpp>
#include "frl_types.h"
#include "frl_exception.h"

namespace frl{ namespace poor_xml{

class Parser;
class Node;

typedef boost::shared_ptr< Node > NodeListEl;
typedef std::list< NodeListEl > NodesList;

class Node
{
friend class Parser;
private:
	NodesList nodes;
	std::map< String, String > properties;
	String name;
public:
	FRL_EXCEPTION_CLASS( NodeNotFound );
	FRL_EXCEPTION_CLASS( PropertyNotFound );

	Node();
	Node( const String &name_ );
	~Node();
	void setSubNodes( const NodesList &nodes_ );
	const String& getName();
	NodesList getSubNodes( const String &name_ );
	String getProprtyVal( const String &propertyName );
	boost::shared_ptr< Node > getFirstNode( const String &name_ );
	boost::shared_ptr< Node > getNode( const String &name_, const String &propertyName, const String &propertyValue );
}; // class Node

} // namespace poor_xml
} // FatRat Library

#endif // frl_poor_xml_node_h_
