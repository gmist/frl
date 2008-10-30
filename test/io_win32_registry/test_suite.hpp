#ifndef os_win32_registry_test_suite_h_
#define os_win32_registry_test_suite_h_
#include <boost/test/unit_test.hpp>
#include "os/win32/registry/frl_registry_Key.h"

BOOST_AUTO_TEST_SUITE( os_win32_registry )

BOOST_AUTO_TEST_CASE( is_exist_key )
{
	using namespace frl::os::win32::registry;
	Key testKey( FRL_STR("Software") );
	BOOST_CHECK( testKey.isExist() );
}

BOOST_AUTO_TEST_CASE( is_exist_value )
{
	frl::os::win32::registry::Key key( FRL_STR("Console"));
	BOOST_CHECK( key.isExistValue( FRL_STR("WindowSize") ) );
}

BOOST_AUTO_TEST_CASE( create_and_remove )
{
	using namespace frl::os::win32::registry;
	Key testKey( FRL_STR("{A9290406-9B9A-4f9b-AF9A-EA7D8A8EF4B2}") );

	// if find old test key - remove this
	if ( testKey.isExist() )
	{
		BOOST_CHECK_NO_THROW( testKey.deleteKey() );
	}

	BOOST_CHECK_NO_THROW( testKey.create() );
	BOOST_CHECK( testKey.isExist() );
	BOOST_CHECK_NO_THROW( testKey.deleteKey() );
	BOOST_CHECK( ! testKey.isExist() );
}

BOOST_AUTO_TEST_CASE( create_full_path_and_recursive_delete)
{
	using namespace frl::os::win32::registry;
	
	Key removeKey( FRL_STR("{2B870503-751D-4230-84E5-42527743F742}") );

	// if find old test key - remove this
	if( removeKey.isExist() )
	{
		BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
		BOOST_CHECK( ! removeKey.isExist() );
	}

	// create test keys
	frl::os::win32::registry::Key testKey( FRL_STR("{2B870503-751D-4230-84E5-42527743F742}\\test") );
	frl::os::win32::registry::Key testKey1( FRL_STR("{2B870503-751D-4230-84E5-42527743F742}\\test\\test1") );
	frl::os::win32::registry::Key testKey2( FRL_STR("{2B870503-751D-4230-84E5-42527743F742}\\test\\test1\\test2") );
	BOOST_CHECK_NO_THROW( testKey2.create() ); // create sub key and full path

	BOOST_CHECK( testKey.isExist() );
	BOOST_CHECK( testKey1.isExist() );
	BOOST_CHECK( testKey2.isExist() );

	BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
	BOOST_CHECK( ! removeKey.isExist() );
}

BOOST_AUTO_TEST_CASE( DWORD_value_test )
{
	using namespace frl::os::win32::registry;
	Key removeKey( FRL_STR("{90C77F11-2CA8-4278-B4C1-D48E2FB43C8C}" ) );

	// if find old test key - remove this
	if( removeKey.isExist() )
	{
		BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
		BOOST_CHECK( ! removeKey.isExist() );
	}

	Key testKey( FRL_STR("{90C77F11-2CA8-4278-B4C1-D48E2FB43C8C}\\test"), frl::os::win32::registry::RootKeys::currentUser );
	Key testKey2( FRL_STR("{90C77F11-2CA8-4278-B4C1-D48E2FB43C8C}\\test"), frl::os::win32::registry::RootKeys::currentUser );
	BOOST_CHECK_NO_THROW( testKey.setDWORDValue( FRL_STR("test_dword"), 10 ) );
	BOOST_CHECK_EQUAL( testKey2.getDWORDValue( FRL_STR("test_dword")), static_cast<DWORD>(10) );

	BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
}

BOOST_AUTO_TEST_CASE( DWORD_default_value_test )
{
	using namespace frl::os::win32::registry;	
	Key removeKey( FRL_STR("{0D9579E5-3091-4906-96CD-D9932E324670}" ) );
	
	// if find old test key - remove this
	if( removeKey.isExist() )
	{
		BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
		BOOST_CHECK( ! removeKey.isExist() );
	}

	Key testKey( FRL_STR("{0D9579E5-3091-4906-96CD-D9932E324670}\\test"), frl::os::win32::registry::RootKeys::currentUser );
	Key testKey2( FRL_STR("{0D9579E5-3091-4906-96CD-D9932E324670}\\test"), frl::os::win32::registry::RootKeys::currentUser );

	BOOST_CHECK_NO_THROW( testKey.setDWORDValue(10) );
	BOOST_CHECK_EQUAL( testKey2.getDWORDValue(), static_cast<DWORD>(10) );
}

BOOST_AUTO_TEST_CASE( string_value_test )
{
	using namespace frl::os::win32::registry;
	Key removeKey( FRL_STR("{C185F268-3904-4630-87C6-371B274077AD}") );

	if( removeKey.isExist() )
	{
		BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
		BOOST_CHECK( ! removeKey.isExist() );
	}

	Key key1 ( FRL_STR("{C185F268-3904-4630-87C6-371B274077AD}\\test") );
	frl::String testMsg1 = FRL_STR("I`m testing StringValue" );
	BOOST_CHECK_NO_THROW( key1.setStringValue( FRL_STR("test_str"), testMsg1 ) );

	Key key2 ( FRL_STR("{C185F268-3904-4630-87C6-371B274077AD}\\test") );
	BOOST_CHECK( key2.getStringValue( FRL_STR("test_str") ) == testMsg1 );
	BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
}

BOOST_AUTO_TEST_CASE( string_default_value_test )
{
	using namespace frl::os::win32::registry;
	Key removeKey( FRL_STR("{6CC02086-72CE-433a-8B9F-4A6B7A229EC2}" ) );

	if( removeKey.isExist() )
	{
		BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
		BOOST_CHECK( ! removeKey.isExist() );
	}

	Key key1 ( FRL_STR("{6CC02086-72CE-433a-8B9F-4A6B7A229EC2}\\test") );
	frl::String testMsg1 = FRL_STR("I`m testing StringValue" );
	BOOST_CHECK_NO_THROW( key1.setStringValue( testMsg1 ) );
	Key key2 ( FRL_STR("{6CC02086-72CE-433a-8B9F-4A6B7A229EC2}\\test") );
	BOOST_CHECK( key2.getStringValue() == testMsg1 );
	BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
}

BOOST_AUTO_TEST_CASE( multi_string_value_test )
{
	using namespace frl::os::win32::registry;
	Key removeKey( FRL_STR("{9648C663-5517-4a7e-AFDA-44DE68BD5188}") );

	if( removeKey.isExist() )
	{
		BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
		BOOST_CHECK( ! removeKey.isExist() );
	}

	Key key1( FRL_STR("{9648C663-5517-4a7e-AFDA-44DE68BD5188}\\test") );
	std::vector< frl::String > val;
	val.push_back(FRL_STR("I`m testing MultiStringValue1"));
	val.push_back(FRL_STR("I`m testing MultiStringValue2"));
	BOOST_CHECK_NO_THROW( key1.setMultiStringValue( FRL_STR("test_str"), val ) );
	Key key2( FRL_STR("{9648C663-5517-4a7e-AFDA-44DE68BD5188}\\test" ) );
	BOOST_CHECK( key2.getMultiStringValue( FRL_STR("test_str") ) == val );
	BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
}

BOOST_AUTO_TEST_CASE( multi_string_default_value_test )
{
	using namespace frl::os::win32::registry;
	Key removeKey( FRL_STR("{C2B4FDB9-9FB9-41a3-91BD-AF16F32FF85A}") );

	if( removeKey.isExist() )
	{
		BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
		BOOST_CHECK( ! removeKey.isExist() );
	}

	Key key1( FRL_STR("{C2B4FDB9-9FB9-41a3-91BD-AF16F32FF85A}\\test") );
	std::vector< frl::String > val;
	val.push_back(FRL_STR("I`m testing MultiStringValue1"));
	val.push_back(FRL_STR("I`m testing MultiStringValue2"));
	BOOST_CHECK_NO_THROW( key1.setMultiStringValue( val ) );
	Key key2( FRL_STR("{C2B4FDB9-9FB9-41a3-91BD-AF16F32FF85A}\\test") );
	BOOST_CHECK( key2.getMultiStringValue() == val );
	BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
}

BOOST_AUTO_TEST_CASE( binary_value_test )
{
	using namespace frl::os::win32::registry;
	Key removeKey( FRL_STR("{1FB0B8D2-26B7-4043-9514-FEFE2D2DD28D}") );

	if( removeKey.isExist() )
	{
		BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
		BOOST_CHECK( ! removeKey.isExist() );
	}

	Key key1( FRL_STR("{1FB0B8D2-26B7-4043-9514-FEFE2D2DD28D}") );
	std::vector< unsigned char > zz1;
	for( int i = 0; i < 200; i++ )
		zz1.push_back( (unsigned char) i );

	BOOST_CHECK_NO_THROW( key1.setBinaryValue( FRL_STR("test_binary_value"), zz1) );
	Key key2( FRL_STR("{1FB0B8D2-26B7-4043-9514-FEFE2D2DD28D}") );
	BOOST_CHECK( key2.getBinaryValue( FRL_STR("test_binary_value") ) == zz1 );
	BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
}

BOOST_AUTO_TEST_CASE( binary_default_value_test )
{
	using namespace frl::os::win32::registry;
	Key removeKey( FRL_STR("{80ACE291-7E40-4438-8122-242FA8120460}") );
	
	if( removeKey.isExist() )
	{
		BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
		BOOST_CHECK( ! removeKey.isExist() );
	}

	
	frl::os::win32::registry::Key key1( FRL_STR("{80ACE291-7E40-4438-8122-242FA8120460}") );
	std::vector< unsigned char > zz1;
	for( int i = 0; i < 200; i++ )
		zz1.push_back( (unsigned char) i );

	BOOST_CHECK_NO_THROW( key1.setBinaryValue( zz1) );
	frl::os::win32::registry::Key key2( FRL_STR("{80ACE291-7E40-4438-8122-242FA8120460}") );
	BOOST_CHECK( key2.getBinaryValue() == zz1 );
	BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
}

BOOST_AUTO_TEST_CASE( get_num_sub_keys )
{
	using namespace frl::os::win32::registry;
	Key removeKey( FRL_STR("{0CD3CAF5-AC99-445a-80B4-CCCA9DA1CB0A}"));

	if( removeKey.isExist() )
	{
		BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
		BOOST_CHECK( ! removeKey.isExist() );
	}

	Key key1( FRL_STR("{0CD3CAF5-AC99-445a-80B4-CCCA9DA1CB0A}\\test1"));
	BOOST_CHECK_NO_THROW( key1.create() );
	Key key2( FRL_STR("{0CD3CAF5-AC99-445a-80B4-CCCA9DA1CB0A}\\test2"));
	BOOST_CHECK_NO_THROW( key2.create() );
	Key key3( FRL_STR("{0CD3CAF5-AC99-445a-80B4-CCCA9DA1CB0A}\\test3"));
	BOOST_CHECK_NO_THROW( key3.create() );
	Key key4( FRL_STR("{0CD3CAF5-AC99-445a-80B4-CCCA9DA1CB0A}\\test4"));
	BOOST_CHECK_NO_THROW( key4.create() );
	Key key4_1( FRL_STR("{0CD3CAF5-AC99-445a-80B4-CCCA9DA1CB0A}\\test4\\test4.1"));
	BOOST_CHECK_NO_THROW( key4_1.create() );

	BOOST_CHECK_EQUAL( removeKey.getNumSubkeys(), 4 );
	BOOST_CHECK_NO_THROW( removeKey.deleteKey(frl::True) );
}

BOOST_AUTO_TEST_CASE( get_num_sub_values_test )
{
	using namespace frl::os::win32::registry;
	Key removeKey( FRL_STR("{0C5E2BDA-9FC8-45ba-B922-E55C9B3BA8EE}"));

	if( removeKey.isExist() )
	{
		BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
		BOOST_CHECK( ! removeKey.isExist() );
	}

	BOOST_CHECK_NO_THROW( removeKey.create() );

	frl::os::win32::registry::Key key( FRL_STR("{0C5E2BDA-9FC8-45ba-B922-E55C9B3BA8EE}"));
	key.setDWORDValue( FRL_STR("value0"), 0 );
	key.setDWORDValue( FRL_STR("value1"), 1 );
	key.setDWORDValue( FRL_STR("value2"), 2 );
	key.setDWORDValue( FRL_STR("value3"), 3 );

	BOOST_CHECK_EQUAL( removeKey.getNumSubvalues(), 4 );
	BOOST_CHECK_NO_THROW( removeKey.deleteKey( frl::True ) );
}

BOOST_AUTO_TEST_SUITE_END()

#endif // os_win32_registry_test_suite_h_
