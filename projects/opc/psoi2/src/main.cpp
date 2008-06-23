#include <Windows.h>
#include "console_std/frl_iostream.h"
#include "psoi2_device_manager.h"
#include "psoi2_device.h"
#include <WinCon.h>

int main( int argc, char *argv[] )
{
	if( argc >= 2 )
	{
		using namespace frl::console_std;
		if( std::string( "-r" ) == std::string( argv[1] ) )
		{
			try
			{
				psoi2util::regServer();
				ColorOut( foregroundColor::iGreen )
				<< "Registration success!" << std::endl;
			}
			catch( std::exception &ex )
			{
				ColorOut( foregroundColor::iRed )
				<< "Registration fail! " << std::endl;
				ColorOut( foregroundColor::iDefault )
				<< ex.what() << std::endl;
			}
			exit( 0 );
		}

		if( std::string("-u") == std::string( argv[1] ) )
		{
			try
			{
				psoi2util::unregServer();
				ColorOut( foregroundColor::iGreen )
					<< "Removing registration success!" << std::endl;
			}
			catch( std::exception &ex )
			{
				ColorOut( foregroundColor::iRed )
					<< "Unregistration fail! " << std::endl;
				ColorOut( foregroundColor::iDefault )
					<< ex.what() << std::endl;
			}
			exit( 0 );
		}

		if( std::string("-Embedding") != std::string( argv[1] ) )
		{
			ColorOut( foregroundColor::iGreen )
				<< "Available parameters:" << std::endl;
			ColorOut( foregroundColor::iDefault ) << "-r";
			ColorOut( foregroundColor::onDefault )
				<< " - registration server." << std::endl;
			ColorOut( foregroundColor::iDefault ) << "-u";
			ColorOut( foregroundColor::onDefault )
				<< " - Unregistration server." << std::endl;
			exit( 0 );
		}
	}
	
	Bool exitOnAllClientDisconnected = True;
	{
		frl::poor_xml::Document config;
		try
		{
			config.loadFromCurrenttDir( FRL_STR( "config.xml" ) );
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
