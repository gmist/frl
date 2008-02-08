#include "frl_opc.h"

int _tmain( int , _TCHAR* )
{
	using namespace frl;
	
	frl::opc::opcAddressSpace.finalConstruct( FRL_STR("."));
	frl::opc::opcAddressSpace.addBranch( FRL_STR( "branch1" ) );
	frl::opc::opcAddressSpace.addBranch( FRL_STR( "branch2" ) );
	frl::opc::opcAddressSpace.addBranch( FRL_STR( "branch1.branch11" ) );
	frl::opc::opcAddressSpace.addBranch( FRL_STR( "branch1.branch11.branch111" ) );

	frl::opc::opcAddressSpace.addLeaf( FRL_STR("rootLeaf"));
	frl::opc::address_space::Tag *tag = frl::opc::opcAddressSpace.getLeaf( FRL_STR("rootLeaf") );
	tag->isWriteable( False );
	tag->setCanonicalDataType( VT_R4 );
	tag->write( 0.1 );

	frl::opc::opcAddressSpace.addLeaf( FRL_STR("rootLeaf-writable"));
	tag = frl::opc::opcAddressSpace.getLeaf( FRL_STR("rootLeaf-writable") );
	tag->isWriteable( True );
	tag->setCanonicalDataType( VT_R4 );
	tag->write( 0.0 );

	frl::opc::opcAddressSpace.addLeaf( FRL_STR("rootLeaf1") );
	tag = frl::opc::opcAddressSpace.getLeaf( FRL_STR("rootLeaf1") );
	tag->setCanonicalDataType( VT_BSTR );
	tag->write( String(FRL_STR( "qwerty" )) );

	frl::opc::opcAddressSpace.addLeaf( FRL_STR( "branch1.leaf11" ) );
	tag = frl::opc::opcAddressSpace.getLeaf( FRL_STR( "branch1.leaf11" ) );
	tag->setCanonicalDataType( VT_BOOL );
	tag->write( true );

	frl::opc::opcAddressSpace.addLeaf( FRL_STR( "branch1.branch11.leaf111" ) );
	tag = frl::opc::opcAddressSpace.getLeaf( FRL_STR( "branch1.branch11.leaf111" ) );
	tag->setCanonicalDataType( VT_BOOL );
	tag->write( true );

	opc::DA2Server server( opc::ServerTypes::localSever32 );
	server.setCLSID( FRL_STR("{251D3C74-535C-4ea4-A0FE-5FF3921DF3BB}") );
	server.setVendor( FRL_STR("Serg Baburin") );
	server.setDriverName( FRL_STR("SERVER_TEST") );
	server.setDescription( FRL_STR("Test OPC server from Serg Baburin"));
	server.setVersion( 0.1 );
	server.registrerServer();
	server.init();

	while(  true )
	{
		::Sleep(100);
	}
	return 0;
}
