#include "io/comm_ports/frl_comm_ports_serial.h"

namespace frl
{
namespace io
{
namespace comm_ports
{

Serial::Serial()
	:	handle( fs::InvalidFileDescriptor ),
		lastEvent( serial::EVENT_NONE )
{
}

Serial::~Serial()
{
	if( handle != fs::InvalidFileDescriptor )
		close();
}

void Serial::open( int portNumber )
{
	FRL_EXCEPT_GUARD();
	name = serial::getPortName( portNumber );
	handle = serial::open( name );				
}

void Serial::close()
{
	FRL_EXCEPT_GUARD();
	serial::close( handle );
}

void Serial::init( DWORD inQueue /*= 0*/, DWORD outQueue /*= 0 */ )
{
	FRL_EXCEPT_GUARD();
	serial::init( handle, inQueue, outQueue );
}

void Serial::setEventMask( unsigned long eventMask_ )
{
	FRL_EXCEPT_GUARD();
	serial::setEventMask( handle, eventMask_ );
	eventMask = eventMask_;
}

void Serial::setReadTimeouts( serial::ReadTimeout timeOutType )
{
	FRL_EXCEPT_GUARD();
	serial::setReadTimeouts( handle, timeOutType );
}

void Serial::setup(	serial::BaudRate baudrate /*= serial::BAUD_RATE_DEFAULT*/,
							serial::CharSize charSize /*= serial::CHAR_SIZE_DEFAULT*/,
							serial::Parity parity /*= serial::EPARITY_DEFAULT*/,
							serial::StopBits stopBits /*= serial::STOP_BITS_DEFAULT */ )
{
	FRL_EXCEPT_GUARD();
	serial::setup( handle, baudrate, charSize, parity, stopBits );
}

void Serial::waitEvent()
{
	FRL_EXCEPT_GUARD();
	serial::waitEvent( handle, lastEvent );
}

serial::PortEvent Serial::getLastEvent()
{
	return lastEvent;
}

serial::SerialRWCount Serial::write( void *data, serial::SerialRWCount counts )
{
	FRL_EXCEPT_GUARD();
	return serial::write( handle, data, counts );
}

serial::SerialRWCount Serial::read( void *buffer, serial::SerialRWCount counts )
{
	FRL_EXCEPT_GUARD();
	return serial::read( handle, buffer, counts );
}

} // namespace comm_ports
} // namespace io
} // FatRat Library

