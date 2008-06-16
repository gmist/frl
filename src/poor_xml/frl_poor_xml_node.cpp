#include <algorithm>
#include "poor_xml/frl_poor_xml_node.h"
#include "poor_xml/frl_poor_xml_document.h"
#include "frl_util_functors.h"

namespace frl
{
namespace poor_xml
{
Node::Node()
{
}

Node::Node( const String &name_ )
	: name( name_ )
{
}

Node::~Node()
{
}

void Node::setSubNodes( const NodesList &nodes_ )
{
	nodes = nodes_;
}

const String& Node::getName()
{
	return name;
}

NodesList Node::getSubNodes( const String &name_ )
{
	NodesList nodesList;
	NodesList::iterator end = nodes.end();
	for( NodesList::iterator it = nodes.begin(); it != end; ++it )
	{ 
		if( (*it)->getName() == name_ )
			nodesList.push_back( (*it) );
	}
	if( nodesList.empty() )
		FRL_THROW_S_CLASS( Node::NodeNotFound );
	return nodesList;
}

boost::shared_ptr< Node > Node::getFirstNode( const String& name_ )
{
	NodesList::iterator end = nodes.end();
	for( NodesList::iterator it = nodes.begin(); it != end; ++it )
	{
		if( (*it)->getName() == name_ )
			return (*it);
	}
	FRL_THROW_S_CLASS( Node::NodeNotFound );
}

String Node::getProprtyVal( const String &propertyName )
{
	std::map< String, String >::iterator it = properties.find( propertyName );
	if( it == properties.end() )
		FRL_THROW_S_CLASS( Node::PropertyNotFound );
	return (*it).second;
}

boost::shared_ptr< Node > Node::getNode( const String &name_, const String &propertyName, const String &propertyValue )
{
	NodesList nodesList = getSubNodes( name_ );
	NodesList::iterator end = nodesList.end();
	for( NodesList::iterator it = nodesList.begin(); it != end; ++it )
	{
		if( (*it)->getProprtyVal( propertyName ) == propertyValue )
			return (*it );
	}
	FRL_THROW_S_CLASS( Node::NodeNotFound );
}
} // namespace poor_xml
} // FatRat Library
