#ifndef lexical_cast_test_suite_h_
#define lexical_cast_test_suite_h_
#include <boost/test/unit_test.hpp>
#include "frl_lexical_cast.h"

BOOST_AUTO_TEST_SUITE( lexical_cast )

BOOST_AUTO_TEST_CASE( int_to_string )
{
	frl::String value = frl::lexicalCast< frl::Int, frl::String >( 12 );
	BOOST_CHECK( value == FRL_STR("12") );
}

BOOST_AUTO_TEST_CASE( simply_int_to_string )
{	
	frl::String value = frl::lexicalCast< frl::Int, frl::String >( 12 );
	BOOST_CHECK( value == FRL_STR("12") );
}

BOOST_AUTO_TEST_CASE( int_to_hex_string )
{
	frl::String value = frl::lexicalCast< frl::Int , frl::String >( 12, frl::lexicalMutators::hex() );	
	BOOST_CHECK( value == FRL_STR("c") );
}

BOOST_AUTO_TEST_CASE( int_to_oct_string )
{
	frl::String value = frl::lexicalCast< frl::Int , frl::String >( 123, frl::lexicalMutators::oct() );	
	BOOST_CHECK( value == FRL_STR("173") );
}

BOOST_AUTO_TEST_CASE( string_to_int )
{
	frl::Int value = frl::lexicalCast< frl::String, frl::Int >( FRL_STR("12") );
	BOOST_CHECK_EQUAL( value, 12 );

}

BOOST_AUTO_TEST_CASE( hex_string_to_int )
{
	frl::Int value = frl::lexicalCast< frl::String, frl::Int >( FRL_STR("c"),  frl::lexicalMutators::hex() );
	BOOST_CHECK_EQUAL( value, 12 );
}

BOOST_AUTO_TEST_CASE( oct_hex_to_string )
{
	frl::Int value = frl::lexicalCast< frl::String, frl::Int >( FRL_STR("710"), frl::lexicalMutators::oct() );
	BOOST_CHECK_EQUAL( value, 456 );
}

BOOST_AUTO_TEST_CASE( dec_string_to_int )
{
	frl::Int value = frl::lexicalCast< frl::String, frl::Int >( FRL_STR("712"), frl::lexicalMutators::dec() );
	BOOST_CHECK_EQUAL( value, 712 );
}

#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )

BOOST_AUTO_TEST_CASE( guid_to_string )
{
	static const GUID testGuid = { 0x8f7cc534, 0xec16, 0x4471, { 0xa6, 0x74, 0x21, 0x2, 0x35, 0xfc, 0xd0, 0x67 } };
	frl::String strGuid = frl::lexicalCast< GUID, frl::String >( testGuid );
	BOOST_CHECK( strGuid == FRL_STR("{8F7CC534-EC16-4471-A674-210235FCD067}") );
}

BOOST_AUTO_TEST_CASE( string_to_guid )
{
	static const GUID originalGuid = { 0x8f7cc534, 0xec16, 0x4471, { 0xa6, 0x74, 0x21, 0x2, 0x35, 0xfc, 0xd0, 0x67 } };
	static const frl::String testString = FRL_STR("{8F7CC534-EC16-4471-A674-210235FCD067}");	
	GUID guid = frl::lexicalCast< frl::String, GUID >( testString );
	BOOST_CHECK( guid == originalGuid );
}

#endif // FRL_PLATFORM_WIN32

BOOST_AUTO_TEST_CASE( lower_string )
{
	const frl::String original = FRL_STR( "QwErTy" );
	frl::String modif = frl::lexicalCast< frl::String, frl::String >( original, frl::lexicalMutators::lower() );
	BOOST_CHECK( modif == FRL_STR("qwerty") );
}

BOOST_AUTO_TEST_CASE( upper_string )
{
	const frl::String original = FRL_STR( "QwErTy" );
	frl::String modif = frl::lexicalCast< frl::String, frl::String >( original,  frl::lexicalMutators::upper() );
	BOOST_CHECK( modif == FRL_STR("QWERTY") );
}

BOOST_AUTO_TEST_SUITE_END()

#endif // lexical_cast_test_suite_h_
