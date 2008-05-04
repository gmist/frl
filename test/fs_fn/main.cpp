#include "frl_types.h"
#include "frl_io.h"
#include "frl_exception.h"
#include "console_std/frl_iostream.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "frl_unit_test.h"


frl::Bool TestCreate( void )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "Create - removal operations" );
	try
	{
		create( fileName );	
		removal( fileName );
		return frl::True;
	}
	catch ( frl::Exception &ex )
	{
		frl::console_std::Out << ex.getFullDescription() << std::endl;
		return frl::False;	
	}	
}

frl::Bool TestIsExist( void )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "IsExist operation" );
	try
	{
		create( fileName );
		frl::Bool isExist = frl::io::fs::isExist( fileName );
		removal( fileName  );
		return isExist;
	}
	catch ( frl::Exception &ex )
	{
		frl::console_std::Out << ex.getFullDescription() << std::endl;
		return frl::False;
	}		
}

frl::Bool TestWrite( void )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "TestWriteOperation" );

	try
	{
		create( fileName );
		FileDescriptor file;
		open( file, fileName, openWriteOnly );		
		frl::Char data[] = FRL_STR("qwertyuiop");
		FileRWCount count = write( file, data, sizeof( data )-1 );
		close( file );
		removal( fileName );
		if( count != sizeof( data )-1 )
			return frl::False;
		return frl::True;
	}
	catch ( frl::Exception &ex )
	{
		frl::console_std::Out << ex.getFullDescription() << std::endl;
		return frl::False;
	}
}

frl::Bool TestRead( void )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "TestReadOperation" );

	try
	{
		create( fileName );	
		FileDescriptor file;
		open( file, fileName, openWriteOnly );
		frl::Char data[] = FRL_STR( "qwertyuiop" );
		FileRWCount count = write( file, data, sizeof( data )   );
		close( file );

		if( count !=  sizeof( data )   )
			return frl::False;

		open( file, fileName, openReadOnly );
		
		frl::Char *buffer = new frl::Char[  ( sizeof( data ) ) / 2];		

		count = read( file, buffer,  sizeof( data )   );
		close( file );
		removal( fileName );
		if( count != sizeof( data )  )
		{
			delete [] buffer;
			return frl::False;
		}

		if( frl::String( data ) != frl::String( buffer ) )
		{
			delete [] buffer;
			return frl::False;
		}
		delete [] buffer;
		return frl::True;
	}
	catch ( frl::Exception &ex )
	{
		if ( isExist( fileName ))
			removal( fileName );
		frl::console_std::Out << ex.getFullDescription() << std::endl;
		return frl::False;
	}	
}

frl::Bool TestLength( void )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "TestLengthOperation" );
	try
	{
		create( fileName );		
		FileDescriptor file;
		open( file, fileName, openReadWrite );
		FileOffset length = frl::io::fs::length( file );
		if( length != 0 )
		{
			close( file );
			removal( fileName );
			return frl::False;
		}
		FileRWCount count = write( file, FRL_STR("qwerty"), 6 );
		close( file );
		if( count != 6 )
		{
			removal( fileName );
			return frl::False;
		}
		open( file, fileName, openReadWrite );		
		length = frl::io::fs::length( file );
		close( file );
		removal( fileName );
		if( length != 6 )
			return frl::False;
		return frl::True;
	}
	catch ( frl::Exception &ex )
	{
		if ( isExist( fileName ))
			removal( fileName );
		frl::console_std::Out << ex.getFullDescription() << std::endl;
		return frl::False;
	}	
}

frl::Bool TestSeek( void )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "TestSeekOperation" );
	try
	{
		create( fileName );		
		FileDescriptor file;
		open( file, fileName, openReadWrite );
		if( file == InvalidFileDescriptor )
			return frl::False;
		frl::Char data[] = FRL_STR( "qwertyuiop" );
		FileRWCount count = write( file, data, sizeof( data )  );
		if( count != sizeof( data )  )
		{
			close( file );
			removal( fileName );
			return frl::False;
		}
		FileOffset startPos = seek( file, 0, seekFromStart );
		frl::Char *buffer = new frl::Char[ sizeof( data ) / 2 ];
		count = read( file, buffer, sizeof( data )  );
		close( file );
		removal( fileName );
		if( count != sizeof( data )  )
		{
			delete [] buffer;
			return frl::False;
		}

		frl::String bufferStr = buffer;
		delete [] buffer;
		if( ( frl::String( data ) != bufferStr ) || startPos != 0 )
			return frl::False;
		return frl::True;
	}
	catch ( frl::Exception &ex )
	{
		frl::console_std::Out << ex.getFullDescription() << std::endl;
		return frl::False;
	}
	
}

frl::Bool TestTell( void )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "TestTellOperation" );
	create( fileName );
		
	FileDescriptor file;
	open( file, fileName );
	if( file == InvalidFileDescriptor )
		return frl::False;
	FileOffset startPos = tell( file );
	if( startPos != 0 )
		return frl::False;

	frl::Char data[] = FRL_STR( "qwertyuiop" );
	FileRWCount count = write( file, data, sizeof( data ) - 1 );
	FileOffset endPos = tell( file );
	startPos = seek( file, 5, seekFromStart );
	FileOffset FivePos = tell( file );
	close( file );
	removal( fileName );
	if( ( count != sizeof( data ) - 1 ) || ( endPos != sizeof( data ) - 1 ) || ( startPos != 5 ) || ( FivePos != 5 ) )
		return frl::False;
	return frl::True;
}

frl::Bool TestIsEof( void )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "TestIsEofOperation" );

	create( fileName );
		
	FileDescriptor file;
	open( file, fileName );
	if( file == InvalidFileDescriptor )
		return frl::False;
	seekToEnd( file );
	frl::Bool flag = isEof( file );
	close( file );
	removal( fileName );
	return flag;
}

frl::Bool TestTruncate( void )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "TestTruncateOperation" );
	create( fileName );		
	FileDescriptor file;
	open( file, fileName );
	if( file == InvalidFileDescriptor )
		return frl::False;
	frl::Char data[] = FRL_STR( "qwertyuiop" );
	FileRWCount count = write( file, data, sizeof( data ) - 1 );
	if( count != sizeof( data ) - 1 )
	{
		close( file );
		removal( fileName );
		return frl::False;
	}
	truncate( file, 5 );
	FileOffset length = frl::io::fs::length( file );
	close( file );
	removal( fileName );
	return length == 5;
}

frl::Bool TestRename( void )
{
	using namespace frl::io::fs;
	frl::String fileNameSrc = FRL_STR( "TestRenameOperatioSrc" );
	frl::String fileNameDst = FRL_STR( "TestRenameOperationDst" );
	try
	{
		create( fileNameSrc );
		FileDescriptor file;
		open( file, fileNameSrc );		
		frl::Char data[] = FRL_STR( "qwertyuiop" );
		FileRWCount count = write( file, data, sizeof( data ) - 1 );
		close( file );
		if( count != sizeof( data ) - 1 )
		{
			removal( fileNameSrc );
			return frl::False;
		}
		rename( fileNameSrc, fileNameDst );		
		if( ! isExist( fileNameDst ) )
			return frl::False;
		FileOffset length = frl::io::fs::length( fileNameDst );
		removal( fileNameDst );
		return ( length == sizeof( data ) -1 );
	}
	catch ( frl::Exception &ex )
	{
		if( isExist( fileNameSrc) )
			removal( fileNameSrc );
		frl::console_std::Out << ex.getFullDescription() << std::endl;
		return frl::False;
	}	

}

frl::Bool TestMove( void )
{
	using namespace frl::io::fs;
	frl::String fileNameSrc = FRL_STR( "TestMoveOperatioSrc" );
	frl::String fileNameDst = FRL_STR( "TestMoveOperationDst" );

	create( fileNameSrc );
		
	FileDescriptor file;
	open( file, fileNameSrc );
	if( file == InvalidFileDescriptor )
		return frl::False;
	frl::Char data[] = FRL_STR( "qwertyuiop" );
	FileRWCount count = write( file, data, sizeof( data ) - 1 );
	close( file );
	if( count != sizeof( data ) - 1 )
	{
		removal( fileNameSrc );
		return frl::False;
	}

	try
	{
		move( fileNameSrc, fileNameDst );
	}
	catch ( frl::Exception &ex )
	{
		removal( fileNameSrc );
		if( isExist( fileNameDst ) )
			removal( fileNameDst );
		frl::console_std::Out << ex.getFullDescription() << std::endl;		
		return frl::False;
	}
	
	if( ! isExist( fileNameDst ) )
		return frl::False;

	FileOffset length = frl::io::fs::length( fileNameDst );
	removal( fileNameDst );

	return ( ( length == sizeof( data ) - 1 ) && ! isExist( fileNameSrc ) );
}

frl::Bool TestCopy( void )
{
	using namespace frl::io::fs;
	frl::String fileNameSrc = FRL_STR( "TestMoveOperatioSrc" );
	frl::String fileNameDst = FRL_STR( "TestMoveOperationDst" );

	create( fileNameSrc );
		
	FileDescriptor file;
	open( file, fileNameSrc );
	if( file == InvalidFileDescriptor )
		return frl::False;
	frl::Char data[] = FRL_STR( "qwertyuiop" );
	FileRWCount count = write( file, data, sizeof( data ) - 1 );
	close( file );
	if( count != sizeof( data ) - 1 )
	{
		removal( fileNameSrc );
		return frl::False;
	}

	try
	{
		copy( fileNameSrc, fileNameDst );
	}
	catch ( frl::Exception &ex )
	{
		removal( fileNameSrc );
		if( isExist( fileNameDst ) )
			removal( fileNameDst );
		frl::console_std::Out << ex.getFullDescription() << std::endl;
		return frl::False;
	}

	if( ! isExist( fileNameSrc ) )
	{
		if( isExist( fileNameDst ) )
			removal( fileNameDst );
		return frl::False;
	}

	if( ! isExist( fileNameDst ) )
	{
		removal( fileNameSrc );
		return frl::False;
	}

	FileOffset length1 = length( fileNameSrc );
	FileOffset length2 = length( fileNameDst );

	removal( fileNameSrc );
	removal( fileNameDst );

	return length1 == length2;
}

frl::Bool TestHardLink( void )
{
	using namespace frl::io::fs;
	frl::String fileNameSrc = FRL_STR( "TestHardLinkOperatioSrc" );
	frl::String fileNameDst = FRL_STR( "TestHardLinkOperationDst" );

	create( fileNameSrc );		

	FileDescriptor file;
	open( file, fileNameSrc );
	if( file == InvalidFileDescriptor )
		return frl::False;

	frl::Char data[] = FRL_STR( "qwertyuiop" );
	FileRWCount count = write( file, data, sizeof( data ) - 1 );
	close( file );
	if( count != sizeof( data ) - 1 )
	{
		removal( fileNameSrc );
		return frl::False;
	}

	try
	{
		hardLink( fileNameSrc, fileNameDst );
	}
	catch ( frl::Exception &ex )
	{
		removal( fileNameSrc );
		if( isExist( fileNameDst ) )
			removal( fileNameDst );
		frl::console_std::Out << ex.getFullDescription() << std::endl;
		return frl::False;
	}	

	removal( fileNameSrc );

	if( ! isExist( fileNameDst ) )
		return frl::False;

	removal( fileNameDst );

	return frl::True;
}

frl::Bool TestSymLink( void )
{
	using namespace frl::io::fs;
	frl::String fileNameSrc = FRL_STR( "TestSymLinkOperatioSrc" );
	frl::String fileNameDst = FRL_STR( "TestSymLinkOperationDst" );

	create( fileNameSrc );		

	FileDescriptor file;
	open( file, fileNameSrc );
	if( file == InvalidFileDescriptor )
		return frl::False;

	frl::Char data[] = FRL_STR( "qwertyuiop" );
	FileRWCount count = write( file, data, sizeof( data ) - 1 );
	close( file );
	if( count != sizeof( data ) - 1 )
	{
		removal( fileNameSrc );
		return frl::False;
	}

	try
	{
		symLink( fileNameSrc, fileNameDst );
	}
	catch ( frl::Exception &ex )
	{
		removal( fileNameSrc );
		if( isExist( fileNameDst ) )
			removal( fileNameDst );
		frl::console_std::Out << ex.getFullDescription() << std::endl;
		return frl::False;	
	}

	removal( fileNameSrc );

	if( ! isExist( fileNameDst ) )
		return frl::False;

	removal( fileNameDst );

	return frl::True;
}

frl::Bool TestIsIdentical( void )
{
	using namespace frl::io::fs;
	frl::String fileName = FRL_STR( "TestIsIdentical" );

	create( fileName );
		
	FileDescriptor file;
	open( file, fileName );
	if( file == InvalidFileDescriptor )
		return frl::False;
	write( file, FRL_STR( "qwerty" ), 6 );
	close( file );

	frl::Bool flag = isIdentical( fileName, fileName );
	removal( fileName );

	return flag;
}

frl::Bool TestConcat( void )
{
	using namespace frl::io::fs;
	frl::String fileName1 = FRL_STR( "TestConcatenate1" );
	frl::String fileName2 = FRL_STR( "TestConcatenate2" );
	frl::String fileNameDst = FRL_STR( "TestConcanateDst" );

	create( fileName1 );		

	FileDescriptor file;
	open( file, fileName1 );
	if( file == InvalidFileDescriptor )
		return frl::False;
	frl::String text = FRL_STR( "qwerty" );
	FileRWCount count = write( file, text.c_str(), static_cast< FileRWCount >( text.size() ) );
	close( file );
	if( count != text.size() )
	{
		removal( fileName1 );
		return frl::False;
	}
	
	copy( fileName1, fileName2 );		

	create( fileNameDst );
		
	concatenate( fileName1, fileName2, fileNameDst );
	removal( fileName1 );
	removal( fileName2 );
	FileOffset length = frl::io::fs::length( fileNameDst );
	removal( fileNameDst );
	if( (size_t)length != text.size() * 2 )
		return frl::False;
	return frl::True;
}

int main( int argc, char *argv[] )
{

	try
	{
		frl::unit_test::ConsoleUnitTest FileTest( FRL_STR( " File IO operations" ) );

		FileTest.Add( TestCreate, FRL_STR( "Create - removal file" ) );
		FileTest.Add( TestIsExist, FRL_STR( "isExist operation" ) );
		FileTest.Add( TestWrite, FRL_STR( "Write to file" ) );
		FileTest.Add( TestRead, FRL_STR( "Read from file" ) );
		FileTest.Add( TestLength, FRL_STR( "Get length operation" ) );
		FileTest.Add( TestSeek, FRL_STR( "Seek operation" ) );
		FileTest.Add( TestTell, FRL_STR( "Tell operation" ) );
		FileTest.Add( TestIsEof, FRL_STR( "IsEof operation" ) );
		FileTest.Add( TestTruncate, FRL_STR( "truncate operation" ) );
		FileTest.Add( TestRename, FRL_STR( "rename operation" ) );
		FileTest.Add( TestMove, FRL_STR( "move operation" ) );
		FileTest.Add( TestCopy, FRL_STR( "copy operation" ) );
		FileTest.Add( TestHardLink, FRL_STR( "hardLink operation" ) );
		FileTest.Add( TestSymLink, FRL_STR( "symLink operation" ) );
		FileTest.Add( TestIsIdentical, FRL_STR( "isIdentical operation" ) );
		FileTest.Add( TestConcat, FRL_STR( "concatenate operation" ) );
	}
	catch ( frl::Exception &ex )
	{
		frl::console_std::Out << ex.getFullDescription() << std::endl;
	}

	return 0;
}
