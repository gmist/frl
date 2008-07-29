#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/address_space/frl_opc_addr_space_crawler.h"
#include "opc/address_space/frl_opc_address_space.h"

namespace frl
{
namespace opc
{
namespace address_space
{

AddrSpaceCrawler::AddrSpaceCrawler()
{
	root = curPos = opcAddressSpace::getInstance().getRootBranch();				
}

void AddrSpaceCrawler::goToRoot()
{
	curPos = root;
}

frl::Bool AddrSpaceCrawler::goUp()
{
	if( curPos == root )
		return False;
	curPos = curPos->getParent();
	return True;
}

void AddrSpaceCrawler::goDown( const String &path )
{
	if( ! getCurPosPath().empty() )
		curPos = curPos->getBranch( getCurPosPath() + opcAddressSpace::getInstance().getDelimiter() + path );
	else
		curPos = curPos->getBranch( path );
}

frl::String AddrSpaceCrawler::getCurPosPath()
{
	return curPos->getID();
}

void AddrSpaceCrawler::goTo( const String &fullPath )
{
	curPos = opcAddressSpace::getInstance().getBranch( fullPath );
}

void AddrSpaceCrawler::browseBranches( std::vector< String > &branches )
{
	branches.clear();
	curPos->browseBranches( branches );
}

void AddrSpaceCrawler::browseBranches( std::vector< TagBrowseInfo > &branchesArr )
{
	branchesArr.clear();
	curPos->browseBranches( branchesArr );
}

void AddrSpaceCrawler::browseLeafs( std::vector< String > &leafs, DWORD accessFilter /*= 0 */ )
{
	leafs.clear();
	curPos->browseLeafs( leafs, accessFilter );
}

void AddrSpaceCrawler::browseLeafs( std::vector< TagBrowseInfo > &leafsArr )
{
	leafsArr.clear();
	curPos->browseLeafs( leafsArr );
}

void AddrSpaceCrawler::browse( std::vector< TagBrowseInfo >& arr )
{
	std::vector< TagBrowseInfo > tmp;
	curPos->browseLeafs( tmp );
	arr.assign( tmp.begin(), tmp.end() );
	tmp.clear();
	curPos->browseBranches( tmp );
	arr.insert( arr.end(), tmp.begin(), tmp.end() );
}

} // namespace address_space
} // namespace opc
} // namespace frl

#endif // FRL_PLATFORM_WIN32
