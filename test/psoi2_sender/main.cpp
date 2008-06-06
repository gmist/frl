#include "io/comm_ports/frl_comm_ports_serial.h"
#include "poor_xml/frl_poor_xml_document.h"
#include "io/fs/frl_fs_fn.h"
#include "console_std/frl_iostream.h"
#include "frl_smart_ptr.h"
#include "frl_lexical_cast.h"
#include "time/frl_time_sys_time.h"
#include <bitset>
#include <time.h>

unsigned long getNumber()
{
	unsigned long tmp = (unsigned long)( ( (double) rand() / (double) 9 ) * 9 );
	while( tmp > 9 )
		tmp -= 9;
	return tmp;
}

void genSendingPacket( unsigned char* buffer, int channels )
{
	int numBytes = ( channels ) * 2 + 2;
	for( int i = 0; i < numBytes; ++i )
		buffer[ i ] = 0;

	buffer[0] = 251;
	buffer[1] = static_cast< unsigned char >( channels * 2 );
	
	std::bitset< 8 > tmpByte;
	std::bitset< 4 > tmpVal;
	// уставливаем признак исправности ППХ и генерируем третью цифру концентрации
	for( int i = 3; i < numBytes ; i = i + 2 )
	{
		tmpByte[0] = 1;
		tmpVal = getNumber();
		tmpByte[7] = tmpVal[3];
		tmpByte[6] = tmpVal[2];
		tmpByte[5] = tmpVal[1];
		tmpByte[4] = tmpVal[0];
		buffer[ i ] = (unsigned char)tmpByte.to_ulong();
	}
	
	std::bitset< 4 > tmpVal2;
	// генерируем 1 и 2 цифры концентрации
	for( int i = 2; i < numBytes ; i = i + 2 )
	{
		tmpByte[0] = 1;
		tmpVal = getNumber();
		tmpVal2 = getNumber();
		tmpByte[7] = tmpVal[3];
		tmpByte[6] = tmpVal[2];
		tmpByte[5] = tmpVal[1];
		tmpByte[4] = tmpVal[0];
		tmpByte[3] = tmpVal2[3];
		tmpByte[2] = tmpVal2[2];
		tmpByte[1] = tmpVal2[1];
		tmpByte[0] = tmpVal2[0];
		buffer[ i ] = (unsigned char)tmpByte.to_ulong();
	}
}

int main( int argc, char *argv[] )
{
	using namespace frl;
	poor_xml::Document config;
	Int numChannels;
	Int numCom;
	Int delay;
	try
	{
		config.LoadFromCurrenttDir( FRL_STR("config.xml") );
		poor_xml::NodesList psoi2conf = config.getRoot()->getSubNodes( FRL_STR("Psoi2Sender") );
		numCom = lexicalCast< String, Int >( (*psoi2conf.begin())->getProprtyVal( FRL_STR("ComPort") ) );
		numChannels = lexicalCast< String, Int >( (*psoi2conf.begin())->getProprtyVal( FRL_STR("Channels") ) );
		if( numChannels <= 0 )
		{
			console_std::ColorOut( console_std::foregroundColor::iRed ) << FRL_STR("Error: ");
			console_std::ColorOut( console_std::foregroundColor::iDefault ) << FRL_STR( "Number of channels mast be > 0" );
			return 1;
		}

		delay = lexicalCast< String, Int >( (*psoi2conf.begin())->getProprtyVal( FRL_STR("Delay") ) );
		if( numChannels <= 0 )
		{
			console_std::ColorOut( console_std::foregroundColor::iRed ) << FRL_STR("Error: ");
			console_std::ColorOut( console_std::foregroundColor::iDefault ) << FRL_STR( "Delay mast be > 0" );
			return 1;
		}
		
		delay = delay * 1000;
		
		io::comm_ports::Serial port;
		port.open( numCom );
		port.setup( io::comm_ports::serial::BAUD_RATE_1200 );
		
		srand( (unsigned int) ::time( NULL ) );
		SmartPtr< unsigned char, smart_ptr::OwnerRefCount, smart_ptr::ArrayStorage > buffer( new unsigned char[ ( numChannels * 2 ) + 2] );
		for( ; ; )
		{
			genSendingPacket( smart_ptr::GetPtr( buffer ), numChannels );
			io::fs::FileDescriptor file;
			frl::time::SysTime tmpTime;
			tmpTime.setTimeSeparator( FRL_STR("-") );
			String fileName = tmpTime.getDateYYYYMMDD();
			fileName += FRL_STR( " - " );
			fileName += tmpTime.getTimeHHMMSS();
			fileName += FRL_STR(".gen");
			io::fs::create( fileName );
			io::fs::open( file, fileName );
			io::fs::write( file, smart_ptr::GetPtr( buffer), ( numChannels * 2 ) + 2 );
			port.write( smart_ptr::GetPtr( buffer ), ( numChannels * 2) + 2 );
			::Sleep( delay );
		}
	}
	catch( Exception& ex )
	{
		console_std::ColorOut( console_std::foregroundColor::iDefault ) << ex.getFullDescription() << FRL_NEW_LINE;
		return 1;
	}
}
