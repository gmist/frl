#ifndef frl_comm_ports_serial_win32_h_
#define frl_comm_ports_serial_win32_h_
#include "io/comm_ports/frl_comm_ports_serial_fh.h"
#include <boost/noncopyable.hpp>

namespace frl{ namespace io{ namespace comm_ports{

class Serial : private boost::noncopyable
{
private:
	String name;
	HANDLE handle;
	unsigned long eventMask;
	serial::PortEvent lastEvent;
public:
	Serial();
	~Serial();
	void open( int portNumber );
	void close();
	void init( DWORD inQueue = 0, DWORD outQueue = 0 );
	void setEventMask( unsigned long eventMask_ );
	void setReadTimeouts( serial::ReadTimeout timeOutType );
	void setup(		serial::BaudRate baudrate = serial::BAUD_RATE_DEFAULT,
						serial::CharSize charSize = serial::CHAR_SIZE_DEFAULT,
						serial::Parity parity = serial::EPARITY_DEFAULT,
						serial::StopBits stopBits = serial::STOP_BITS_DEFAULT );
	void waitEvent();
	serial::PortEvent getLastEvent();
	serial::SerialRWCount write( void *data, serial::SerialRWCount counts );
	serial::SerialRWCount read( void *buffer, serial::SerialRWCount counts );
};

} // namespace comm_ports
} // namespace io
} // FatRat Library

#endif // frl_comm_ports_serial_win32_h_
