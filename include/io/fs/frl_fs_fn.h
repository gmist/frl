#ifndef FRL_FS_FN_H_
#define FRL_FS_FN_H_

#include "frl_types.h"

namespace frl{ namespace io{ namespace fs{

enum SeekMode
{
	seekFromStart,
	seekFromCurrent,
	seekFromEnd
}; // enum SeekMode

enum AccessMode
{
	accessRead,
	accessWrite,
	accessReadWrite,
	accessExecute,
	accessExist
}; // enum AccessMode

enum OpenMode
{
	openNotOpen			= 0x0000,	// Not open file
	openReadOnly		= 0x0001,	// Open file for read operations only
	openWriteOnly		= 0x0002,	// Open file for wtite operations only
	openReadWrite		= 0x0003,	// Open file for read-write operations
	openAppend			= 0x0004,	// At open file seek to end file
	openTruncate			= 0x0008,	// At open file erase content
	openCreate			= 0x0010,	// Create file
	openUnbuffered		= 0x0020,	// Open file with out spool
	openNonBlocking	= 0x0040,	// FIXME comment
	openExclusive 		= 0x0080,	// Open file in exclusive mode
	openText				= 0x0100,	// Open file as text file
	openBinary				= 0x0200,	// Open file as binary file
	openLargeFile		= 0x0400	// Open large file
}; // enum OpenMode

/*enum OpenMode
{
	openNoOpen,
	openReadOnly,
	openWriteOnly,
	openReadWrite,
	openWriteOnlyAppend,
	openReadWriteAppend,
	openWriteOnlyTrunc,
	openReadWriteTrunc
};*/

enum PermissionMode
{
	// Permissions
	permOtherRead 			= 0x00004,	// Others have read permission
	permOtherWrite			= 0x00002,	// Others have write permisson
	permOtherExec			= 0x00001,	// Others have execute permission
	permOtherReadWrite 	= 0x00006,	// Others have read and write permission
	permOtherFull 			= 0x00007,	// Others have full access

	permGroupRead 		= 0x00020,	// Group has read permission
	permGroupWrite 		= 0x00010,	// Group has write permission
	permGroupExec 			= 0x00008,	// Group has execute permission
	permGroupReadWrite = 0x00030,	// Group has read and write permission
	permGroupFull 			= 0x00038,	// Group has full access

	permOwnerRead 		= 0x00100,		// Owner has read permission
	permOwnerWrite 		= 0x00080,		// Owner has write permission
	permOwnerExec 		= 0x00040,		// Owner has execute permission
	permOwnerReadWrite	= 0x00180,	// Owner has read and write permission
	permOwnerFull 			= 0x001C0,		// Owner has full access

	// Other flags
	permHidden 		= 0x00200,		// Hidden file
	permDirectory 	= 0x00400,		// Is directory
	permFile 			= 0x00800,		// Is regular file
	permSymLink 		= 0x01000,	// Is symbolic link

	// Special mode bits
	permSetUserID 		= 0x02000,	// SUID
	permSetGroupID 	= 0x04000,	// SGID
	permSticky 			= 0x08000,	// Sticky bit

	// Device special files
	permCharacter	= 0x10000,	// Character device
	permBlock			= 0x20000,	// Block device
	permSocket		= 0x40000,	// Socket device
	permFifo			= 0x80000		// Fifo device
}; // enum PermissionMode

struct Stat
{
	UInt permissionMode; // File permission mode
	UInt  uid; // UID
	UInt  gid; // GID

	FileOffset length;					// Length file
	time::Time  createTime;			// File create time (ns)
	time::Time  accessTime;		// File last access time (ns)
	time::Time  modifyTime;		// File modify time (ns)

	Stat( void )
	{
		Clear();
	}

	void Clear( void )
	{
		permissionMode = 0;
		uid = 0;
		gid = 0;
		length = 0;
		accessTime = 0;
		createTime = 0;
		modifyTime = 0;
	}

	// FIXME add operator ==
}; // struct State

void close( frl::io::fs::FileDescriptor &file );

frl::Bool isExist( const frl::String &fileName );

frl::Bool access( const frl::String &fileName, AccessMode mode );

void open( FileDescriptor &FileDescriptor, const frl::String &fileName, frl::UInt openMode = openReadWrite | openBinary, frl::UInt permMode = permOwnerReadWrite | permGroupRead | permOtherRead );

void create( const frl::String &fileName, frl::UInt permMode = permOwnerReadWrite | permGroupRead | permOtherRead );

FileOffset seek( FileDescriptor &file, FileOffset new_position, SeekMode mode );

FileOffset tell( FileDescriptor &file );

FileOffset length( FileDescriptor &file );

FileOffset length( const frl::String &fileName );

FileOffset seekToStart( FileDescriptor &file );

FileOffset seekToEnd( FileDescriptor &file );

frl::Bool isEof( FileDescriptor &file );

void flush( FileDescriptor &file );

FileRWCount write( FileDescriptor &file, const void *data, FileRWCount count );

FileRWCount read( FileDescriptor &file, void *buffer, FileRWCount count );

void truncate( FileDescriptor &file, FileOffset newLength );

void removal( const frl::String &fileName );

void rename( const frl::String &fileNameSrc, const frl::String &fileNameDst , Bool isOverWrite = False );

void hardLink( const frl::String &fileName, const frl::String &linkName );

void symLink( const frl::String &fileName, const frl::String &linkName );

void copy( const frl::String &fileNameSrc, const frl::String &fileNameDst, frl::Bool overwrite = frl::True );

void move( const frl::String &fileNameSrc, const frl::String &fileNameDst, frl::Bool overwrite = frl::True );

frl::Bool isIdentical( const String &fileName1, const String &fileName2 );

void concatenate( const String &fileName1, const String &fileName2, const String &fileNameDst, Bool overwrite = True );

void getStat( const String &fileName, Stat &stat_ );

void setStat( const String &fileName, UInt permMode );

String getCurrentDirectory();

void addSlashToEndPath( String &path );

} // namespace fs
}	// namespace io
}	// FatRat Library

#endif // FRL_FILE_FN_H_
