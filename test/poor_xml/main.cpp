#include "frl_unit_test.h"
#include "poor_xml/frl_poor_xml_document.h"
#include "poor_xml/frl_poor_xml_parser.h"

frl::Bool testGetNodeName()
{
	frl::String node1Str = FRL_STR( "<node1/>" );
	frl::String node2Str = FRL_STR( "<node2 />" );
	frl::String node3Str = FRL_STR( "<node3 property=\"property\" />" );
	frl::String node4Str = FRL_STR( "/>\"<node3 property=\'property\'\"<node4 />" );
	frl::String node5Str = FRL_STR( "/>\"<node3 property=\'property\'\"</>" );
	size_t start;
	size_t end;
	try
	{
		frl::String node1 = frl::poor_xml::Parser::getFirstNodeName( node1Str, start, end );
		frl::String node2 = frl::poor_xml::Parser::getFirstNodeName( node2Str, start, end );
		frl::String node3 = frl::poor_xml::Parser::getFirstNodeName( node3Str, start, end );
		frl::String node4 = frl::poor_xml::Parser::getFirstNodeName( node4Str, start, end );
		if(	node1 != FRL_STR("node1") ||
			node2 != FRL_STR("node2") ||
			node3 != FRL_STR("node3") ||
			node4 != FRL_STR("node4") )
			return frl::False;
	}
	catch( frl::Exception )
	{
		return frl::False;
	}
	try
	{
		frl::String node5 = frl::poor_xml::Parser::getFirstNodeName( node5Str, start, end );
	}
	catch( frl::poor_xml::Parser::BadNodeName )
	{
		return frl::True;
	}
	return frl::False;
}

frl::Bool testFindNodeClose()
{
	frl::String node1Str = FRL_STR( "<node1/>" );
	frl::String node2Str = FRL_STR( "<node2 />" );
	frl::String node3Str = FRL_STR( "<node3 property=\"property\" />" );
	frl::String node4Str = FRL_STR( "\"<node3 property=\'property\'\"<node4 />" );
	frl::String node5Str = FRL_STR( "<node5 prop=\"prop\" </node5>" );
	frl::Bool simple;
	size_t pos = frl::poor_xml::Parser::findNodeClose( node1Str, FRL_STR("node1"), simple );
	if( pos != 6 )
		return frl::False;

	pos = frl::poor_xml::Parser::findNodeClose( node2Str, FRL_STR("node2"), simple );
	if( pos != 7 )
		return frl::False;

	pos = frl::poor_xml::Parser::findNodeClose( node3Str, FRL_STR("node3"), simple );
	if( pos != 27 )
		return frl::False;

	pos = frl::poor_xml::Parser::findNodeClose( node4Str, FRL_STR("node4"), simple );
	if( pos != 35 )
		return frl::False;

	pos = frl::poor_xml::Parser::findNodeClose( node5Str, FRL_STR("node5"), simple );
	if( pos != 19 )
		return frl::False;

	return frl::True;
}

frl::Bool openNotExistFile()
{
	frl::poor_xml::Document doc;
	try
	{
		doc.LoadFromCurrenttDir( FRL_STR("12345") );
	}
	catch( frl::poor_xml::Document::FileNotFound )
	{
		return frl::True;
	}
	catch( ... )
	{
		return frl::False;
	}
	return frl::False;
}

frl::Bool openEmptyFile()
{
	frl::poor_xml::Document doc;
	try
	{
		doc.LoadFromCurrenttDir( FRL_STR("empty_file.xml") );
	}
	catch( frl::poor_xml::Document::EmptyFile )
	{
		return frl::True;
	}
	catch( ... )
	{
		return frl::False;
	}
	return frl::False;
}

frl::Bool openNotEmptyFile()
{
	frl::poor_xml::Document doc;
	try
	{
		doc.LoadFromCurrenttDir( FRL_STR("test_poor_xml.xml") );
		if( doc.getVersion() != FRL_STR("1.0" ) || doc.getEncoding() != FRL_STR( "windows-1251" ) )
			return frl::False;
		if( doc.getRoot()->getName() != FRL_STR("VisualStudioProject" ) )
			return frl::False;
		
		boost::shared_ptr< frl::poor_xml::Node > node;
		node = doc.getRoot()->getFirstNode( FRL_STR("Configurations") )->getNode( FRL_STR("Configuration"), 
					FRL_STR("Name"),
					FRL_STR("Debug|Win32") );
		if( node->getProprtyVal( FRL_STR("ConfigurationType") ) != FRL_STR("1") )
			return frl::False;
	}
	catch( frl::Exception )
	{
		return frl::False;
	}
	return frl::True;
}

frl::Bool openLargeFile()
{
	frl::poor_xml::Document doc;
	try
	{
		doc.LoadFromCurrenttDir( FRL_STR("largeFile.xml") );
		if( doc.getVersion() != FRL_STR("1.0" ) || doc.getEncoding() != FRL_STR( "utf-8" ) )
			return frl::False;
		boost::shared_ptr< frl::poor_xml::Node > node;
		node = doc.getRoot()->getFirstNode( FRL_STR("members") )->getNode( FRL_STR("member"),
					FRL_STR("name"),
					FRL_STR("T:Microsoft.VisualStudio.Package.AuthoringScope") );
	}
	catch( frl::Exception )
	{
		return frl::False;
	}
	return frl::True;
}


int main( int argc, char *argv[] )
{
	frl::unit_test::ConsoleUnitTest poorXml( FRL_STR( "frl::poor_xml" ) );
	poorXml.Add( testGetNodeName, FRL_STR("Get node name from string") );
	poorXml.Add( testFindNodeClose, FRL_STR("Find close node") );
	poorXml.Add( openNotExistFile );
	poorXml.Add( openEmptyFile );
	poorXml.Add( openNotEmptyFile );
	poorXml.Add( openLargeFile );	
	return 0;
}
