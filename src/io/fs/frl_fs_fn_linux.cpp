#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_LINUX )

#include "io/fs/frl_fs_fn.h"
#include "frl_types.h"
#include "frl_string.h"
#include <errno.h>

namespace frl{ namespace io{ namespace fs{

void close( frl::io::fs::Descriptor file )
{
	if( file != InvalidDescriptor )
		::close( file );
}

frl::Bool isExist( const frl::String &fileName )
{
	return access( fileName, accessExist );
}

frl::Bool access( const frl::String &fileName, AccessMode mode )
{
	switch( mode )
	{
		case accessRead:
			return ::access( frl::multiByteCompatibility( fileName ).c_str(), R_OK ) == 0;
		case accessWrite:
			return ::access( frl::multiByteCompatibility( fileName ).c_str(), W_OK ) == 0;
		case accessReadWrite:
			return ::access( frl::multiByteCompatibility( fileName ).c_str(), R_OK | W_OK ) == 0;
		case accessExecute:
			return ::access( frl::multiByteCompatibility( fileName ).c_str(), X_OK ) == 0;
		case accessExist:
			return ::access( frl::multiByteCompatibility( fileName ).c_str(), F_OK ) == 0;
	}
	return frl::False;
}

// FIXME loss permissions at create file!!
Descriptor open( const frl::String &fileName, frl::UInt openMode, frl::UInt permMode )
{
	if( fileName.empty()  || openMode == openNotOpen || permMode <= 0)
		return InvalidDescriptor;

	frl::UInt flags = 0;

	switch( openMode & ( openReadOnly | openWriteOnly) )
	{
		case openReadOnly:
			flags=O_RDONLY;
			break;
		case openWriteOnly:
			flags=O_WRONLY;
			break;
		default:
			flags = O_RDWR;
	}

	if( openMode & openAppend )
		flags |= O_APPEND;

	if( openMode & openTruncate )
		flags |= O_TRUNC;

	if( openMode & openCreate )
	{
		flags |= O_CREAT;
		if( openMode & openExclusive )
			flags |= O_EXCL;
	}

	if( openMode & openNonBlocking )
		flags |= O_NONBLOCK;

	#ifdef O_BINARY
	if( openMode & openBinary )
		flags |= O_BINARY;
	#endif

	#ifdef O_TEXT
	if( openMode & openText )
		flags |= O_TEXT;
	#endif

	#ifdef O_LARGEFILE
	if( openMode & openLargeFile )
		flags |= O_LARGEFILE;
	#endif

	Descriptor file = ::open( multiByteCompatibility( fileName ).c_str(), flags, permMode );
	if( file < 0 )
		return InvalidDescriptor;
	if(openMode & openAppend)					
		seekToEnd( descriptor );
	return file;
}

frl::Bool create( const frl::String &fileName, frl::UInt permMode )
{
	if( permMode <= 0  )
		return frl::False;

	Descriptor file =open( fileName, frl::io::fs::openCreate |  frl::io::fs::openWriteOnly | openTruncate, permMode );
	if( file == InvalidDescriptor )
	{
		return frl::False;
	}
	close( file );
	return frl::True;
}

Offset seek( Descriptor file, Offset new_position, SeekMode mode )
{
	if( file == InvalidDescriptor || new_position < 0 )
		return InvalidOffset;
	switch( mode )
	{
		case seekFromStart:
			return lseek( file, new_position, SEEK_SET );
		case seekFromEnd:
			return lseek( file, new_position, SEEK_END );
		default:
			return lseek( file, new_position, SEEK_CUR );
	}
	return ::lseek( file, new_position, mode );
}

Offset tell( Descriptor file )
{
	return seek( file, 0, seekFromCurrent );
}

Offset length( Descriptor file )
{
	if( file == InvalidDescriptor )
		return InvalidOffset;
	Offset curPosition = tell( file );
	Offset length = seekToEnd( file );
	seek( file, curPosition, seekFromStart );
	return length;
}

Offset length( const frl::String &fileName )
{
	Descriptor file = open( fileName, openReadOnly );
	Offset result = length( file );
	close( file );
	return result;
}

Offset seekToStart( Descriptor file )
{
	return seek( file, 0, seekFromStart );
}

Offset seekToEnd( Descriptor file )
{
	return seek( file, 0, seekFromEnd );
}

frl::Bool isEof( Descriptor file )
{
	if( file == InvalidDescriptor )
		return frl::True;
	register Offset curPosition = tell( file );
	return (  0<= curPosition ) && ( length( file ) <= curPosition );
}

frl::Bool flush( Descriptor file )
{
	if( file == InvalidDescriptor )
		return frl::False;
	if( ::fsync( file ) == -1 )
		return frl::False;
	return frl::True;
}

RWCount write( Descriptor file, const void *data, RWCount count )
{
	if( file == InvalidDescriptor || data == NULL || count < 0 )
		return InvalidRWCount;
	if( count == 0 )
		return frl::True;
	return ::write( file, data, count );
}

RWCount read( Descriptor file, void *buffer, RWCount count )
{
	if( file == InvalidDescriptor || count < 0 )
		return InvalidRWCount;
	if( count == 0 )
		return frl::True;
	return ::read( file, buffer, count );
}

frl::Bool truncate( Descriptor file, Offset newLength )
{
	if( file == InvalidDescriptor )
		return frl::False;
	return ::ftruncate( file, newLength ) == 0;
}

frl::Bool removal( const frl::String &fileName )
{
	return ::unlink( frl::multiByteCompatibility( fileName ).c_str() ) == 0;
}

frl::Bool rename( const frl::String &fileNameSrc, const frl::String &fileNameDst )
{
	if( fileNameSrc == fileNameDst )
		return frl::False;
	return ::rename( multiByteCompatibility( fileNameSrc ).c_str(), multiByteCompatibility( fileNameDst ).c_str() ) == 0;
}

// FIXME this code not work!!!
frl::Bool hardLink( const frl::String &fileName, const frl::String &linkName )
{
	if( fileName == linkName )
		return frl::False;
	return ::link( multiByteCompatibility( fileName ).c_str(), multiByteCompatibility( fileName ).c_str() ) == 0;
}

// FIXME this code not work!!!
frl::Bool symLink( const frl::String &fileName, const frl::String &linkName )
{
	if( fileName == linkName )
		return frl::False;
	return ::symlink( multiByteCompatibility( fileName ).c_str(), multiByteCompatibility( fileName ).c_str() ) == 0;
}

frl::Bool copy( const frl::String &fileNameSrc, const frl::String &fileNameDst, frl::Bool overwrite )
{
	struct DescriptorManager
	{
		Descriptor fileSrc;
		Descriptor fileDst;
		DescriptorManager( void )
			:	fileSrc( InvalidDescriptor ),
				fileDst( InvalidDescriptor )
		{
		}

		~DescriptorManager( void )
		{
			close( fileSrc );
			close( fileDst );
		}
	}; // struct DescriptorManger


	if( ! isExist( fileNameSrc ) )
		return frl::False;

	if( isExist( fileNameDst ) && ( ! overwrite ) )
		return frl::False;

	Stat stat_;
	if( ! getStat( fileNameSrc, stat_ ) )
		return False;

	if( stat_.permissionMode & permDirectory )
		return False; // FIXME mrdir implement!

	DescriptorManager dm;
	dm.fileSrc = open( fileNameSrc, openReadOnly );
	if( dm.fileSrc == InvalidDescriptor )
		return frl::False;

	dm.fileDst = open( fileNameDst, openCreate | openTruncate | openWriteOnly, stat_.permissionMode );
	if( dm.fileDst == InvalidDescriptor )
		return frl::False;

	frl::UChar buffer[4096];
	RWCount readCount = InvalidRWCount;
	RWCount writeCount = InvalidRWCount;
	while( 1 )
	{
		readCount = read( dm.fileSrc, buffer, sizeof(buffer) );
		if( readCount < 0 )
			return frl::False;

		if( readCount == 0 )
			break;
		writeCount = write( dm.fileDst, buffer, readCount );
		if( writeCount < 0)
			return frl::False;
	}
	return frl::True;
}

frl::Bool move( const frl::String &fileNameSrc, const frl::String &fileNameDst, frl::Bool overwrite )
{
	if( ! copy( fileNameSrc, fileNameDst, overwrite ) )
		return frl::False;
	return removal( fileNameSrc );
}

frl::Bool isIdentical( const String &fileName1, const String &fileName2 )
{
	struct stat stat1, stat2;
	return ( 	!::lstat( multiByteCompatibility( fileName1 ).c_str(), &stat1) &&
					!::lstat( multiByteCompatibility( fileName2 ).c_str(), &stat2) &&
					stat1.st_ino==stat2.st_ino &&
					stat1.st_dev==stat2.st_dev
				);
}

frl::Bool concatenate( const String &fileName1, const String &fileName2, const String &fileNameDst, Bool overwrite )
{
	if( ! isExist( fileName1 ) || ! isExist( fileName2 ) )
		return False;

	struct DescriptorManager
	{
		Descriptor file1;
		Descriptor file2;
		Descriptor fileDst;

		DescriptorManager( void )
			:	file1( InvalidDescriptor ),
				file2( InvalidDescriptor ),
				fileDst( InvalidDescriptor )
		{
		}
		~DescriptorManager()
		{
			close( file1 );
			close( file2 );
			close( fileDst );
		}

	};
	DescriptorManager dm;

	dm.file1 = open( fileName1, openReadOnly );
	if( dm.file1 == InvalidDescriptor )
		return False;

	dm.file2 = open( fileName2, openReadOnly );
	if( dm.file2 == InvalidDescriptor )
		return False;

	if( isExist( fileNameDst ) )
	{
		if( overwrite )
		{
			removal( fileNameDst );
			create( fileNameDst );
			dm.fileDst = open( fileNameDst, openWriteOnly );
			if( dm.fileDst == InvalidDescriptor )
				return False;
		}
		else
		{
			dm.fileDst = open( fileNameDst, openWriteOnly );
			if( dm.fileDst == InvalidDescriptor )
				return False;
			seekToEnd( dm.fileDst );
		}
	}
	else
	{
		create( fileNameDst );
		dm.fileDst = open( fileNameDst, openWriteOnly );
		if( dm.fileDst == InvalidDescriptor )
			return False;
	}

	UChar buffer[4096];
	RWCount countWrite, countRead;
	while( True )
	{
		countRead = read( dm.file1, buffer, sizeof(buffer) );
		if( countRead < 0 )
			return False;

		if( countRead == 0 )
			break;

		countWrite = write( dm.fileDst, buffer, countRead );
		if( countWrite < 0 )
			return False;
	}
	while( True )
	{
		countRead = read( dm.file2, buffer, sizeof(buffer) );
		if( countRead < 0 )
			return False;

		if( countRead == 0)
			break;

		countWrite = write( dm.fileDst, buffer, countRead );
		if( countWrite < 0)
			return False;
	}
	return True;
}

Bool getStat( const String & fileName, Stat &stat_ )
{
	if( ! isExist( fileName ) )
		return False;

	struct stat data;
	if( ::stat( multiByteCompatibility( fileName ).c_str(), &data ) != 0 )
		return False;

	const time::Time seconds = 1000000000;

	stat_.permissionMode= static_cast<UInt>( data.st_mode );
	stat_.uid = data.st_uid;
	stat_.gid = data.st_gid;
	stat_.accessTime = data.st_atime*seconds;
	stat_.modifyTime = data.st_mtime*seconds;
	stat_.createTime = data.st_ctime*seconds;
	stat_.length = static_cast< Offset >( data.st_size );
	return True;
}

Bool setStat( const String &fileName, UInt permMode )
{
	if( ! isExist( fileName ) )
		return False;
	return ::chmod( multiByteCompatibility( fileName ).c_str(), permMode ) == 0;
}

String getCurrentDirectory()
{
	return FRL_STR( "" ); // TODO fix me
}

void addSlashToEndPath( String &path )
{
	String::size_type length = path.size();
	if( length < 2 )
		return;
	if( path[length-1] == FRL_STR( '//' ) )
		return;
	path += FRL_STR('//');
}

} // namespace fs
} // namespace io
} // FatRat Library

#endif // FRL_PLATFORM_LINUX
