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
				root = curPos = opcAddressSpace.getRootBranch();				
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
				if( getCurPosPath().size() )
					curPos = curPos->getBranch( getCurPosPath() + opcAddressSpace.getDelimiter() + path );
				else
					curPos = curPos->getBranch( path );
			}

			frl::String AddrSpaceCrawler::getCurPosPath()
			{
				return curPos->getID();
			}

			void AddrSpaceCrawler::goTo( const String &fullPath )
			{
				curPos = opcAddressSpace.getBranch( fullPath );
			}

			void AddrSpaceCrawler::browseBranches( std::vector< String > &branches )
			{
				if( branches.size() )
					branches.erase( branches.begin(), branches.end() );
				curPos->browseBranches( branches );
			}

			void AddrSpaceCrawler::browseLeafs( std::vector< String > &leafs, DWORD accessFilter /*= 0 */ )
			{
				if( leafs.size() )
					leafs.erase( leafs.begin(), leafs.end() );
				curPos->browseLeafs( leafs, accessFilter );
			}
		} // namespace address_space
	} // namespace opc
} // namespace frl

#endif // FRL_PLATFORM_WIN32
