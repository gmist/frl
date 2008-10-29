#ifndef fs_fn_test_suite_h_
#define fs_fn_test_suite_h_
#include <boost/test/unit_test.hpp>
#include "frl_io.h"

BOOST_AUTO_TEST_SUITE( fs_fn )

BOOST_AUTO_TEST_CASE( file_create_and_removal )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "Create - removal operations" );

	BOOST_CHECK_NO_THROW( create( fileName ) );
	BOOST_CHECK_NO_THROW( removal( fileName ) );
}

BOOST_AUTO_TEST_CASE( is_exist_file )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "IsExist operation" );

	BOOST_CHECK_NO_THROW( create( fileName ) );
	BOOST_CHECK( frl::io::fs::isExist( fileName ) );
	BOOST_CHECK_NO_THROW( removal( fileName  ) );
}

BOOST_AUTO_TEST_CASE( write_operation )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "TestWriteOperation" );

	BOOST_CHECK_NO_THROW( create( fileName ) );
	FileDescriptor file = InvalidFileDescriptor;
	BOOST_CHECK_NO_THROW( open( file, fileName, openWriteOnly ) );
	BOOST_CHECK_NE( file, InvalidFileDescriptor );

	frl::Char data[] = FRL_STR("qwertyuiop");
	FileRWCount count = 0;
	BOOST_CHECK_NO_THROW( count = write( file, data, sizeof( data ) - 1 ) );
	BOOST_CHECK_NO_THROW( close( file ) );
	BOOST_CHECK_NO_THROW( removal( fileName ) );

	BOOST_CHECK_EQUAL( count, sizeof( data ) - 1 );
}

BOOST_AUTO_TEST_CASE( read_operation )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "TestReadOperation" );

	BOOST_CHECK_NO_THROW( create( fileName ) );
	FileDescriptor file = InvalidFileDescriptor;
	BOOST_CHECK_NO_THROW( open( file, fileName, openWriteOnly ) );
	BOOST_CHECK_NE( file, InvalidFileDescriptor );

	frl::Char data[] = FRL_STR( "qwertyuiop" );
	FileRWCount count = 0;
	BOOST_CHECK_NO_THROW( count = write( file, data, sizeof( data ) ) );
	BOOST_CHECK_NO_THROW( close( file ) );
	BOOST_CHECK_EQUAL( count, sizeof( data ) );

	BOOST_CHECK_NO_THROW( open( file, fileName, openReadOnly ) );
	frl::Char *buffer = new frl::Char[  ( sizeof( data ) ) / 2];		
	BOOST_CHECK_NO_THROW( count = read( file, buffer,  sizeof( data ) ) );
	BOOST_CHECK_NO_THROW( close( file ) );
	BOOST_CHECK_NO_THROW( removal( fileName ) );

	BOOST_CHECK_EQUAL( count, sizeof( data ) );
	BOOST_CHECK( frl::String( data ) == frl::String( buffer ) );
	delete [] buffer;
}

BOOST_AUTO_TEST_CASE( test_length )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "TestLengthOperation" );

	BOOST_CHECK_NO_THROW( create( fileName ) );
	FileDescriptor file = InvalidFileDescriptor;
	BOOST_CHECK_NO_THROW( open( file, fileName, openReadWrite ) );
	BOOST_CHECK_NE( file, InvalidFileDescriptor );

	FileOffset length = 0;
	BOOST_CHECK_NO_THROW( length = frl::io::fs::length( file ) );
	BOOST_CHECK_EQUAL( length, 0 );

	FileRWCount count = 0;
	BOOST_CHECK_NO_THROW( count = write( file, FRL_STR("qwerty"), 6 ) );
	BOOST_CHECK_NO_THROW( close( file ) );
	BOOST_CHECK_EQUAL( count, static_cast< FileRWCount >( 6 ) );

	BOOST_CHECK_NO_THROW( open( file, fileName, openReadWrite ) );
	BOOST_CHECK_NO_THROW( length = frl::io::fs::length( file ) );
	BOOST_CHECK_NO_THROW( close( file ) );
	BOOST_CHECK_NO_THROW( removal( fileName ) );

	BOOST_CHECK_EQUAL( length, 6 );
}

BOOST_AUTO_TEST_CASE( test_seek )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "TestSeekOperation" );

	BOOST_CHECK_NO_THROW( create( fileName ) );
	FileDescriptor file = InvalidFileDescriptor;
	BOOST_CHECK_NO_THROW( open( file, fileName, openReadWrite ) );
	BOOST_CHECK_NE( file, InvalidFileDescriptor );

	frl::Char data[] = FRL_STR( "qwertyuiop" );
	FileRWCount count = 100;
	BOOST_CHECK_NO_THROW( count = write( file, data, sizeof( data ) ) );
	BOOST_CHECK_EQUAL( count, sizeof( data ) );
	FileOffset startPos = 0;
	BOOST_CHECK_NO_THROW( startPos = seek( file, 0, seekFromStart ) );
	frl::Char *buffer = new frl::Char[ sizeof( data ) / 2 ];
	BOOST_CHECK_NO_THROW( count = read( file, buffer, sizeof( data ) ) );
	BOOST_CHECK_NO_THROW( close( file ) );
	BOOST_CHECK_NO_THROW( removal( fileName ) );
	BOOST_CHECK_EQUAL( count, sizeof( data ) );
	frl::String bufferStr = buffer;
	delete [] buffer;
	BOOST_CHECK( frl::String( data ) == bufferStr );
	BOOST_CHECK( startPos == 0 );
}

BOOST_AUTO_TEST_CASE( test_tell )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "TestTellOperation" );
	BOOST_CHECK_NO_THROW( create( fileName ) );
	FileDescriptor file = InvalidFileDescriptor;
	BOOST_CHECK_NO_THROW( open( file, fileName ) );
	BOOST_CHECK_NE( file, InvalidFileDescriptor );

	FileOffset startPos = 100;
	BOOST_CHECK_NO_THROW( startPos = tell( file ) );
	BOOST_CHECK_EQUAL( startPos, 0 );

	frl::Char data[] = FRL_STR( "qwertyuiop" );
	FileRWCount count = 0;
	BOOST_CHECK_NO_THROW( count = write( file, data, sizeof( data ) - 1 ) );
	FileOffset endPos = 102;
	BOOST_CHECK_NO_THROW( endPos = tell( file ) );
	BOOST_CHECK_NO_THROW( startPos = seek( file, 5, seekFromStart ) );

	FileOffset fivePos = 104;
	BOOST_CHECK_NO_THROW( fivePos = tell( file ) );
	BOOST_CHECK_NO_THROW( close( file ) );
	BOOST_CHECK_NO_THROW( removal( fileName ) );

	BOOST_CHECK_EQUAL( count, sizeof( data ) - 1 );
	BOOST_CHECK_EQUAL( endPos, sizeof( data ) - 1 );
	BOOST_CHECK_EQUAL( startPos, 5 );
	BOOST_CHECK_EQUAL( fivePos, 5 );
}

BOOST_AUTO_TEST_CASE( test_is_eof )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "TestIsEofOperation" );

	BOOST_CHECK_NO_THROW( create( fileName ) );
	FileDescriptor file = InvalidFileDescriptor;
	BOOST_CHECK_NO_THROW( open( file, fileName ) );
	BOOST_CHECK_NE( file, InvalidFileDescriptor );

	BOOST_CHECK_NO_THROW( seekToEnd( file ) );
	frl::Bool flag = frl::False;
	BOOST_CHECK_NO_THROW( flag = isEof( file ) );
	BOOST_CHECK_NO_THROW( close( file ) );
	BOOST_CHECK_NO_THROW( removal( fileName ) );

	BOOST_CHECK( flag );
}

BOOST_AUTO_TEST_CASE( test_truncate )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "TestTruncateOperation" );
	BOOST_CHECK_NO_THROW( create( fileName ) );
	FileDescriptor file = InvalidFileDescriptor;
	BOOST_CHECK_NO_THROW( open( file, fileName ) );
	BOOST_CHECK_NE( file, InvalidFileDescriptor );

	frl::Char data[] = FRL_STR( "qwertyuiop" );
	FileRWCount count = 0;
	BOOST_CHECK_NO_THROW( count = write( file, data, sizeof( data ) - 1 ) );
	BOOST_CHECK_EQUAL( count, sizeof( data ) - 1 );
	BOOST_CHECK_NO_THROW( truncate( file, 5 ) );
	
	FileOffset length = 0;
	BOOST_CHECK_NO_THROW( length = frl::io::fs::length( file ) );
	BOOST_CHECK_NO_THROW( close( file ) );
	BOOST_CHECK_NO_THROW( removal( fileName ) );
	
	BOOST_CHECK_EQUAL( length, 5 );
}

BOOST_AUTO_TEST_CASE( test_rename )
{
	using namespace frl::io::fs;
	frl::String fileNameSrc = FRL_STR( "TestRenameOperatioSrc" );
	frl::String fileNameDst = FRL_STR( "TestRenameOperationDst" );

	BOOST_CHECK_NO_THROW( create( fileNameSrc ) );
	FileDescriptor file = InvalidFileDescriptor;
	BOOST_CHECK_NO_THROW( open( file, fileNameSrc ) );
	BOOST_CHECK_NE( file, InvalidFileDescriptor );

	frl::Char data[] = FRL_STR( "qwertyuiop" );
	FileRWCount count = 0;
	BOOST_CHECK_NO_THROW( count = write( file, data, sizeof( data ) - 1 ) );
	BOOST_CHECK_NO_THROW( close( file ) );
	BOOST_CHECK_EQUAL( count, sizeof( data ) - 1 );

	BOOST_CHECK_NO_THROW( rename( fileNameSrc, fileNameDst ) );
	BOOST_CHECK( isExist( fileNameDst) );
	BOOST_CHECK( ! isExist( fileNameSrc ) );

	FileOffset length = 0;
	BOOST_CHECK_NO_THROW( length = frl::io::fs::length( fileNameDst ) );
	BOOST_CHECK_NO_THROW( removal( fileNameDst ) );
	BOOST_CHECK_EQUAL( length, sizeof( data ) - 1 );
}

BOOST_AUTO_TEST_CASE( test_move )
{
	using namespace frl::io::fs;
	frl::String fileNameSrc = FRL_STR( "TestMoveOperatioSrc" );
	frl::String fileNameDst = FRL_STR( "TestMoveOperationDst" );

	BOOST_CHECK_NO_THROW( create( fileNameSrc ) );
	FileDescriptor file = InvalidFileDescriptor;
	BOOST_CHECK_NO_THROW( open( file, fileNameSrc ) );
	BOOST_CHECK_NE( file, InvalidFileDescriptor );

	frl::Char data[] = FRL_STR( "qwertyuiop" );
	FileRWCount count = 0;
	BOOST_CHECK_NO_THROW( count = write( file, data, sizeof( data ) - 1 ) );
	BOOST_CHECK_NO_THROW( close( file ) );
	BOOST_CHECK_EQUAL( count, sizeof( data ) - 1 );

	BOOST_CHECK_NO_THROW( move( fileNameSrc, fileNameDst ) );
	BOOST_CHECK( isExist( fileNameDst ) );
	FileOffset length = 0;
	BOOST_CHECK_NO_THROW( length= frl::io::fs::length( fileNameDst ) );
	BOOST_CHECK_NO_THROW( removal( fileNameDst ) );
	BOOST_CHECK_EQUAL( length, sizeof( data ) - 1 );
	BOOST_CHECK( ! isExist( fileNameSrc ) );
}

BOOST_AUTO_TEST_CASE( test_copy )
{
	using namespace frl::io::fs;
	frl::String fileNameSrc = FRL_STR( "TestMoveOperatioSrc" );
	frl::String fileNameDst = FRL_STR( "TestMoveOperationDst" );

	create( fileNameSrc );
	FileDescriptor file = InvalidFileDescriptor;
	open( file, fileNameSrc );
	BOOST_CHECK_NE( file, InvalidFileDescriptor );

	frl::Char data[] = FRL_STR( "qwertyuiop" );
	FileRWCount count = 0;
	BOOST_CHECK_NO_THROW( count = write( file, data, sizeof( data ) - 1 ) );
	BOOST_CHECK_NO_THROW( close( file ) );
	BOOST_CHECK_EQUAL( count, sizeof( data ) - 1 );
	BOOST_CHECK_NO_THROW( copy( fileNameSrc, fileNameDst ) );
	BOOST_CHECK( isExist( fileNameSrc ) );
	BOOST_CHECK( isExist( fileNameDst ) );

	FileOffset length1 = 0;
	BOOST_CHECK_NO_THROW( length1 = length( fileNameSrc ) );
	FileOffset length2 = 0;
	BOOST_CHECK_NO_THROW( length2 = length( fileNameDst ) );

	BOOST_CHECK_NO_THROW( removal( fileNameSrc ) );
	BOOST_CHECK_NO_THROW( removal( fileNameDst ) );

	BOOST_CHECK_EQUAL( length1, length2 );
}

BOOST_AUTO_TEST_CASE( test_hard_link )
{
	using namespace frl::io::fs;
	frl::String fileNameSrc = FRL_STR( "TestHardLinkOperatioSrc" );
	frl::String fileNameDst = FRL_STR( "TestHardLinkOperationDst" );

	BOOST_CHECK_NO_THROW( create( fileNameSrc ) );
	FileDescriptor file = InvalidFileDescriptor;
	BOOST_CHECK_NO_THROW( open( file, fileNameSrc ) );
	BOOST_CHECK_NE( file, InvalidFileDescriptor );

	frl::Char data[] = FRL_STR( "qwertyuiop" );
	FileRWCount count = 0;
	BOOST_CHECK_NO_THROW( count = write( file, data, sizeof( data ) ) );
	BOOST_CHECK_EQUAL( count, sizeof( data ) );

	BOOST_CHECK_NO_THROW( hardLink( fileNameSrc, fileNameDst ) );

	frl::Char data1[] = FRL_STR( "write_test" );
	count = 0;
	BOOST_CHECK_NO_THROW( count = write( file, data1, sizeof( data1 ) ) );
	BOOST_CHECK_EQUAL( count, sizeof( data1 ) );

	BOOST_CHECK_NO_THROW( close( file ) );
	BOOST_CHECK_EQUAL( length( fileNameSrc ) , length( fileNameDst ) );
	BOOST_CHECK_NO_THROW( removal( fileNameSrc ) );

	BOOST_CHECK( isExist( fileNameDst ) );
	BOOST_CHECK_NO_THROW( removal( fileNameDst ) );
}

BOOST_AUTO_TEST_CASE( test_sym_link )
{
	using namespace frl::io::fs;
	frl::String fileNameSrc = FRL_STR( "TestSymLinkOperatioSrc" );
	frl::String fileNameDst = FRL_STR( "TestSymLinkOperationDst" );

	BOOST_CHECK_NO_THROW( create( fileNameSrc ) );
	FileDescriptor file = InvalidFileDescriptor;
	BOOST_CHECK_NO_THROW( open( file, fileNameSrc ) );
	BOOST_CHECK_NE( file, InvalidFileDescriptor );

	frl::Char data[] = FRL_STR( "qwertyuiop" );
	FileRWCount count = 0;
	BOOST_CHECK_NO_THROW( count = write( file, data, sizeof( data ) - 1 ) );
	BOOST_CHECK_NO_THROW( close( file ) );
	BOOST_CHECK_EQUAL( count, sizeof( data ) - 1 );

	#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
		BOOST_CHECK_THROW( symLink( fileNameSrc, fileNameDst ), frl::Exception );
	#else
		BOOST_CHECK_NO_THROW( symLink( fileNameSrc, fileNameDst ) );
	#endif

	BOOST_CHECK_NO_THROW( removal( fileNameSrc ) );

	#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
		BOOST_CHECK( ! isExist( fileNameDst ) );
	#else
		BOOST_CHECK( isExist( fileNameDst ) );
		removal( fileNameDst );
	#endif
}

BOOST_AUTO_TEST_CASE( test_is_identical )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "TestIsIdentical" );

	BOOST_CHECK_NO_THROW( create( fileName ) );
	FileDescriptor file = InvalidFileDescriptor;
	BOOST_CHECK_NO_THROW( open( file, fileName ) );
	BOOST_CHECK_NE( file, InvalidFileDescriptor );

	BOOST_CHECK_NO_THROW( write( file, FRL_STR( "qwerty" ), 6 ) );
	BOOST_CHECK_NO_THROW( close( file ) );

	BOOST_CHECK( isIdentical( fileName, fileName ) );
	BOOST_CHECK_NO_THROW( removal( fileName ) );
}

BOOST_AUTO_TEST_CASE( test_concate )
{
	using namespace frl::io::fs;
	frl::String fileName1 = FRL_STR( "TestConcatenate1" );
	frl::String fileName2 = FRL_STR( "TestConcatenate2" );
	frl::String fileNameDst = FRL_STR( "TestConcanateDst" );

	BOOST_CHECK_NO_THROW( create( fileName1 ) );	
	FileDescriptor file = InvalidFileDescriptor;
	BOOST_CHECK_NO_THROW( open( file, fileName1 ) );
	BOOST_CHECK_NE( file, InvalidFileDescriptor );

	frl::String text = FRL_STR( "qwerty" );
	FileRWCount count = 0;
	BOOST_CHECK_NO_THROW( count = write( file, text.c_str(), static_cast< FileRWCount >( text.size() ) ) );
	BOOST_CHECK_NO_THROW( close( file ) );
	BOOST_CHECK_EQUAL( count, text.length() );

	BOOST_CHECK_NO_THROW( copy( fileName1, fileName2 ) );
	BOOST_CHECK_NO_THROW( create( fileNameDst ) );

	BOOST_CHECK_NO_THROW( concatenate( fileName1, fileName2, fileNameDst ) );

	BOOST_CHECK_NO_THROW( removal( fileName1 ) );
	BOOST_CHECK_NO_THROW( removal( fileName2 ) );

	FileOffset length = 0;
	BOOST_CHECK_NO_THROW( length = frl::io::fs::length( fileNameDst ) );
	BOOST_CHECK_NO_THROW( removal( fileNameDst ) );
	BOOST_CHECK_EQUAL( length, text.length() * 2 );
}

BOOST_AUTO_TEST_SUITE_END()

#endif // fs_fn_test_suite_h_
