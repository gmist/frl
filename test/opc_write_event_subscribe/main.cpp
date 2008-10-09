#include "frl_opc.h"
#include "console_std/frl_iostream.h"

class SubscriberOne
{
public:
	void event()
	{
		std::cout << "Subscriber one..." << std::endl;
	}
};

class SubscriberTwo
{
public:
	void event()
	{
		std::cout << "Subscriber two..." << std::endl;
	}
};

class SubscriberThree
{
public:
	void event()
	{
		std::cout << "Subscriber three..." << std::endl;
	}
};

class SubscriberFour
{
public:
	void event()
	{
		std::cout << "Subscriber four..." << std::endl;
	}
};

void functionCallBackOne( const frl::opc::address_space::Tag* const ptr )
{
	frl::console_std::Out << FRL_STR( "================" ) << std::endl;
	frl::console_std::Out << FRL_STR( "Tag ID: " ) << ptr->getID() << std::endl;
	frl::console_std::Out << double( ptr->read() ) << std::endl;
}

void functionCallBackTwo( const frl::opc::address_space::Tag* const ptr )
{
	frl::console_std::Out << FRL_STR( "================" ) << std::endl;
	frl::console_std::Out << FRL_STR( "Tag ID: " ) << ptr->getID() << std::endl;
	frl::console_std::Out << frl::String( ptr->read() ) << std::endl;
}


int main( int argc, char *argv[] )
{
	using namespace frl;

	opc::opcAddressSpace::getInstance().finalConstruct( FRL_STR("."));
	opc::opcAddressSpace::getInstance().addBranch( FRL_STR( "branch1" ) );
	opc::opcAddressSpace::getInstance().addBranch( FRL_STR( "branch2" ) );

	opc::address_space::Tag *tag = NULL;

	tag = opc::opcAddressSpace::getInstance().addLeaf( FRL_STR("rootLeaf_1"));
	tag->isWritable( True );
	tag->setCanonicalDataType( VT_R4 );
	tag->write( 0.1 );
	SubscriberOne one;
	tag->subscribeToOpcChange( boost::function< void() >( boost::bind( &SubscriberOne::event, &one ) ) );

	tag = opc::opcAddressSpace::getInstance().addLeaf( FRL_STR("rootLeaf_2"));
	tag->isWritable( True );
	tag->setCanonicalDataType( VT_R4 );
	tag->write( 0.2 );
	SubscriberTwo two;
	tag->subscribeToOpcChange( boost::function< void() >( boost::bind( &SubscriberTwo::event, &two ) ) );

	tag = opc::opcAddressSpace::getInstance().addLeaf( FRL_STR( "branch1.bool_leaf" ) );
	tag->isWritable( True );
	tag->setCanonicalDataType( VT_BOOL );
	tag->write( true );
	SubscriberThree three;
	tag->subscribeToOpcChange( boost::function< void() >( boost::bind( &SubscriberThree::event, &three ) ) );

	tag = opc::opcAddressSpace::getInstance().addLeaf( FRL_STR( "branch2.string_leaf" ) );
	tag->isWritable( True );
	tag->setCanonicalDataType( VT_BSTR );
	tag->write( String(FRL_STR("string") ) );
	SubscriberFour four;
	tag->subscribeToOpcChange( boost::function< void() >( boost::bind( &SubscriberFour::event, &four ) ) );

	tag = opc::opcAddressSpace::getInstance().addLeaf( FRL_STR( "rootLeaf_3_call_back" ) );
	tag->isWritable( True );
	tag->setCanonicalDataType( VT_R8 );
	tag->write( 0.3 );
	tag->subscribeToOpcChange( boost::function< void( const frl::opc::address_space::Tag* const ) >( boost::bind( &functionCallBackOne, _1 ) ) );

	tag = opc::opcAddressSpace::getInstance().addLeaf( FRL_STR( "rootLeaf_4_call_back" ) );
	tag->isWritable( True );
	tag->setCanonicalDataType( VT_BSTR );
	tag->write( String( FRL_STR( "test string" ) ) );
	tag->subscribeToOpcChange( boost::function< void( const frl::opc::address_space::Tag* const ) >( boost::bind( &functionCallBackTwo, _1 ) ) );

	opc::DAServer server( opc::ServerTypes::localSever32 );
	server.setCLSID( FRL_STR("{E712B16C-963C-4aac-AFF6-67CBBE658D9C}") );
	server.setVendor( FRL_STR("Serg Baburin") );
	server.setDriverName( FRL_STR("SERVER_WRITE_EVENT_TEST") );
	server.setDescription( FRL_STR("Test OPC server write event from Serg Baburin"));
	server.setVersion( 0.1 );
	server.registrerServer3();
	server.init();

	for( ; ; )
		::Sleep(100);
}
