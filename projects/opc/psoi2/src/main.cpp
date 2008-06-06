#include <Windows.h>
#include "psoi2_device_manager.h"
#include "psoi2_device.h"
#include <WinCon.h>

int main( int argc, char *argv[] )
{
	Bool exitOnAllClientDisconnected = True;
	{
		frl::poor_xml::Document config;
		try
		{
			config.LoadFromCurrenttDir( FRL_STR( "config.xml" ) );
			frl::String sw = config.getRoot()->getProprtyVal( FRL_STR("ShowWindow") );
			if( frl::lexicalCast<String, int>( sw ) == 0 )
			{
				::FreeConsole();
			}
			else
			{
				frl::poor_xml::NodesList log = config.getRoot()->getSubNodes( FRL_STR("Log") );
				frl::String logLocale = (*log.begin())->getProprtyVal( FRL_STR("ConsoleLogLocale") );
				setlocale( LC_ALL, frl::multiByteCompatibility( logLocale ).c_str() );
			}
			frl::String isExit = config.getRoot()->getProprtyVal( FRL_STR("ExitOnAllClientDisconnected") );
			exitOnAllClientDisconnected = frl::lexicalCast< String, Bool >( isExit );
		}
		catch( frl::Exception& ){}
	}

	DeviceManager devManager;

	if( exitOnAllClientDisconnected )
	{
		Sleep( 20000 ); // wait for first client
		while( frl::opc::factory.isServerInUse() )
		{
			Sleep( 1000 );
		}
	}
	else
	{
		for( ; ; )
		{
			::Sleep( 1000 );
		}
	}

	return 0;
}
