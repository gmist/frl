#include <iostream>
#include "frl_unit_test.h"
#include "frl_io.h"
#include "frl_exception.h"

frl::Bool isExistTest()
{
	try
	{
		frl::io::win32::registry::Key testKey( FRL_STR("Software") );
		if ( testKey.isExist() )
			return frl::True;
	}
	catch ( frl::Exception &ex )
	{
		ex.~Exception();
		return frl::False;
	}
	return frl::False;
}

frl::Bool createAndDeleteTest()
{
	frl::io::win32::registry::Key testKey( FRL_STR("{A9290406-9B9A-4f9b-AF9A-EA7D8A8EF4B2}") );
	try
	{		
		if ( testKey.isExist() )
			testKey.deleteKey();
		testKey.create();
		if( ! testKey.isExist() )
			return frl::False;
		testKey.deleteKey();
		if( testKey.isExist() )
			return frl::False;
		return frl::True;
	}
	catch (frl::Exception &ex)
	{
		ex.~Exception();
		testKey.deleteKey(frl::True);
		return frl::False;
	}
	return frl::False;
}

frl::Bool createFullPathAndRecurciveDelete()
{
	frl::io::win32::registry::Key removeKey( FRL_STR("{2B870503-751D-4230-84E5-42527743F742}") );
	try
	{
		removeKey.deleteKey( frl::True );
		frl::io::win32::registry::Key testKey( FRL_STR("{2B870503-751D-4230-84E5-42527743F742}\\test") );
		frl::io::win32::registry::Key testKey1( FRL_STR("{2B870503-751D-4230-84E5-42527743F742}\\test\\test1") );
		frl::io::win32::registry::Key testKey2( FRL_STR("{2B870503-751D-4230-84E5-42527743F742}\\test\\test1\\test2") );		
		if ( removeKey.isExist() )
			removeKey.deleteKey( frl::True );
		testKey2.create();
		if ( !testKey.isExist() || !testKey1.isExist() || !testKey2.isExist() )
			FRL_THROW( FRL_STR("Error") );
		removeKey.deleteKey( frl::True );
		if ( testKey.isExist() || testKey1.isExist() || testKey2.isExist() || removeKey.isExist() )
			FRL_THROW( FRL_STR("Error") );
		return frl::True;
	}
	catch ( frl::Exception &ex )
	{
		ex.~Exception();
		if ( removeKey.isExist() )
			removeKey.deleteKey( frl::True );
		return frl::False;
	}
	return frl::False;
}

frl::Bool DWORDValueTest()
{
	frl::io::win32::registry::Key removeKey( FRL_STR("{1D627656-FDDB-46b5-AFA2-2AA5E41D18F8}" ) );
	try
	{
		removeKey.deleteKey( frl::True );
		frl::io::win32::registry::Key testKey( FRL_STR("{1D627656-FDDB-46b5-AFA2-2AA5E41D18F8}\\test"), frl::io::win32::registry::RootKeys::currentUser );
		frl::io::win32::registry::Key testKey2( FRL_STR("{1D627656-FDDB-46b5-AFA2-2AA5E41D18F8}\\test"), frl::io::win32::registry::RootKeys::currentUser );
		testKey.setDWORDValue( FRL_STR("test_dword"), 10 );
		if( testKey2.getDWORDValue( FRL_STR("test_dword") ) != 10 )
			FRL_THROW( FRL_STR("Error!") );		
		removeKey.deleteKey( frl::True );	
		return frl::True;
	}
	catch( frl::Exception &ex )
	{
		ex.~Exception();			
		removeKey.deleteKey( frl::True );
		return frl::False;
	}
	return frl::False;
}

frl::Bool DWORDDefaultValueTest()
{
	frl::io::win32::registry::Key removeKey( FRL_STR("{1D627656-FDDB-46b5-AFA2-2AA5E41D18F8}" ) );
	try
	{
		removeKey.deleteKey( frl::True );
		frl::io::win32::registry::Key testKey( FRL_STR("{1D627656-FDDB-46b5-AFA2-2AA5E41D18F8}\\test"), frl::io::win32::registry::RootKeys::currentUser );
		frl::io::win32::registry::Key testKey2( FRL_STR("{1D627656-FDDB-46b5-AFA2-2AA5E41D18F8}\\test"), frl::io::win32::registry::RootKeys::currentUser );
		testKey.setDWORDValue( 10 );
		if( testKey2.getDWORDValue() != 10 )
			FRL_THROW( FRL_STR("Error!") );		
		removeKey.deleteKey( frl::True );	
		return frl::True;
	}
	catch( frl::Exception &ex )
	{
		ex.~Exception();			
		removeKey.deleteKey( frl::True );
		return frl::False;
	}
	return frl::False;
}


frl::Bool StringValueTest()
{
	frl::io::win32::registry::Key removeKey( FRL_STR("{C185F268-3904-4630-87C6-371B274077AD}" ) );
	try
	{
		removeKey.deleteKey( frl::True );
		frl::io::win32::registry::Key key1 ( FRL_STR("{C185F268-3904-4630-87C6-371B274077AD}\\test ") );
		frl::String testMsg1 = FRL_STR("I`m testing StringValue" );
		key1.setStringValue( FRL_STR("test_str"), testMsg1 );
		frl::io::win32::registry::Key key2 ( FRL_STR("{C185F268-3904-4630-87C6-371B274077AD}\\test ") );
		if( key2.getStringValue( FRL_STR("test_str")) != testMsg1 )
			FRL_THROW( FRL_STR("Error!") );		
		removeKey.deleteKey( frl::True );
		return frl::True;	
	}
	catch ( frl::Exception &ex )
	{
		ex.~Exception();			
		removeKey.deleteKey( frl::True );
		return frl::False;
	}
	return frl::False;
}

frl::Bool StringDefaultValueTest()
{
	frl::io::win32::registry::Key removeKey( FRL_STR("{C185F268-3904-4630-87C6-371B274077AD}" ) );
	try
	{
		removeKey.deleteKey( frl::True );
		frl::io::win32::registry::Key key1 ( FRL_STR("{C185F268-3904-4630-87C6-371B274077AD}\\test ") );
		frl::String testMsg1 = FRL_STR("I`m testing StringValue" );
		key1.setStringValue( testMsg1 );
		frl::io::win32::registry::Key key2 ( FRL_STR("{C185F268-3904-4630-87C6-371B274077AD}\\test ") );
		if( key2.getStringValue() != testMsg1 )
			FRL_THROW( FRL_STR("Error!") );		
		removeKey.deleteKey( frl::True );
		return frl::True;	
	}
	catch ( frl::Exception &ex )
	{
		ex.~Exception();			
		removeKey.deleteKey( frl::True );
		return frl::False;
	}
	return frl::False;
}


frl::Bool MultiStringValueTest()
{
	frl::io::win32::registry::Key removeKey( FRL_STR("{9648C663-5517-4a7e-AFDA-44DE68BD5188}" ) );
	try
	{
		removeKey.deleteKey( frl::True );
		frl::io::win32::registry::Key key1( FRL_STR("{9648C663-5517-4a7e-AFDA-44DE68BD5188}\\test" ) );
		std::vector< frl::String > val;
		val.push_back(FRL_STR("I`m testing MultiStringValue1"));
		val.push_back(FRL_STR("I`m testing MultiStringValue2"));
		key1.setMultiStringValue( FRL_STR("test_str"), val );
		frl::io::win32::registry::Key key2( FRL_STR("{9648C663-5517-4a7e-AFDA-44DE68BD5188}\\test" ) );						
		if ( key2.getMultiStringValue( FRL_STR("test_str") ) != val )
			FRL_THROW(FRL_STR("Error"));				
		removeKey.deleteKey( frl::True );
		return frl::True;	
	}
	catch ( frl::Exception &ex )
	{
		ex.~Exception();			
		removeKey.deleteKey( frl::True );
		return frl::False;
	}
	return frl::False;
}

frl::Bool MultiStringDefaultValueTest()
{
	frl::io::win32::registry::Key removeKey( FRL_STR("{9648C663-5517-4a7e-AFDA-44DE68BD5188}" ) );
	try
	{
		removeKey.deleteKey( frl::True );
		frl::io::win32::registry::Key key1( FRL_STR("{9648C663-5517-4a7e-AFDA-44DE68BD5188}\\test" ) );
		std::vector< frl::String > val;
		val.push_back(FRL_STR("I`m testing MultiStringValue1"));
		val.push_back(FRL_STR("I`m testing MultiStringValue2"));
		key1.setMultiStringValue( val );
		frl::io::win32::registry::Key key2( FRL_STR("{9648C663-5517-4a7e-AFDA-44DE68BD5188}\\test" ) );						
		if ( key2.getMultiStringValue() != val )
			FRL_THROW(FRL_STR("Error"));				
		removeKey.deleteKey( frl::True );
		return frl::True;	
	}
	catch ( frl::Exception &ex )
	{
		ex.~Exception();			
		removeKey.deleteKey( frl::True );
		return frl::False;
	}
	return frl::False;
}


frl::Bool isExistValueTest()
{
	frl::io::win32::registry::Key key( FRL_STR("Console"));	
	return key.isExistValue( FRL_STR("WindowSize") );
}

frl::Bool BinaryValueTest()
{
	frl::io::win32::registry::Key removeKey( FRL_STR("{1FB0B8D2-26B7-4043-9514-FEFE2D2DD28D}") );
	try
	{
		removeKey.deleteKey( frl::True );		
		frl::io::win32::registry::Key key1( FRL_STR("{1FB0B8D2-26B7-4043-9514-FEFE2D2DD28D}") );
		std::vector< unsigned char > zz1;		
		for( int i = 0; i < 200; i++ )
			zz1.push_back( i );
		key1.setBinaryValue( FRL_STR("test_binary_value"), zz1);
		frl::io::win32::registry::Key key2( FRL_STR("{1FB0B8D2-26B7-4043-9514-FEFE2D2DD28D}") );
		if( key2.getBinaryValue( FRL_STR("test_binary_value") ) != zz1 )
			FRL_THROW( FRL_STR("error"));
		removeKey.deleteKey( frl::True );
		return frl::True;
	}
	catch ( frl::Exception &ex )
	{
		ex.~Exception();
		removeKey.deleteKey( frl::True );
		return frl::False;
	}	
	return frl::False;
}

frl::Bool BinaryDefaultValueTest()
{
	frl::io::win32::registry::Key removeKey( FRL_STR("{1FB0B8D2-26B7-4043-9514-FEFE2D2DD28D}") );
	try
	{
		removeKey.deleteKey( frl::True );		
		frl::io::win32::registry::Key key1( FRL_STR("{1FB0B8D2-26B7-4043-9514-FEFE2D2DD28D}") );
		std::vector< unsigned char > zz1;		
		for( int i = 0; i < 200; i++ )
			zz1.push_back( i );
		key1.setBinaryValue( zz1);
		frl::io::win32::registry::Key key2( FRL_STR("{1FB0B8D2-26B7-4043-9514-FEFE2D2DD28D}") );
		if( key2.getBinaryValue() != zz1 )
			FRL_THROW( FRL_STR("error"));
		removeKey.deleteKey( frl::True );
		return frl::True;
	}
	catch ( frl::Exception &ex )
	{
		ex.~Exception();
		removeKey.deleteKey( frl::True );
		return frl::False;
	}	
	return frl::False;
}


frl::Bool GetNumSubKeysTest()
{
	frl::io::win32::registry::Key removeKey( FRL_STR("{0CD3CAF5-AC99-445a-80B4-CCCA9DA1CB0A}"));
	try
	{		
		removeKey.deleteKey( frl::True );
		frl::io::win32::registry::Key key1( FRL_STR("{0CD3CAF5-AC99-445a-80B4-CCCA9DA1CB0A}\\test1"));
		key1.create();
		frl::io::win32::registry::Key key2( FRL_STR("{0CD3CAF5-AC99-445a-80B4-CCCA9DA1CB0A}\\test2"));
		key2.create();
		frl::io::win32::registry::Key key3( FRL_STR("{0CD3CAF5-AC99-445a-80B4-CCCA9DA1CB0A}\\test3"));
		key3.create();
		frl::io::win32::registry::Key key4( FRL_STR("{0CD3CAF5-AC99-445a-80B4-CCCA9DA1CB0A}\\test4"));
		key4.create();
		frl::io::win32::registry::Key key4_1( FRL_STR("{0CD3CAF5-AC99-445a-80B4-CCCA9DA1CB0A}\\test4\\test4.1"));
		key4_1.create();

		int num = removeKey.getNumSubkeys();
		removeKey.deleteKey(frl::True);
		return num == 4;

	}
	catch ( frl::Exception &ex)
	{
		ex.~Exception();
		removeKey.deleteKey(frl::True);
		return frl::False;
	}
	return frl::False;
}

frl::Bool GetNumSubvaluesTest()
{
	frl::io::win32::registry::Key removeKey( FRL_STR("{0C5E2BDA-9FC8-45ba-B922-E55C9B3BA8EE}"));
	try
	{
		removeKey.deleteKey( frl::True );
		removeKey.create();
		frl::io::win32::registry::Key key( FRL_STR("{0C5E2BDA-9FC8-45ba-B922-E55C9B3BA8EE}"));
		key.setDWORDValue( FRL_STR("value0"), 0 );
		key.setDWORDValue( FRL_STR("value1"), 1 );
		key.setDWORDValue( FRL_STR("value2"), 2 );
		key.setDWORDValue( FRL_STR("value3"), 3 );		
		int num = removeKey.getNumSubvalues();
		removeKey.deleteKey( frl::True );
		return num == 4;
	}
	catch ( frl::Exception &ex )
	{
		ex.~Exception();
		removeKey.deleteKey( frl::True );
		return frl::False;
	}
	return frl::False;
}

int main( int argc, char *argv[] )
{
	frl::unit_test::ConsoleUnitTest testRegistry( FRL_STR("frl::os::windows::registry") );
	testRegistry.Add( isExistTest, FRL_STR("Is exist registry key."), frl::True );
	testRegistry.Add( createAndDeleteTest, FRL_STR("Create and delete registry key.") );
	testRegistry.Add( createFullPathAndRecurciveDelete, FRL_STR("Create full path and recurcive delete registry key.") );
	testRegistry.Add( isExistValueTest, FRL_STR("Is exist value?") );
	testRegistry.Add( DWORDValueTest, FRL_STR("DWORD registry value.") );
	testRegistry.Add( DWORDDefaultValueTest, FRL_STR("DWORD default registry value.") );
	testRegistry.Add( StringValueTest, FRL_STR("String registry value.") );
	testRegistry.Add( StringDefaultValueTest, FRL_STR("String default registry value.") );
	testRegistry.Add( MultiStringValueTest, FRL_STR("MultiString registry value.") );
	testRegistry.Add( MultiStringDefaultValueTest, FRL_STR("MultiString default registry value.") );
	testRegistry.Add( BinaryValueTest, FRL_STR("Binary registry value.") );
	testRegistry.Add( BinaryDefaultValueTest, FRL_STR("Binary default registry value.") );
	testRegistry.Add( GetNumSubKeysTest, FRL_STR("Get number subkeys.") );
	testRegistry.Add( GetNumSubvaluesTest, FRL_STR("Get number subvalues.") );
	return 0;
}
