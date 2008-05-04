#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "io/comm_ports/frl_comm_ports_serial_fh.h"
#include "frl_lexical_cast.h"
#include "io/fs/frl_fs_fn.h"

namespace frl
{
namespace io
{
namespace comm_ports
{
namespace serial
{
String getPortName( int portNumber )
{
	String strName;
	strName = FRL_STR( "\\\\.\\COM" );
	strName += lexicalCast< int, String >( portNumber );
	return strName;
}				

SerialPortDescriptor open( int portNumber )
{
	FRL_EXCEPT_GUARD();
	return serial::open( getPortName( portNumber ) );
}

SerialPortDescriptor open( const String &portName )
{
	FRL_EXCEPT_GUARD();
	SerialPortDescriptor portHandle = ::CreateFile( portName.c_str(),
										GENERIC_READ | GENERIC_WRITE,
										0,
										NULL,
										OPEN_EXISTING,
										0,
										NULL );

	if( portHandle == INVALID_HANDLE_VALUE )
	{
		switch( ::GetLastError() )
		{
		case ERROR_FILE_NOT_FOUND:
			FRL_THROW_S_CLASS( serial::PortNotFound );

		case ERROR_ACCESS_DENIED:
			FRL_THROW_S_CLASS( serial::InUse );

		default:
			FRL_THROW_S_CLASS( serial::UnknownError );
		}
	}

	try
	{
		// setup the device for default settings:	
		// set default events mask
		setEventMask(portHandle, EVENT_RECV | EVENT_ERROR | EVENT_BREAK );
		// set default timeouts
		setReadTimeouts( portHandle, READ_TIMEOUT_NON_BLOCK );
	}
	catch( frl::Exception& ex )
	{
		close( portHandle );
		portHandle = fs::InvalidFileDescriptor;
		throw ex;
	}
	return portHandle;
}

void close( SerialPortDescriptor handle )
{
	FRL_EXCEPT_GUARD();
	fs::close( handle );
}

void init( SerialPortDescriptor handle, DWORD inQueue, DWORD outQueue )
{
	FRL_EXCEPT_GUARD();
	if( handle == fs::InvalidFileDescriptor )
		FRL_THROW_S_CLASS( serial::NotOpenned );
	if( !::SetupComm( handle, inQueue, outQueue ) )
		FRL_THROW_S_CLASS( serial::UnableInit );
}

void setEventMask( SerialPortDescriptor handle, unsigned long eventMask )
{
	FRL_EXCEPT_GUARD();
	if( handle == fs::InvalidFileDescriptor )
		FRL_THROW_S_CLASS( serial::NotOpenned );
	if( !::SetCommMask( handle, eventMask) )
		FRL_THROW_S_CLASS( serial::UnableSetEventMask );
}

void setReadTimeouts( SerialPortDescriptor handle, ReadTimeout newTimeOut )
{
	FRL_EXCEPT_GUARD();
	if( handle == fs::InvalidFileDescriptor )
		FRL_THROW_S_CLASS( serial::NotOpenned );
	
	COMMTIMEOUTS cto;
	if( !::GetCommTimeouts( handle, &cto ) )
		FRL_THROW_S_CLASS( UnableGetReadTimeouts );

	switch( newTimeOut )
	{
	case READ_TIMEOUT_BLOCK:
		cto.ReadIntervalTimeout = 0;
		cto.ReadTotalTimeoutConstant = 0;
		cto.ReadTotalTimeoutMultiplier = 0;
		break;
	case READ_TIMEOUT_NON_BLOCK:
		cto.ReadIntervalTimeout = MAXDWORD;
		cto.ReadTotalTimeoutConstant = 0;
		cto.ReadTotalTimeoutMultiplier = 0;
		break;
	default:
		FRL_THROW_S_CLASS( InvalidArgument );
	}

	if( !::SetCommTimeouts( handle, &cto ) )
		FRL_THROW_S_CLASS( UnableSetReadTimeouts );
}

void setup( SerialPortDescriptor handle, BaudRate baudrate ,CharSize charSize , Parity parity , StopBits stopBits )
{
	FRL_EXCEPT_GUARD();
	if( handle == fs::InvalidFileDescriptor )
		FRL_THROW_S_CLASS( serial::NotOpenned );

	DCB dcb;
	dcb.DCBlength = sizeof( DCB );

	if( !::GetCommState( handle, &dcb) )
		FRL_THROW_S_CLASS( UnableGetCommState );

	dcb.BaudRate = DWORD( baudrate	);
	dcb.ByteSize	= BYTE( charSize );
	dcb.Parity		= BYTE( parity );
	dcb.StopBits	= BYTE( stopBits );

	// Determine if parity is used
	dcb.fParity	= ( parity != PARITY_NONE );

	// Set the new DCB structure
	if( !::SetCommState( handle, &dcb) )
		FRL_THROW_S_CLASS( UnableSetCommState );
}

void waitEvent( SerialPortDescriptor handle, PortEvent &event_ )
{
	FRL_EXCEPT_GUARD();
	if( handle == fs::InvalidFileDescriptor )
		FRL_THROW_S_CLASS( serial::NotOpenned );
	if( !::WaitCommEvent( handle, LPDWORD( &event_ ), 0 ) )
		FRL_THROW_S_CLASS( serial::UnableWaitEvent );
}

SerialRWCount write( SerialPortDescriptor handle, const void* data, SerialRWCount counts )
{
	FRL_EXCEPT_GUARD();
	if( handle == fs::InvalidFileDescriptor )
		FRL_THROW_S_CLASS( serial::NotOpenned );
	fs::FileRWCount writeCounts = 0;
	try
	{
		writeCounts = fs::write( handle, data, counts );
	}
	catch( ... )
	{
		FRL_THROW_S_CLASS( serial::UnableWriteData );
	}
	return writeCounts;
}

SerialRWCount read( SerialPortDescriptor handle, void *buffer, SerialRWCount counts )
{
	FRL_EXCEPT_GUARD();
	if( handle == fs::InvalidFileDescriptor )
		FRL_THROW_S_CLASS( serial::NotOpenned );
	fs::FileRWCount readCounts = 0;
	try
	{
		readCounts = fs::read( handle, buffer, counts );
	}
	catch( ... )
	{
		FRL_THROW_S_CLASS( serial::UnableReadData );
	}
	return readCounts;
}
} // namespace serial
} // namespace comm_ports
} // namespace io
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
