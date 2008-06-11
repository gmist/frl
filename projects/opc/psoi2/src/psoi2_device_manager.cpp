#include <Windows.h>
#include "psoi2_device_manager.h"
#include "psoi2_device.h"
#include "frl_lexical_cast.h"
#include "logging/frl_logging.h"

DeviceManager::DeviceManager()
{
	initializeAddressSpace();
	using namespace frl::opc;
	// read configuration file and fill devices array
	frl::Bool startRand = frl::False;
	try
	{
		config.LoadFromCurrenttDir( FRL_STR("config.xml") );
		frl::poor_xml::NodesList log = config.getRoot()->getSubNodes( FRL_STR("Log") );
		frl::String logLevelStr = ( *log.begin() )->getProprtyVal( FRL_STR("Level") );
		frl::logging::Level logLevel = frl::logging::LEVEL_TRACE;

		if( logLevelStr == FRL_STR("msg") )
			logLevel = frl::logging::LEVEL_MSG;
		if( logLevelStr == FRL_STR("warning") )
			logLevel = frl::logging::LEVEL_WARNING;
		if( logLevelStr == FRL_STR("error") )
			logLevel = frl::logging::LEVEL_ERROR;
		if( logLevelStr == FRL_STR("error") )
			logLevel = frl::logging::LEVEL_INFO;
		if( logLevelStr == FRL_STR("none" ) )
			logLevel = frl::logging::LEVEL_NONE;

		frl::String logFileNamePrefix = ( *log.begin() )->getProprtyVal( FRL_STR("LogFileNamePrefix") );
		frl::poor_xml::NodesList settings = config.getRoot()->getSubNodes( FRL_STR("Psoi2") );
		for( frl::poor_xml::NodesList::iterator it = settings.begin(); it != settings.end(); ++it )
		{
			frl::Int channelNumber = frl::lexicalCast< frl::String, frl::Int >( (*it)->getProprtyVal( FRL_STR("Channels") ) );
			frl::Int portNumber = frl::lexicalCast< frl::String, frl::Int >( (*it)->getProprtyVal( FRL_STR("ComPort") ) );
			frl::Bool simulation = frl::lexicalCast< frl::String, frl::Bool >( (*it)->getProprtyVal( FRL_STR("Simulation") ) );
			Psoi2Device *device = new Psoi2Device( portNumber, channelNumber, simulation, logLevel, logFileNamePrefix );
			devices.push_back( device );
			if( simulation )
				startRand = frl::True;
		}
	}
	catch( frl::Exception &ex )
	{
		String msg = FRL_STR("Configuration file maybe broken.\r\n") + ex.getFullDescription();
		::MessageBox( NULL,
								msg.c_str(),
								FRL_STR("OPC server configuration error!"),
								MB_OK | MB_ICONERROR );
		exit( 1 );
	}
	initializeDAServer();
	if( startRand )
		srand( (unsigned)(::time( NULL )) );

	for( std::vector< Psoi2Device* >::iterator it = devices.begin(); it != devices.end(); ++it )
	{
		(*it)->startProcess();
	}
}

DeviceManager::~DeviceManager()
{
	if( devices.size() )
	{
		for( std::vector< Psoi2Device* >::iterator it = devices.begin(); it != devices.end(); ++it )
		{
			(*it)->stopProcess();
			delete (*it);
		}
	}
	delete server;
}

void DeviceManager::initializeAddressSpace()
{
	frl::opc::opcAddressSpace::getInstance().finalConstruct( FRL_STR(".")); // initialize opc server address space
	frl::opc::address_space::Tag* info = frl::opc::opcAddressSpace::getInstance().addLeaf( FRL_STR("information"));
	info->isWriteable( False );
	info->setCanonicalDataType( VT_BSTR );
	info->write( String( FRL_STR("OPC server for PSOI2 (оянх-02) devices. If you to find error - please lem me know (serg.baburin@gmail.com).") ) );
}

void DeviceManager::initializeDAServer()
{
	server = new frl::opc::DAServer( frl::opc::ServerTypes::localSever32 );
	psoi2util::setServerInfo( *server );
	server->init();
}

namespace psoi2util
{
	void regServer()
	{
		opc::DAServer tmp( frl::opc::ServerTypes::localSever32 );
		setServerInfo( tmp );
		tmp.registrerServer();
	}
	
	void unregServer()
	{
		opc::DAServer tmp( frl::opc::ServerTypes::localSever32 );
		setServerInfo( tmp );
		tmp.unregisterServer();
	}
	
	void setServerInfo( opc::DAServer &toServer )
	{
		toServer.setCLSID( FRL_STR("{77841F96-2135-4293-8BEF-A288AAD2DBBC}") );
		toServer.setVendor( FRL_STR("Serg Baburin") );
		toServer.setDriverName( FRL_STR("PSOI2 device") );
		toServer.setDescription( FRL_STR("OPC server for PSOI2 device from Serg Baburin"));
		toServer.setVersion( 0.1 );
	}		
}
