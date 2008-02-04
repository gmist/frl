#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "psoi2_device.h"
#include "frl_types.h"
#include "frl_lexical_cast.h"

Psoi2Device::Psoi2Device()
{
	opcAddressSpace.finalConstruct( FRL_STR("."));

	server = new frl::opc::DA2Server( frl::opc::ServerTypes::localSever32 );
	server->setCLSID( FRL_STR("{77841F96-2135-4293-8BEF-A288AAD2DBBC}") );
	server->setVendor( FRL_STR("Serg Baburin") );
	server->setDriverName( FRL_STR("PSOI2 device") );
	server->setDescription( FRL_STR("This OPC server for PSOI2 device from Serg Baburin."));
	server->setVersion( 0.1 );
	server->registrerServer();
	server->init();
}

Psoi2Device::~Psoi2Device()
{
	delete server;
}

frl::Bool Psoi2Device::init()
{
	using namespace frl;
	using namespace frl::opc;
	
	frl::opc::address_space::Tag* info = opcAddressSpace.addLeaf( FRL_STR("information"));
	info->isWriteable( False );
	info->setCanonicalDataType( VT_BSTR );
	info->write( String( FRL_STR("OPC server for PSOI2( 20 channels) device. If you to find error - please lem me know (serg.baburin@gmail.com).") ) );


	String low = FRL_STR( "channel_0" );
	String hight = FRL_STR( "channel_" );

	// Initialize address space
	for( int i = 0; i < 20; i++ )
	{
		if ( i < 10)
			opcAddressSpace.addBranch( low + lexicalCast< int >::get( i ) );
		else
			opcAddressSpace.addBranch( hight + lexicalCast<int>::get( i ) );
	}

	// set up values
	for( int i = 0; i < 20; i++ )
	{
		if ( i < 10)
			channels[i].value = opcAddressSpace.addLeaf( low
																				+ lexicalCast<int>::get( i )
																				+ opcAddressSpace.getDelimiter()
																				+ FRL_STR("value") ) ;
		else
			channels[i].value = opcAddressSpace.addLeaf( hight
																				+ lexicalCast<int>::get( i )
																				+ opcAddressSpace.getDelimiter()
																				+ FRL_STR("value") );
		channels[i].value->isWriteable( False );
		channels[i].value->setCanonicalDataType( VT_R4 );
		channels[i].value->write( 0.0f );
	}

	// set up threshold exceeding
	for( int i = 0; i < 20; i++ )
	{
		if ( i < 10)
			channels[i].thresholdExceeding = opcAddressSpace.addLeaf( low
																				+ lexicalCast<int>::get( i )
																				+ opcAddressSpace.getDelimiter()
																				+ FRL_STR("thresholdExceeding") ) ;
		else
			channels[i].thresholdExceeding = opcAddressSpace.addLeaf( hight
																				+ lexicalCast<int>::get( i )
																				+ opcAddressSpace.getDelimiter()
																				+ FRL_STR("thresholdExceeding") );
		channels[i].thresholdExceeding->isWriteable( False );
		channels[i].thresholdExceeding->setCanonicalDataType( VT_UI4 );
		channels[i].thresholdExceeding->write( 0 );
	}

	// set up type PPC
	for( int i = 0; i < 20; i++ )
	{
		if ( i < 10)
			channels[i].typePPC = opcAddressSpace.addLeaf( low
																				+ lexicalCast<int>::get( i )
																				+ opcAddressSpace.getDelimiter()
																				+ FRL_STR("type") ) ;
		else
			channels[i].typePPC = opcAddressSpace.addLeaf( hight
																				+ lexicalCast<int>::get( i )
																				+ opcAddressSpace.getDelimiter()
																				+ FRL_STR("type") );
		channels[i].typePPC->isWriteable( False );
		channels[i].typePPC->setCanonicalDataType( VT_UI4 );
		channels[i].typePPC->write( 0 );
	}

	// set up state of micro generator chlorine
	for( int i = 0; i < 20; i++ )
	{
		if ( i < 10)
			channels[i].goodMGC = opcAddressSpace.addLeaf( low
																				+ lexicalCast<int>::get( i )
																				+ opcAddressSpace.getDelimiter()
																				+ FRL_STR("goodMGC") ) ;
		else
			channels[i].goodMGC = opcAddressSpace.addLeaf( hight
																				+ lexicalCast<int>::get( i )
																				+ opcAddressSpace.getDelimiter()
																				+ FRL_STR("goodMGC") );
		channels[i].goodMGC->isWriteable( False );
		channels[i].goodMGC->setCanonicalDataType( VT_UI4 );
		channels[i].goodMGC->write( 0 );
	}
	
	// set up state PPC
	for( int i = 0; i < 20; i++ )
	{
		if ( i < 10)
			channels[i].goodPPC = opcAddressSpace.addLeaf( low
																				+ lexicalCast<int>::get( i )
																				+ opcAddressSpace.getDelimiter()
																				+ FRL_STR("goodPPC") ) ;
		else
			channels[i].goodPPC = opcAddressSpace.addLeaf( hight
																				+ lexicalCast<int>::get( i )
																				+ opcAddressSpace.getDelimiter()
																				+ FRL_STR("goodPPC") );
		channels[i].goodPPC->isWriteable( False );
		channels[i].goodPPC->setCanonicalDataType( VT_UI4 );
		channels[i].goodPPC->write( 0 );
	}

	return True;
}

void Psoi2Device::setValue( int channelNum, float value )
{
	channels[ channelNum ].value->write( value );
}

void Psoi2Device::setThresholdExceeding( int channelNum, int isThresholdExeending )
{
	channels[ channelNum ].thresholdExceeding->write( isThresholdExeending );
}

void Psoi2Device::setType( int channelNum, int type )
{
	channels[ channelNum ].typePPC->write( type );
}

void Psoi2Device::setGoodPPC( int channelNum, int isGood )
{
	channels[ channelNum].goodPPC->write( isGood );
}

void Psoi2Device::setGoodMGC( int channelNum, int isGood )
{
	channels[ channelNum ].goodMGC->write( isGood );
}
#endif // FRL_PLATFORM_WIN32