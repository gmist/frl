#ifndef opc_address_space_test_suite_h_
#define opc_address_space_test_suite_h_
#include <boost/test/unit_test.hpp>
#include "opc/address_space/frl_opc_address_space.h"

BOOST_AUTO_TEST_SUITE( opc_address_space )

BOOST_AUTO_TEST_CASE( create_branch_in_root_branch )
{
	using namespace frl::opc::address_space;
	const frl::String branchName = FRL_STR("branch1");

	AddressSpace addressSpace;
	addressSpace.finalConstruct( FRL_STR(".") );

	BOOST_CHECK_NO_THROW( addressSpace.addBranch( branchName ) );
	BOOST_CHECK_NO_THROW( BOOST_CHECK( addressSpace.getBranch( branchName )->getID() == branchName ) );
}

BOOST_AUTO_TEST_CASE( create_branch_in_branch )
{
	using namespace frl::opc::address_space;
	const frl::String branchName = FRL_STR( "branch1" );
	const frl::String subBranchName = FRL_STR( "branch1.branch11" );

	AddressSpace addressSpace;
	addressSpace.finalConstruct( FRL_STR(".") );
	BOOST_CHECK_NO_THROW( addressSpace.addBranch( branchName ) );
	BOOST_CHECK_NO_THROW( addressSpace.addBranch(subBranchName ) );
	BOOST_CHECK_NO_THROW( BOOST_CHECK( addressSpace.getBranch( subBranchName )->getID() == subBranchName ) );
}

BOOST_AUTO_TEST_CASE( create_sub_branch_in_not_exist_branch )
{
	using namespace frl::opc::address_space;
	AddressSpace addressSpace;
	addressSpace.finalConstruct( FRL_STR(".") );
	BOOST_CHECK_THROW( addressSpace.addBranch( FRL_STR("brach1.branch11") ), Tag::NotExistTag );
}

BOOST_AUTO_TEST_CASE( create_leaf_in_root_branch )
{
	using namespace frl::opc::address_space;
	const frl::String branchName = FRL_STR("branch1");
	const frl::String leafName = FRL_STR("leaf1");
	
	AddressSpace addressSpace;
	addressSpace.finalConstruct( FRL_STR(".") );
	BOOST_CHECK_NO_THROW( addressSpace.addBranch(branchName) );
	BOOST_CHECK_NO_THROW( addressSpace.addLeaf( branchName + addressSpace.getDelimiter() + leafName ) );
	BOOST_CHECK_NO_THROW(
		BOOST_CHECK(
		addressSpace.getLeaf( branchName + addressSpace.getDelimiter() + leafName )->getID() 
		== branchName + addressSpace.getDelimiter() + leafName ) );
}

BOOST_AUTO_TEST_CASE( create_leaf_in_sub_branch )
{
	using namespace frl::opc::address_space;
	const frl::String branchName = FRL_STR("branch1");
	const frl::String leafName = FRL_STR("leaf1");
	AddressSpace addressSpace;
	addressSpace.finalConstruct( FRL_STR(".") );

	BOOST_CHECK_NO_THROW( addressSpace.addBranch(branchName) );
	BOOST_CHECK_NO_THROW( addressSpace.addLeaf( branchName + addressSpace.getDelimiter() + leafName ) );
	BOOST_CHECK_NO_THROW(
		BOOST_CHECK(
			addressSpace.getLeaf( branchName + addressSpace.getDelimiter() + leafName )->getID() 
			== branchName + addressSpace.getDelimiter() + leafName ) );	
}

BOOST_AUTO_TEST_CASE( create_leaf_in_not_exist_branch )
{
	using namespace frl::opc::address_space;
	AddressSpace addressSpace;
	addressSpace.finalConstruct( FRL_STR(".") );
	BOOST_CHECK_THROW( addressSpace.addLeaf( FRL_STR( "branch1.leaf1" ) ), Tag::NotExistTag );
}

BOOST_AUTO_TEST_CASE( get_full_id_leaf )
{
	using namespace frl::opc::address_space;
	const frl::String branchName = FRL_STR("branch1");
	const frl::String leafName = FRL_STR("leaf1");

	AddressSpace addressSpace;
	addressSpace.finalConstruct( FRL_STR(".") );
	const frl::String fullName = branchName + addressSpace.getDelimiter() + leafName;

	BOOST_CHECK_NO_THROW( addressSpace.addBranch(branchName) );
	BOOST_CHECK_NO_THROW( addressSpace.addLeaf( branchName + addressSpace.getDelimiter() + leafName ) );
	frl::String name;
	BOOST_CHECK_NO_THROW( name = addressSpace.getLeaf( fullName )->getID() );
	BOOST_CHECK_NO_THROW(
		BOOST_CHECK( addressSpace.getLeaf( fullName )->getID() == fullName ) );
}

BOOST_AUTO_TEST_CASE( create_double_branch )
{
	using namespace frl::opc::address_space;
	const frl::String branchName = FRL_STR("branch1");

	AddressSpace addressSpace;
	addressSpace.finalConstruct( FRL_STR(".") );
	BOOST_CHECK_NO_THROW( addressSpace.addBranch( branchName ) );
	BOOST_CHECK_THROW( addressSpace.addBranch( branchName ), Tag::IsExistTag );
}

BOOST_AUTO_TEST_CASE( create_double_sub_branch )
{
	using namespace frl::opc::address_space;
	const frl::String branchName = FRL_STR("branch1");
	const frl::String subBranchName = FRL_STR("branch11");

	AddressSpace addressSpace;
	addressSpace.finalConstruct( FRL_STR(".") );
	BOOST_CHECK_NO_THROW( addressSpace.addBranch( branchName ) );
	BOOST_CHECK_NO_THROW( addressSpace.addBranch( branchName + addressSpace.getDelimiter() + subBranchName ) );
	BOOST_CHECK_THROW( 
		addressSpace.addBranch( branchName + addressSpace.getDelimiter() + subBranchName ),
		Tag::IsExistTag );
}

BOOST_AUTO_TEST_CASE( create_branch_with_invalid_name_1 )
{
	using namespace frl::opc::address_space;
	const frl::String branchName = FRL_STR("branch1");
	const frl::String subBranchName = FRL_STR(".branch11");
	
	AddressSpace addressSpace;
	addressSpace.finalConstruct( FRL_STR(".") );
	BOOST_CHECK_NO_THROW( addressSpace.addBranch( branchName ) );
	BOOST_CHECK_THROW(
		addressSpace.addBranch( branchName + addressSpace.getDelimiter() + subBranchName ),
		AddressSpace::InvalidBranchName );
}

BOOST_AUTO_TEST_CASE( create_branch_with_invalid_name_2 )
{
	using namespace frl::opc::address_space;
	const frl::String branchName = FRL_STR("branch1");
	const frl::String subBranchName = FRL_STR("branch11.");	

	AddressSpace addressSpace;
	addressSpace.finalConstruct( FRL_STR(".") );
	BOOST_CHECK_NO_THROW( addressSpace.addBranch( branchName ) );
	BOOST_CHECK_THROW(
		addressSpace.addBranch( branchName + addressSpace.getDelimiter() + subBranchName ),
		AddressSpace::InvalidBranchName );
}

BOOST_AUTO_TEST_CASE( create_double_leaf )
{
	using namespace frl::opc::address_space;	
	AddressSpace addressSpace;
	addressSpace.finalConstruct( FRL_STR(".") );
	BOOST_CHECK_NO_THROW( addressSpace.addLeaf( FRL_STR( "leaf1" ) ) );
	BOOST_CHECK_THROW( addressSpace.addLeaf( FRL_STR( "leaf1" ) ), Tag::IsExistTag );
}

BOOST_AUTO_TEST_SUITE_END()

#endif // opc_address_space_test_suite_h_
