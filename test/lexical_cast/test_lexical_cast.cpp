#include "frl_lexical_cast.h"
#include "frl_unit_test.h"

frl::Bool intToStringTest()
{
	frl::String value = frl::lexicalCast< frl::Int, frl::String >::get( 12 );
	if ( value == FRL_STR("12") )
		return frl::True;
	return frl::False;
}

frl::Bool intToStringSimplyTest()
{
	frl::String value = frl::lexicalCast< frl::Int >::get( 12 );
	if ( value == FRL_STR("12") )	
		return frl::True;
	return frl::False;
}


frl::Bool intToHexStringTest()
{
	frl::String value = frl::lexicalCast< frl::Int , frl::String, frl::lexicalCastMutators::hex >::get( 12 );	
	if ( value == FRL_STR("c") )
		return frl::True;
	return frl::False;
}

frl::Bool intToOctStringTest()
{
	frl::String value = frl::lexicalCast< frl::Int , frl::String, frl::lexicalCastMutators::oct >::get( 123 );	
	if ( value == FRL_STR("173") )
		return frl::True;
	return frl::False;
}

frl::Bool stringToInt()
{
	frl::Int value = frl::lexicalCast< frl::String, frl::Int >::get( FRL_STR("12") );
	if ( value == 12)
		return frl::True;
	return frl::False;
}

frl::Bool hexStringToInt()
{
	frl::Int value = frl::lexicalCast< frl::String, frl::Int, frl::lexicalCastMutators::hex >::get( FRL_STR("c") );
	if ( value == 12)
		return frl::True;
	return frl::False;
}

frl::Bool octStringToInt()
{
	frl::Int value = frl::lexicalCast< frl::String, frl::Int, frl::lexicalCastMutators::oct >::get( FRL_STR("710") );
	if ( value == 456)
		return frl::True;
	return frl::False;
}

frl::Bool decStringToInt()
{
	frl::Int value = frl::lexicalCast< frl::String, frl::Int, frl::lexicalCastMutators::dec >::get( FRL_STR("712") );
	if ( value == 712 )
		return frl::True;
	return frl::False;
}

#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )

frl::Bool guidToString()
{
	static const GUID testGuid = { 0x8f7cc534, 0xec16, 0x4471, { 0xa6, 0x74, 0x21, 0x2, 0x35, 0xfc, 0xd0, 0x67 } };
	frl::String strGuid = frl::lexicalCast< GUID, frl::String >::get( testGuid );
	return strGuid == FRL_STR("{8F7CC534-EC16-4471-A674-210235FCD067}");	
}

frl::Bool stringToGuid()
{
	static const GUID originalGuid = { 0x8f7cc534, 0xec16, 0x4471, { 0xa6, 0x74, 0x21, 0x2, 0x35, 0xfc, 0xd0, 0x67 } };
	static const frl::String testString = FRL_STR("{8F7CC534-EC16-4471-A674-210235FCD067}");	
	GUID guid = frl::lexicalCast< frl::String, GUID >::get( testString );
	return (guid == originalGuid) == 1;
}

#endif // FRL_PLATFORM_WIN32

frl::Bool lowerString()
{
	const frl::String original = FRL_STR( "QwErTy" );
	frl::String modif = frl::lexicalCast< frl::lexicalCastMutators::lower >::get( original );
	if( modif == FRL_STR("qwerty") )
		return frl::True;
	return frl::False;
}

frl::Bool upperString()
{
	const frl::String original = FRL_STR( "QwErTy" );
	frl::String modif = frl::lexicalCast< frl::lexicalCastMutators::upper >::get( original );
	if( modif == FRL_STR("QWERTY") )
		return frl::True;
	return frl::False;
}

int main( int argc, char *argv[] )
{
	frl::unit_test::ConsoleUnitTest test( FRL_STR("Lexical cast test.") );
	test.Add( intToStringTest, FRL_STR("Int to string.") );
	test.Add( intToStringSimplyTest, FRL_STR("Int to string (simply).") );
	test.Add ( intToHexStringTest, FRL_STR("Int to hex string.") );
	test.Add( intToOctStringTest, FRL_STR("Int to oct string.") );
	test.Add ( stringToInt, FRL_STR("String to int.") );
	test.Add( hexStringToInt,  FRL_STR("HEX string to int.") );	
	test.Add( octStringToInt, FRL_STR( "OCT string to int.") );
	test.Add( decStringToInt, FRL_STR("DEC string to int.") );
	#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
	test.Add( guidToString, FRL_STR("GUID value to string.") );
	test.Add( stringToGuid, FRL_STR( "String to guid value.") );
	#endif
	test.Add( lowerString, FRL_STR( "Lower cast string" ) );
	test.Add( upperString, FRL_STR( "Upper cast string" ) );
	return 0;
}
