#include "opc/address_space/frl_opc_address_space.h"
#include "frl_unit_test.h"
#include <conio.h>

using namespace frl;
using namespace frl::opc::address_space;

frl::Bool createBranchInRootBranch()
{
	try
	{
		const String branchName = FRL_STR("branch1");
		AddressSpace addressSpace;
		addressSpace.finalConstruct( FRL_STR(".") );
		addressSpace.addBranch( branchName );
		if( addressSpace.getBranch( branchName )->getID() == branchName )
			return True;
	}
	catch( frl::Exception& )
	{
		return False;
	}
	return False;
}

frl::Bool createBranchInBranch()
{
	try
	{
		const String branchName = FRL_STR( "branch1" );
		const String subBranchName = FRL_STR( "branch1.branch11" );
		AddressSpace addressSpace;
		addressSpace.finalConstruct( FRL_STR(".") );
		
		addressSpace.addBranch( branchName );
		addressSpace.addBranch(subBranchName );
		if( addressSpace.getBranch( subBranchName )->getID() == subBranchName )
			return True;
	}
	catch( frl::Exception& )
	{
		return False;
	}
	return False;
}

frl::Bool createSubBranchInNotExistBranch()
{	
	try
	{
		AddressSpace addressSpace;
		addressSpace.finalConstruct( FRL_STR(".") );
		addressSpace.addBranch( FRL_STR("brach1.branch11") );
	}
	catch( Tag::NotExistTag& )
	{
		return True;
	}
	catch( ... )
	{
		return False;
	}
	return False;
}


frl::Bool createLeafInRootBranch()
{
	try
	{
		const String leafName = FRL_STR("leaf1");
		AddressSpace addressSpace;
		addressSpace.finalConstruct( FRL_STR(".") );
		addressSpace.addLeaf( leafName );
		if( addressSpace.getLeaf( leafName )->getID() == leafName )
			return True;
	}
	catch( frl::Exception& )
	{
		return False;
	}
	return False;
}

frl::Bool createLeafInSubBranch()
{
	try
	{
		const String branchName = FRL_STR("branch1");
		const String leafName = FRL_STR("leaf1");
		AddressSpace addressSpace;
		addressSpace.finalConstruct( FRL_STR(".") );
		addressSpace.addBranch(branchName);
		addressSpace.addLeaf( branchName + addressSpace.getDelimiter() + leafName );
		if( addressSpace.getLeaf( branchName + addressSpace.getDelimiter() + leafName )->getID() 
			== branchName + addressSpace.getDelimiter() + leafName )
			return True;
	}
	catch( frl::Exception& )
	{
		return False;
	}
	return False;
}

frl::Bool createLeafInNotExistBranch()
{	
	try
	{
		AddressSpace addressSpace;
		addressSpace.finalConstruct( FRL_STR(".") );
		addressSpace.addLeaf( FRL_STR( "branch1.leaf1" ) );
	}
	catch( Tag::NotExistTag& )
	{
		return True;
	}
	catch( ... )
	{
		return False;
	}
	return False;
}


frl::Bool getFullIDLeaf()
{
	try
	{
		const String branchName = FRL_STR("branch1");
		const String leafName = FRL_STR("leaf1");
		AddressSpace addressSpace;
		addressSpace.finalConstruct( FRL_STR(".") );
		const String fullName = branchName + addressSpace.getDelimiter() + leafName;
		addressSpace.addBranch(branchName);
		addressSpace.addLeaf( branchName + addressSpace.getDelimiter() + leafName );
		String name = addressSpace.getLeaf( fullName )->getID();
		if( addressSpace.getLeaf( fullName )->getID() == fullName )
			return True;
	}
	catch( frl::Exception& )
	{
		return False;
	}
	return False;
}

frl::Bool doubleCreateBranch()
{
	const String branchName = FRL_STR("branch1");
	try
	{
		AddressSpace addressSpace;
		addressSpace.finalConstruct( FRL_STR(".") );
		addressSpace.addBranch( branchName );
		addressSpace.addBranch( branchName );
	}
	catch( Tag::IsExistTag& )
	{
		return True;
	}
	catch( ... )
	{
		return False;
	}
	return False;
}

frl::Bool doubleCreateSubBranch()
{
	const String branchName = FRL_STR("branch1");
	const String subBranchName = FRL_STR("branch11");
	try
	{
		AddressSpace addressSpace;
		addressSpace.finalConstruct( FRL_STR(".") );
		addressSpace.addBranch( branchName );
		addressSpace.addBranch( branchName + addressSpace.getDelimiter() + subBranchName );
		addressSpace.addBranch( branchName + addressSpace.getDelimiter() + subBranchName );
	}
	catch( Tag::IsExistTag& )
	{
		return True;
	}
	catch( ... )
	{
		return False;
	}
	return False;
}

frl::Bool createInvalidNameBranch1()
{
	const String branchName = FRL_STR("branch1");
	const String subBranchName = FRL_STR(".branch11");
	try
	{
		AddressSpace addressSpace;
		addressSpace.finalConstruct( FRL_STR(".") );
		addressSpace.addBranch( branchName );
		addressSpace.addBranch( branchName + addressSpace.getDelimiter() + subBranchName );
	}
	catch( frl::opc::address_space::AddressSpace::InvalidBranchName& )
	{
		return True;
	}
	catch( ... )
	{
		return False;
	}
	
	return False;
}

frl::Bool createInvalidNameBranch2()
{
	const String branchName = FRL_STR("branch1");
	const String subBranchName = FRL_STR("branch11.");	
	try
	{
		AddressSpace addressSpace;
		addressSpace.finalConstruct( FRL_STR(".") );
		addressSpace.addBranch( branchName );
		addressSpace.addBranch( branchName + addressSpace.getDelimiter() + subBranchName );
	}
	catch( frl::opc::address_space::AddressSpace::InvalidBranchName& )
	{
		return True;
	}
	catch( ... )
	{
		return False;
	}

	return False;
}

frl::Bool createDoubleLeaf()
{
	try
	{
		AddressSpace addressSpace;
		addressSpace.finalConstruct( FRL_STR(".") );
		addressSpace.addLeaf( FRL_STR( "leaf1" ) );
		addressSpace.addLeaf( FRL_STR( "leaf1" ) );
	}
	catch( Tag::IsExistTag& )
	{
		return True;
	}
	return False;
}

int main( int argc, char *argv[] )
{
	frl::unit_test::ConsoleUnitTest test( FRL_STR("frl::opc::address_space test") );
	test.Add( createBranchInRootBranch );
	test.Add( createBranchInBranch );
	test.Add( createSubBranchInNotExistBranch );
	test.Add( createLeafInRootBranch );
	test.Add( createLeafInSubBranch );
	test.Add( getFullIDLeaf );
	test.Add( doubleCreateBranch );
	test.Add( doubleCreateSubBranch );
	test.Add( createInvalidNameBranch1 );
	test.Add( createInvalidNameBranch2 );
	test.Add( createLeafInNotExistBranch );
	test.Add( createDoubleLeaf );
	return 0;
}
