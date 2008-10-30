#ifndef poor_xml_test_suite_h_
#define poor_xml_test_suite_h_
#include <boost/test/unit_test.hpp>
#include "poor_xml/frl_poor_xml_document.h"
#include "poor_xml/frl_poor_xml_parser.h"

BOOST_AUTO_TEST_SUITE( poor_xml )

BOOST_AUTO_TEST_CASE( get_node_name )
{
	frl::String node1Str = FRL_STR( "<node1/>" );
	frl::String node2Str = FRL_STR( "<node2 />" );
	frl::String node3Str = FRL_STR( "<node3 property=\"property\" />" );
	frl::String node4Str = FRL_STR( "/>\"<node3 property=\'property\'\"<node4 />" );
	frl::String node5Str = FRL_STR( "/>\"<node3 property=\'property\'\"</>" );
	size_t start = 0;
	size_t end = 0;
	
	frl::String node1;
	BOOST_CHECK_NO_THROW( node1 = frl::poor_xml::Parser::getFirstNodeName( node1Str, start, end ) );

	frl::String node2;
	BOOST_CHECK_NO_THROW( node2 = frl::poor_xml::Parser::getFirstNodeName( node2Str, start, end ) );

	frl::String node3;
	BOOST_CHECK_NO_THROW( node3 = frl::poor_xml::Parser::getFirstNodeName( node3Str, start, end ) );

	frl::String node4;
	BOOST_CHECK_NO_THROW( node4 = frl::poor_xml::Parser::getFirstNodeName( node4Str, start, end ) );

	BOOST_CHECK( node1 == FRL_STR("node1") );
	BOOST_CHECK( node2 == FRL_STR("node2") );
	BOOST_CHECK( node3 == FRL_STR("node3") );
	BOOST_CHECK( node4 == FRL_STR("node4") );
	
	frl::String node5;
	BOOST_CHECK_THROW( 
		node5 = frl::poor_xml::Parser::getFirstNodeName( node5Str, start, end ),
		frl::poor_xml::Parser::BadNodeName );
}

BOOST_AUTO_TEST_CASE( find_node_close )
{
	frl::String node1Str = FRL_STR( "<node1/>" );
	frl::String node2Str = FRL_STR( "<node2 />" );
	frl::String node3Str = FRL_STR( "<node3 property=\"property\" />" );
	frl::String node4Str = FRL_STR( "\"<node3 property=\'property\'\"<node4 />" );
	frl::String node5Str = FRL_STR( "<node5 prop=\"prop\" </node5>" );
	frl::Bool simple = frl::False;
	size_t pos = 0;

	BOOST_CHECK_NO_THROW( pos = frl::poor_xml::Parser::findNodeClose( node1Str, FRL_STR("node1"), simple ) );
	BOOST_CHECK_EQUAL( pos, static_cast<size_t>( 6 ) );

	BOOST_CHECK_NO_THROW( pos = frl::poor_xml::Parser::findNodeClose( node2Str, FRL_STR("node2"), simple ) );
	BOOST_CHECK_EQUAL( pos, static_cast<size_t>( 7 ) );

	BOOST_CHECK_NO_THROW( pos = frl::poor_xml::Parser::findNodeClose( node3Str, FRL_STR("node3"), simple ) );
	BOOST_CHECK_EQUAL( pos, static_cast<size_t>( 27 ) );

	BOOST_CHECK_NO_THROW( pos = frl::poor_xml::Parser::findNodeClose( node4Str, FRL_STR("node4"), simple ) );
	BOOST_CHECK_EQUAL( pos, static_cast<size_t>( 35 ) );

	BOOST_CHECK_NO_THROW( pos = frl::poor_xml::Parser::findNodeClose( node5Str, FRL_STR("node5"), simple ) );
	BOOST_CHECK_EQUAL( pos, static_cast<size_t>( 19 ) );
}

BOOST_AUTO_TEST_CASE( open_not_exist_file )
{
	frl::poor_xml::Document doc;
	BOOST_CHECK_THROW( doc.loadFromCurrenttDir( FRL_STR("12345") ), frl::poor_xml::Document::FileNotFound );
}

BOOST_AUTO_TEST_CASE( open_empty_file )
{
	frl::poor_xml::Document doc;
	BOOST_CHECK_THROW(doc.loadFromCurrenttDir( FRL_STR("empty_file.xml") ), frl::poor_xml::Document::EmptyFile );
}

BOOST_AUTO_TEST_CASE( open_not_empty_file )
{
	frl::poor_xml::Document doc;
	doc.loadFromCurrenttDir( FRL_STR("test_poor_xml.xml") );
	BOOST_CHECK( doc.getVersion() == FRL_STR("1.0") );
	BOOST_CHECK( doc.getEncoding() == FRL_STR("windows-1251") );
	BOOST_CHECK( doc.getRoot()->getName() == FRL_STR("VisualStudioProject" ) );

	boost::shared_ptr< frl::poor_xml::Node > node;
	BOOST_CHECK_NO_THROW( node = doc.getRoot()->getFirstNode( FRL_STR("Configurations") )->getNode( FRL_STR("Configuration"), 
												FRL_STR("Name"),
												FRL_STR("Debug|Win32") ) 
											);
	BOOST_CHECK( node->getProprtyVal( FRL_STR("ConfigurationType") ) == FRL_STR("1") );
}

BOOST_AUTO_TEST_CASE( open_large_file )
{
	frl::poor_xml::Document doc;
	BOOST_CHECK_NO_THROW( doc.loadFromCurrenttDir( FRL_STR("largeFile.xml") ) );
	BOOST_CHECK( doc.getVersion() == FRL_STR("1.0" ) );
	BOOST_CHECK( doc.getEncoding() == FRL_STR( "utf-8" ) );

	boost::shared_ptr< frl::poor_xml::Node > node;
	BOOST_CHECK_NO_THROW( 
		node = doc.getRoot()->getFirstNode(	FRL_STR("members") )->getNode( FRL_STR("member"),
																FRL_STR("name"),
																FRL_STR("T:Microsoft.VisualStudio.Package.AuthoringScope")
																) );
}

BOOST_AUTO_TEST_SUITE_END()

#endif // poor_xml_test_suite_h_
