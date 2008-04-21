#ifndef frl_comm_ports_serial_fh_h_
#define frl_comm_ports_serial_fh_h_
#include "io/fs/frl_fs_fn.h"
#include "frl_exception.h"

namespace frl
{
namespace io
{
namespace comm_ports
{
namespace serial
{
typedef fs::FileDescriptor SerialPortDescriptor;
typedef fs::FileRWCount SerialRWCount;

FRL_EXCEPTION_CLASS( PortNotFound );
FRL_EXCEPTION_CLASS( InUse );
FRL_EXCEPTION_CLASS( UnknownError );
FRL_EXCEPTION_CLASS( AlreadyOpenned );
FRL_EXCEPTION_CLASS( NotOpenned );
FRL_EXCEPTION_CLASS( UnableSetEventMask );
FRL_EXCEPTION_CLASS( UnableInit );
FRL_EXCEPTION_CLASS( UnableSetReadTimeouts );
FRL_EXCEPTION_CLASS( UnableGetReadTimeouts );
FRL_EXCEPTION_CLASS( InvalidArgument );
FRL_EXCEPTION_CLASS( UnableWaitEvent );
FRL_EXCEPTION_CLASS( UnableWriteData );
FRL_EXCEPTION_CLASS( UnableReadData );

#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
FRL_EXCEPTION_CLASS( UnableGetCommState );
FRL_EXCEPTION_CLASS( UnableSetCommState );

FRL_EXCEPTION_CLASS( UnableSetCommConfig );
FRL_EXCEPTION_CLASS( UnableGetDefaultCommConfig );
#endif

#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )

// Timeout settings
enum ReadTimeout
{
	READ_TIMEOUT_UNKNOWN		= -1,	// Unknown
	READ_TIMEOUT_NON_BLOCK	=  0,	// Always return immediately
	READ_TIMEOUT_BLOCK			=  1	// Block until everything is retrieved
};

enum PortEvent
{
	EVENT_NONE  					 = 0,					// Event trigged without cause
	EVENT_BREAK 					= EV_BREAK,		// A break was detected on input
	EVENT_CTS   					= EV_CTS,			// The CTS signal changed state
	EVENT_DSR   					= EV_DSR,			// The DSR signal changed state
	EVENT_ERROR 					= EV_ERR,			// A line-status error occurred
	EVENT_RING  					= EV_RING,		// A ring indicator was detected
	EVENT_RLSD  					= EV_RLSD,		// The RLSD signal changed state
	EVENT_RECV  					= EV_RXCHAR,	// Data is received on input
	EVENT_RCV_EV 					= EV_RXFLAG,	// Event character was received on input
	EVENT_SEND						= EV_TXEMPTY,	// Last character on output was sent
	EVENT_PRINTER_ERROR		= EV_PERR,		// Printer error
	EVENT_RX_80_FULL			= EV_RX80FULL,// Receive buffer is 80 percent full
	EVENT_PROVIDER_EVNT1	= EV_EVENT1,	// Provider specific event 1
	EVENT_PROVIDER_EVNT2	= EV_EVENT2	// Provider specific event 2
};

// Baud rate
enum BaudRate
{
	BAUD_RATE_110     = CBR_110,		// 110 bits/sec
	BAUD_RATE_300     = CBR_300,		// 300 bits/sec
	BAUD_RATE_600     = CBR_600,		// 600 bits/sec
	BAUD_RATE_1200    = CBR_1200,		// 1200 bits/sec
	BAUD_RATE_2400    = CBR_2400,		// 2400 bits/sec
	BAUD_RATE_4800    = CBR_4800,		// 4800 bits/sec
	BAUD_RATE_9600    = CBR_9600,		// 9600 bits/sec
	BAUD_RATE_14400   = CBR_14400,	// 14400 bits/sec
	BAUD_RATE_19200   = CBR_19200,	// 19200 bits/sec
	BAUD_RATE_38400   = CBR_38400,	// 38400 bits/sec
	BAUD_RATE_56000   = CBR_56000,	// 56000 bits/sec
	BAUD_RATE_57600   = CBR_57600,	// 57600 bits/sec
	BAUD_RATE_115200  = CBR_115200,	// 115200 bits/sec
	BAUD_RATE_128000  = CBR_128000,	// 128000 bits/sec
	BAUD_RATE_256000  = CBR_256000,	// 256000 bits/sec
	BAUD_RATE_DEFAULT = BAUD_RATE_9600
};

// Data bits (5-8)
enum CharSize
{
	CHAR_SIZE_5       =  5,			// 5 bits per byte
	CHAR_SIZE_6       =  6,			// 6 bits per byte
	CHAR_SIZE_7       =  7,			// 7 bits per byte
	CHAR_SIZE_8       =  8,			// 8 bits per byte
	CHAR_SIZE_DEFAULT = CHAR_SIZE_8 
};

// Parity scheme
enum Parity
{
	EPARITY_NONE		= NOPARITY,		// No parity
	EPARITY_ODD		= ODDPARITY,	// Odd parity
	EPARITY_EVEN		= EVENPARITY,	// Even parity
	EPARITY_MARK		= MARKPARITY,	// Mark parity
	EPARITY_SPACE		= SPACEPARITY,// Space parity
	EPARITY_DEFAULT = EPARITY_NONE
};

// Stop bits
enum StopBits
{
	STOP_BITS_1				= ONESTOPBIT,		// 1 stop bit
	STOP_BITS_1_5				= ONE5STOPBITS,	// 1.5 stop bit
	STOP_BITS_2				= TWOSTOPBITS,	// 2 stop bits
	STOP_BITS_DEFAULT		= STOP_BITS_1
};

enum FlowControl
{
	FLOW_CONTROL_NONE = 0,
	FLOW_CONTROL_HARD = 1, // RTS/CTS
	FLOW_CONTROL_SOFT = 2, // XON.XOFF
	FLOW_CONTROL_DEFAULT = FLOW_CONTROL_NONE
};
#endif // FRL_PLATFORM_WIN32

#if( FRL_PLATFORM == FRL_PLATFORM_LINUX )
enum BaudRate
{
	BAUD_RATE_50			= B50,
	BAUD_RATE_75			= B75,
	BAUD_RATE_110		= B110,
	BAUD_RATE_134		= B134,
	BAUD_RATE_150		= B150,
	BAUD_RATE_200		= B200,
	BAUD_RATE_300		= B300,
	BAUD_RATE_600		= B600,
	BAUD_RATE_1200		= B1200,
	BAUD_RATE_1800		= B1800,
	BAUD_RATE_2400		= B2400,
	BAUD_RATE_4800		= B4800,
	BAUD_RATE_9600		= B9600,
	BAUD_RATE_19200		= B19200,
	BAUD_RATE_38400		= B38400,
	BAUD_RATE_57600		= B57600,
	BAUD_RATE_115200	= B115200,
	BAUD_RATE_230400	= B230400,
	BAUD_RATE_460800	= B460800,
	BAUD_RATE_DEFAULT = BAUD_RATE_9600
};

enum Parity
{
	EPARITY_EVEN,
	EPARITY_ODD,
	EPARITY_NONE,
	EPARITY_DEFAULT = EPARITY_NONE
}; 

enum StopBits
{
	STOP_BITS_1,
	STOP_BITS_2,
	STOP_BITS_DEFAULT = STOP_BITS_1
};

enum CharSize
{
	CHAR_SIZE_5  = CS5,
	CHAR_SIZE_6  = CS6,
	CHAR_SIZE_7  = CS7,
	CHAR_SIZE_8  = CS8,
	CHAR_SIZE_DEFAULT = CHAR_SIZE_8
};

enum FlowControl
{
	FLOW_CONTROL_NONE,
	FLOW_CONTROL_HARD,
	FLOW_CONTROL_SOFT,
	FLOW_CONTROL_DEFAULT = FLOW_CONTROL_NONE
};
#endif // FRL_PLATFORM_LINUX

String getPortName( int portNumber );
SerialPortDescriptor open( int portNumber );
SerialPortDescriptor open( const String &portName );
void close( SerialPortDescriptor handle );
void init( SerialPortDescriptor handle, DWORD inQueue = 0, DWORD outQueue = 0 );
void setEventMask( SerialPortDescriptor handle, unsigned long eventMask );
void setReadTimeouts( SerialPortDescriptor handle, ReadTimeout timeOutType );
void setup(	SerialPortDescriptor handle,
					BaudRate baudrate = BAUD_RATE_DEFAULT,
					CharSize charSize = CHAR_SIZE_DEFAULT,
					Parity parity = EPARITY_DEFAULT,
					StopBits stopBits = STOP_BITS_DEFAULT );

void waitEvent( SerialPortDescriptor handle, PortEvent &event_ );
SerialRWCount write( SerialPortDescriptor handle, const void* data, SerialRWCount counts );
SerialRWCount read( SerialPortDescriptor handle, void *buffer, SerialRWCount counts );
} // namespace serial
} // namespace comm_ports
} // namespace io
} // FatRat Library

#endif // frl_comm_ports_serial_fh_h_
