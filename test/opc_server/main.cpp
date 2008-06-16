#include <tchar.h>
#include "frl_opc.h"

int main( int argc, char *argv[] )
{
	using namespace frl;

	opc::opcAddressSpace::getInstance().finalConstruct( FRL_STR("."));
	opc::opcAddressSpace::getInstance().addBranch( FRL_STR( "branch1" ) );
	opc::opcAddressSpace::getInstance().addBranch( FRL_STR( "branch2" ) );
	opc::opcAddressSpace::getInstance().addBranch( FRL_STR( "branch1.branch11" ) );
	opc::opcAddressSpace::getInstance().addBranch( FRL_STR( "branch1.branch11.branch111" ) );

	opc::opcAddressSpace::getInstance().addLeaf( FRL_STR("rootLeaf"));
	opc::address_space::Tag *tag = opc::opcAddressSpace::getInstance().getLeaf( FRL_STR("rootLeaf") );
	tag->isWritable( False );
	tag->setCanonicalDataType( VT_R4 );
	tag->write( 0.1 );

	opc::opcAddressSpace::getInstance().addLeaf( FRL_STR("rootLeaf-writable"));
	tag = opc::opcAddressSpace::getInstance().getLeaf( FRL_STR("rootLeaf-writable") );
	tag->isWritable( True );
	tag->setCanonicalDataType( VT_R4 );
	tag->write( 0.0 );

	opc::opcAddressSpace::getInstance().addLeaf( FRL_STR("rootLeaf1") );
	tag = opc::opcAddressSpace::getInstance().getLeaf( FRL_STR("rootLeaf1") );
	tag->setCanonicalDataType( VT_BSTR );
	tag->write( String(FRL_STR( "qwerty" )) );

	opc::opcAddressSpace::getInstance().addLeaf( FRL_STR( "branch1.leaf11" ) );
	tag = opc::opcAddressSpace::getInstance().getLeaf( FRL_STR( "branch1.leaf11" ) );
	tag->setCanonicalDataType( VT_BOOL );
	tag->write( true );

	opc::opcAddressSpace::getInstance().addLeaf( FRL_STR( "branch1.branch11.leaf111" ) );
	tag = opc::opcAddressSpace::getInstance().getLeaf( FRL_STR( "branch1.branch11.leaf111" ) );
	tag->setCanonicalDataType( VT_BOOL );
	tag->write( true );

	opc::DAServer server( opc::ServerTypes::localSever32 );
	server.setCLSID( FRL_STR("{251D3C74-535C-4ea4-A0FE-5FF3921DF3BB}") );
	server.setVendor( FRL_STR("Serg Baburin") );
	server.setDriverName( FRL_STR("SERVER_TEST") );
	server.setDescription( FRL_STR("Test OPC server from Serg Baburin"));
	server.setVersion( 0.1 );
	server.registrerServer3();
	server.init();

	for( ; ; )
		::Sleep(100);
}
