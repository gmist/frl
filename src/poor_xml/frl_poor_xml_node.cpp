#include <boost/foreach.hpp>
#include <algorithm>
#include "poor_xml/frl_poor_xml_node.h"
#include "poor_xml/frl_poor_xml_document.h"

namespace frl{ namespace poor_xml{

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
	BOOST_FOREACH( NodeListEl &el, nodes )
	{
		if( el->getName() == name_ )
			nodesList.push_back( el );
	}
	if( nodesList.empty() )
		FRL_THROW_S_CLASS( Node::NodeNotFound );
	return nodesList;
}

NodeListEl Node::getFirstNode( const String& name_ )
{
	BOOST_FOREACH( NodeListEl &el, nodes )
	{
		if( el->getName() == name_ )
			return el;
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
	BOOST_FOREACH( NodeListEl &el, nodes )
	{
		if( el->getProprtyVal( propertyName ) == propertyValue )
			return el;
	}
	FRL_THROW_S_CLASS( Node::NodeNotFound );
}

} // namespace poor_xml
} // FatRat Library
