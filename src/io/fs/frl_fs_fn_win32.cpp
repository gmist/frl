#include "frl_platform.h"
#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <io.h>
#include "io/fs/frl_fs_fn.h"
#include "frl_string.h"
#include "frl_exception.h"

namespace frl
{
namespace io
{
namespace fs
{
frl::Bool access( const frl::String &fileName, AccessMode mode )
{
	FRL_EXCEPT_GUARD();
	switch( mode )
	{
	case accessRead:
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			#if( FRL_COMPILER == FRL_COMPILER_MSVC )
			return ::_waccess_s( fileName.c_str(), 4 ) == 0;
			#else
			return ::_waccess( fileName.c_str(), 4 ) == 0;
			#endif
		#else
			#if( FRL_COMPILER == FRL_COMPILER_MSVC )
			return ::_access_s( fileName.c_str(), 4 ) == 0;
			#else
			return ::_access( fileName.c_str(), 4 ) == 0;
			#endif
		#endif
		break;
	case accessWrite:
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			#if( FRL_COMPILER == FRL_COMPILER_MSVC )
			return ::_waccess_s( fileName.c_str(), 2 ) == 0;
			#else
			return ::_waccess( fileName.c_str(), 2 ) == 0;
			#endif
		#else
			#if( FRL_COMPILER == FRL_COMPILER_MSVC )
			return ::_access_s( fileName.c_str(), 2 ) == 0;
			#else
			return ::_access( fileName.c_str(), 2 ) == 0;
			#endif
		#endif
			break;
	case accessReadWrite:
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			#if( FRL_COMPILER == FRL_COMPILER_MSVC )
			return ::_waccess_s( fileName.c_str(), 6 ) == 0;
			#else
			return ::_waccess( fileName.c_str(), 6 ) == 0;
			#endif
		#else
			#if( FRL_COMPILER == FRL_COMPILER_MSVC )
			return ::_access_s( fileName.c_str(), 6 ) == 0;
			#else
			return ::_access( fileName.c_str(), 6 ) == 0;
			#endif
		#endif
		break;
	case accessExecute:
		/*#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			return ::_waccess_s( fileName.c_str(), X_OK ) == 0;
		#else
			return ::_access_s( fileName.c_str(), X_OK ) == 0;
		#endif*/
		break;
	case accessExist:
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			#if( FRL_COMPILER == FRL_COMPILER_MSVC )
			return ::_waccess_s( fileName.c_str(), 0 ) == 0;
			#else
			return ::_waccess( fileName.c_str(), 0 ) == 0;
			#endif
		#else
			#if( FRL_COMPILER == FRL_COMPILER_MSVC )
			return ::_access_s( fileName.c_str(), 0 ) == 0;
			#else
			return ::_access( fileName.c_str(), 0 ) == 0;
			#endif
		#endif
		break;
	}
	return frl::False;
}

void close( frl::io::fs::FileDescriptor &file)
{
	FRL_EXCEPT_GUARD();
	if ( file != InvalidFileDescriptor )
	{
		::CloseHandle( file );
		file = InvalidFileDescriptor;
	}
	else
		FRL_THROW( FRL_STR( "Invalid file handle." ) );
}

frl::Bool isExist( const frl::String &fileName )
{
	#ifdef UNICODE
		return ::GetFileAttributesW( fileName.c_str() ) != 0xffffffff;
	#else
		return ::GetFileAttributesA( fileName.c_str() ) != 0xffffffff;
	#endif
}

void open( FileDescriptor &FileDescriptor, const frl::String &fileName, frl::UInt openMode, frl::UInt permMode)
{
	FRL_EXCEPT_GUARD();
	DWORD flags=GENERIC_READ;
	DWORD creation=OPEN_EXISTING;

	// Basic access mode
	switch( openMode & ( openReadOnly | openWriteOnly ) )
	{
		case openReadOnly: flags = GENERIC_READ; break;
		case openWriteOnly: flags = GENERIC_WRITE; break;
		case openReadWrite: flags = GENERIC_READ | GENERIC_WRITE; break;
	}

	// Creation and truncation mode
	switch( openMode & (openCreate | openTruncate | openExclusive ) )
	{
		case openCreate: creation=OPEN_ALWAYS; break;
		case openTruncate: creation = TRUNCATE_EXISTING; break;
		case openCreate | openTruncate: creation = CREATE_ALWAYS; break;
		case openCreate | openTruncate | openExclusive: creation = CREATE_NEW; break;
	}
	if ( openMode & openNonBlocking )
	{
		// FIXME
	}

	#ifdef UNICODE
		FileDescriptor = ::CreateFileW( fileName.c_str(), flags, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, creation, FILE_ATTRIBUTE_NORMAL, NULL );
	#else
		FileDescriptor = ::CreateFileA( fileName.c_str(), flags, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, creation, FILE_ATTRIBUTE_NORMAL,NULL );
	#endif
	if ( FileDescriptor == InvalidFileDescriptor)
		FRL_THROW_SYSAPI( FRL_STR( "File \x22" ) + fileName + FRL_STR( "\x22 not opened." ) );

	if(openMode & openAppend)
		seekToEnd( FileDescriptor );
}

void create( const frl::String &fileName, frl::UInt permMode )
{
	FRL_EXCEPT_GUARD();
	FileDescriptor file;
	#ifdef UNICODE
		file = ::CreateFileW( fileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ,NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL,NULL );
	#else
		file = ::CreateFileA( fileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL,CREATE_NEW, FILE_ATTRIBUTE_NORMAL,NULL );
	#endif
	if( file == InvalidFileDescriptor )
		FRL_THROW_SYSAPI( FRL_STR( "File \x22" )+ fileName + FRL_STR( "\x22 not created."  ) );
	close( file );
}

FileOffset seek( FileDescriptor &file, FileOffset new_position, SeekMode mode )
{
	FRL_EXCEPT_GUARD();
	if ( file == InvalidFileDescriptor )
		FRL_THROW( FRL_STR( "Invalid file FileDescriptor." ) );
	if ( new_position < 0 )
		FRL_THROW( FRL_STR( "Invalid new position. New Position must be >= 0" ) );

	LARGE_INTEGER pos;
	pos.QuadPart=new_position;

	switch( mode )
	{
	case seekFromStart:
		pos.LowPart = ::SetFilePointer( file, pos.LowPart, &pos.HighPart, FILE_BEGIN );
		break;
	case seekFromEnd:
		pos.LowPart = ::SetFilePointer( file, pos.LowPart, &pos.HighPart, FILE_END );
		break;
	default:
		pos.LowPart = ::SetFilePointer( file, pos.LowPart, &pos.HighPart, FILE_CURRENT );
		break;
	}

	if( pos.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
		FRL_THROW_SYSAPI( FRL_STR( "Error move the file pointer." ) );
	return pos.QuadPart;
}

FileOffset tell( FileDescriptor &file )
{
	FRL_EXCEPT_GUARD();
	return seek( file, 0, seekFromCurrent );
}

FileOffset length( FileDescriptor &file )
{
	FRL_EXCEPT_GUARD();
	if( file == InvalidFileDescriptor )
		FRL_THROW( FRL_STR( "Invalid file FileDescriptor." ) );
	ULARGE_INTEGER result;
	result.LowPart = ::GetFileSize(file, &result.HighPart );
	if( result.LowPart  == INVALID_FILE_SIZE && GetLastError() != NO_ERROR )
		FRL_THROW_SYSAPI( FRL_STR( "Error get file size." ) );
	return result.QuadPart;
}

FileOffset length( const frl::String &fileName )
{
	FRL_EXCEPT_GUARD();
	FileDescriptor file;
	open( file, fileName, openReadOnly );
	FileOffset result = length( file );
	close( file );
	return result;
}

FileOffset seekToStart( FileDescriptor &file )
{
	FRL_EXCEPT_GUARD();
	return seek( file, 0, seekFromStart );
}

FileOffset seekToEnd( FileDescriptor &file )
{
	FRL_EXCEPT_GUARD();
	return seek( file, 0, seekFromEnd );
}

Bool isEof( FileDescriptor &file )
{
	FRL_EXCEPT_GUARD();
	if( file == InvalidFileDescriptor )
		FRL_THROW( FRL_STR( "Invalid file FileDescriptor." ) );
	register FileOffset pos = tell( file );
	return 0<=pos && length( file ) <=pos;
}

void flush( FileDescriptor &file )
{
	FRL_EXCEPT_GUARD();
	if( file == InvalidFileDescriptor )
		FRL_THROW( FRL_STR( "Invalid file FileDescriptor." ) );
	if( ::FlushFileBuffers( file ) == 0 )
		FRL_THROW_SYSAPI( FRL_STR( "Error flushing file buffers." ) );
}

void truncate( FileDescriptor &file, FileOffset newLength )
{
	FRL_EXCEPT_GUARD();
	if( file == InvalidFileDescriptor )
		FRL_THROW( FRL_STR( "Invalid file FileDescriptor." ) );
	FileOffset odlpos = tell( file );
	seek( file, newLength, seekFromStart );
	if( ! ::SetEndOfFile( file ) )
		FRL_THROW_SYSAPI( FRL_STR( "Error truncate file." ) );
	seek( file, odlpos, seekFromStart );
}

void removal( const frl::String &fileName )
{
	FRL_EXCEPT_GUARD();
	if ( !isExist( fileName ) )
		FRL_THROW( FRL_STR( "File \x22" ) + fileName + FRL_STR( "\x22 not exist." ) );
	#ifdef UNICODE
		 if ( ::DeleteFileW( fileName.c_str() )  == FALSE )
			FRL_THROW_SYSAPI( FRL_STR( "Error delete file \x22" ) + fileName + FRL_STR( "\x22." ) );
	#else
		if ( ::DeleteFileA( fileName.c_str() ) == FALSE )
			FRL_THROW_SYSAPI( FRL_STR( "Error delete file \x22" ) + fileName + FRL_STR( "\x22." ) );
	#endif
}

void rename( const frl::String &fileNameSrc, const frl::String &fileNameDst, Bool isOverWrite )
{
	FRL_EXCEPT_GUARD();
	if( !isExist( fileNameSrc ) )
		FRL_THROW( FRL_STR( "File \x22" ) + fileNameSrc + FRL_STR( "\x22 not exist." ) );
	if( ! isOverWrite )
	{
		if ( isExist( fileNameDst ) )
			FRL_THROW( FRL_STR( "File \x22" ) + fileNameDst + FRL_STR( "\x22 already exist." ) );
	}
	if( fileNameSrc == fileNameDst )
		FRL_THROW( FRL_STR( "Files names identical. \x22" ) + fileNameSrc + FRL_STR( "\x22 = \x22" ) + fileNameDst + FRL_STR( "\x22" ) );
	#ifdef UNICODE
		if( ::MoveFileExW( fileNameSrc.c_str(), fileNameDst.c_str(), MOVEFILE_REPLACE_EXISTING ) == 0 )
			FRL_THROW_SYSAPI( FRL_STR( "File rename failed." ) );
	#else
		if( ::MoveFileExA( fileNameSrc.c_str(), fileNameDst.c_str(), MOVEFILE_REPLACE_EXISTING ) == 0 )
			FRL_THROW_SYSAPI( FRL_STR( "File rename failed." ) );
	#endif
}

void copy( const frl::String &fileNameSrc, const frl::String &fileNameDst, frl::Bool overwrite )
{
	FRL_EXCEPT_GUARD();
	if ( ! isExist( fileNameSrc ) )
		FRL_THROW( FRL_STR( "File \x22" ) + fileNameSrc + FRL_STR( "\x22 not exist." ) );
	#ifdef UNICODE
		if ( ! ::CopyFileW(fileNameSrc.c_str(), fileNameDst.c_str(), ! overwrite ) )
			FRL_THROW_SYSAPI( FRL_STR( "Error file copy." ) );
	#else
		if ( ! ::CopyFileA(fileNameSrc.c_str(), fileNameDst.c_str(), ! overwrite ) )
			FRL_THROW_SYSAPI( "Error file copy." );
	#endif
}

void move( const frl::String &fileNameSrc, const frl::String &fileNameDst, frl::Bool overwrite )
{
	FRL_EXCEPT_GUARD();
	copy( fileNameSrc, fileNameDst, overwrite );
	removal( fileNameSrc );
}

FileRWCount write( FileDescriptor &file, const void *data, FileRWCount count )
{
	FRL_EXCEPT_GUARD();
	if ( file == InvalidFileDescriptor )
		FRL_THROW( FRL_STR( "Invalid file FileDescriptor." ) );
	if( data == NULL )
		FRL_THROW( FRL_STR( "Invalid data." ) );
	if ( count == 0 )
		return 0;
	FileRWCount writeCount = 0;
	if( ::WriteFile( file, data, count, ( FileRWCount* )&writeCount, NULL ) == 0 )
		FRL_THROW_SYSAPI( FRL_STR( "Error write data to file." ) );
	return writeCount;
}

FileRWCount read( FileDescriptor &file, void *buffer, FileRWCount count )
{
	FRL_EXCEPT_GUARD();
	if ( file == InvalidFileDescriptor )
		FRL_THROW( FRL_STR( "Invalid file FileDescriptor." ) );
	if ( count == 0 )
		return 0;
	FileRWCount readCount = 0;
	if ( ::ReadFile( file, buffer, count, ( FileRWCount* )&readCount, NULL) == 0 )
		FRL_THROW_SYSAPI( FRL_STR( "Error read data from file." ) );
	return readCount;
}

void hardLink( const frl::String &fileName, const frl::String &linkName )
{
	FRL_EXCEPT_GUARD();
	typedef BOOL (WINAPI *FunctionCreateHardLink)(const TCHAR*,const TCHAR*,LPSECURITY_ATTRIBUTES);

	class LibLoader
	{
	public:
		HMODULE module;
		LibLoader( void ) : module( NULL ) {}
		~LibLoader( void )
		{
			if( module != NULL )
				FreeLibrary( module );
		}
		void Load( String moduleName )
		{
			#ifdef UNICODE
				module = LoadLibraryW( moduleName.c_str() );
			#else
				module = LoadLibraryA( moduleName.c_str() );
			#endif
			if ( module == NULL )
				FRL_THROW_SYSAPI( FRL_STR("Module \x22") + moduleName + FRL_STR( "\x22 not loaded.") );
		}
	};

	if ( !isExist( fileName ) )
		FRL_THROW( FRL_STR( "File not exist!" ) );

	LibLoader ll;
	ll.Load( FRL_STR( "Kernel32" ) );
	FunctionCreateHardLink fn = (FunctionCreateHardLink)::GetProcAddress( ll.module, "CreateHardLinkW" );
	if ( fn( linkName.c_str(), fileName.c_str(), NULL) == 0 )
		FRL_THROW_SYSAPI( FRL_STR( "Hard link not created." ) );
}

Bool isIdentical( const String &fileName1, const String &fileName2 )
{
	FRL_EXCEPT_GUARD();
	if( !isExist( fileName1) )
		FRL_THROW( FRL_STR( "File not exist." ) );
	if( ! isExist( fileName2 ) )
		FRL_THROW( FRL_STR( "File not Exist." ) );
	if ( fileName1 == fileName2)
		return True;
	BY_HANDLE_FILE_INFORMATION info1,info2;
	HANDLE hFile1,hFile2;
	Bool same=false;
	#ifdef UNICODE
		hFile1=::CreateFileW(fileName1.c_str(), GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile1!=INVALID_HANDLE_VALUE){
			hFile2=::CreateFileW( fileName2.c_str(), GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			if(hFile2!=INVALID_HANDLE_VALUE){
				if(::GetFileInformationByHandle(hFile1,&info1) && ::GetFileInformationByHandle(hFile2,&info2)){
					same=(info1.nFileIndexLow==info2.nFileIndexLow && info1.nFileIndexHigh==info2.nFileIndexHigh && info1.dwVolumeSerialNumber==info2.dwVolumeSerialNumber);
				}
				::CloseHandle(hFile2);
			}
			::CloseHandle(hFile1);
		}
		return same;
	#else
	hFile1=::CreateFileA(fileName1.c_str(), GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile1!=INVALID_HANDLE_VALUE){
		hFile2=::CreateFileA( fileName2.c_str(), GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile2!=INVALID_HANDLE_VALUE){
			if(::GetFileInformationByHandle(hFile1,&info1) && ::GetFileInformationByHandle(hFile2,&info2)){
				same=(info1.nFileIndexLow==info2.nFileIndexLow && info1.nFileIndexHigh==info2.nFileIndexHigh && info1.dwVolumeSerialNumber==info2.dwVolumeSerialNumber);
			}
			::CloseHandle(hFile2);
		}
		::CloseHandle(hFile1);
	}
	return same;
	#endif
}

void concatenate( const String &fileName1, const String &fileName2, const String &fileNameDst, Bool overwrite )
{
	FRL_EXCEPT_GUARD();
	struct FileDescriptorManager
	{
		FileDescriptor file1;
		FileDescriptor file2;
		FileDescriptor fileDst;

		FileDescriptorManager( void )
			:	file1( InvalidFileDescriptor ),
			file2( InvalidFileDescriptor ),
			fileDst( InvalidFileDescriptor )
		{
		}
		~FileDescriptorManager()
		{
			close( file1 );
			close( file2 );
			close( fileDst );
		}

	};
	FileDescriptorManager dm;

	 open( dm.file1, fileName1, openReadOnly );
	if( dm.file1 == InvalidFileDescriptor )
		FRL_THROW( FRL_STR( "File " ) + fileName2 + FRL_STR( " not opened." ) );

	 open( dm.file2, fileName2, openReadOnly );
	if( dm.file2 == InvalidFileDescriptor )
		FRL_THROW( FRL_STR( "File " ) + fileName2 + FRL_STR( " not opened." ) );

	if( isExist( fileNameDst ) )
	{
		if( overwrite )
		{
			removal( fileNameDst );
			create( fileNameDst );
			open( dm.fileDst, fileNameDst, openWriteOnly );
			if( dm.fileDst == InvalidFileDescriptor )
				FRL_THROW( FRL_STR( "File " ) + fileNameDst + FRL_STR( " not opened." ) );
		}
		else
		{
			 open( dm.fileDst, fileNameDst, openWriteOnly );
			if( dm.fileDst == InvalidFileDescriptor )
				FRL_THROW( FRL_STR( "File " ) + fileNameDst + FRL_STR( " not opened." ) );
			seekToEnd( dm.fileDst );
		}
	}
	else
	{
		create( fileNameDst );
		open( dm.fileDst, fileNameDst, openWriteOnly );
		if( dm.fileDst == InvalidFileDescriptor )
			FRL_THROW( FRL_STR( "File " ) + fileNameDst + FRL_STR( " not opened." ) );
	}

	UChar buffer[4096];
	FileRWCount countWrite, countRead;
	while( True )
	{
		countRead = read( dm.file1, buffer, sizeof(buffer) );

		if( countRead == 0 )
			break;

		countWrite = write( dm.fileDst, buffer, countRead );
	}
	while( True )
	{
		countRead = read( dm.file2, buffer, sizeof(buffer) );

		if( countRead == 0)
			break;

		countWrite = write( dm.fileDst, buffer, countRead );
	}
}

void symLink( const frl::String&, const frl::String& )
{
	FRL_EXCEPT_GUARD();
	FRL_THROW( FRL_STR( "OS Windows(C) not supported symbol links to file." ) );
}

void getStat( const String &fileName, Stat &stat_ )
{
	FRL_EXCEPT_GUARD();
	if( ! isExist( fileName ) )
		FRL_THROW( FRL_STR( "File not exist" ) );

	WIN32_FILE_ATTRIBUTE_DATA data;
	SHFILEINFO sfi;
	#ifdef UNICODE
		if ( ! ::GetFileAttributesExW( fileName.c_str(), GetFileExInfoStandard, &data ) )
			FRL_THROW( FRL_STR( "Error get file attributes and statistics." ) );
	#else
		if ( ! ::GetFileAttributesExA( fileName.c_str(), GetFileExInfoStandard, &data ) )
			FRL_THROW( FRL_STR( "Error get file attributes and statistics." ) );
	#endif

		if( data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN )
			stat_.permissionMode |= permHidden;
		if ( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			stat_.permissionMode |= permDirectory;
		else
			stat_.permissionMode |= permFile;
		if ( data.dwFileAttributes & FILE_ATTRIBUTE_READONLY )
			stat_.permissionMode &=~ ( permOwnerReadWrite | permGroupWrite | permOtherWrite );
	#ifdef UNICODE
		if(::SHGetFileInfoW(fileName.c_str(), 0, &sfi, sizeof(SHFILEINFO), SHGFI_EXETYPE)==0)
			stat_.permissionMode &=! ( permOwnerExec | permGroupExec | permOtherExec );
	#else
		if(::SHGetFileInfoA( fileName.c_str(), 0, &sfi, sizeof(SHFILEINFO), SHGFI_EXETYPE)==0)
			stat_.permissionMode &=! ( permOwnerExec | permGroupExec | permOtherExec );
	#endif
		stat_.uid = 0;
		stat_.gid = 0;

		struct TimeUtil
		{
			static Long getTime( Long time_ )
			{
				#if( FRL_COMPILER == FRL_COMPILER_MINGW )
					return ( time_ - 1164447360 * 100000000 ) * 100;
				#else
					return ( time_ - __int64( 1164447360 ) * __int64( 100000000 ) ) * 100;
				#endif
			}
		};

		stat_.accessTime = TimeUtil::getTime ( *( ( Long*) &data.ftLastAccessTime ) );
		stat_.modifyTime = TimeUtil::getTime ( *( ( Long*) &data.ftLastWriteTime ) );
		stat_.createTime = TimeUtil::getTime ( *( ( Long*) &data.ftCreationTime ) );
		stat_.length = ( ( ( Long ) data.nFileSizeHigh )<<32) | ( ( Long ) data.nFileSizeLow );
}

String getCurrentDirectory()
{
	String currentDir;
	Char path[MAX_PATH];
	DWORD ret = ::GetModuleFileName(NULL, path ,MAX_PATH);
	if ( ret != 0 )
	{
		currentDir = path;
		size_t pos = currentDir.rfind( FRL_STR('\\') );
		if( pos != String::npos )
		{
			currentDir = currentDir.substr( 0, pos );
		}
		return currentDir;
	}
	return FRL_STR( "" );
}

void addSlashToEndPath( String &path )
{
	String::size_type length = path.size();
	if( length < 2 )
		return;
	if( path[length-1] == FRL_STR( '\\' ) )
		return;
	path += FRL_STR('\\');
}

} // namespace fs
} // namespace io
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
