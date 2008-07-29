#ifndef frl_opc_addr_space_crawler_h_
#define frl_opc_addr_space_crawler_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <vector>
#include "frl_types.h"
#include "frl_exception.h"

namespace frl{ namespace opc{ namespace address_space{

class Tag;
struct TagBrowseInfo;
class AddrSpaceCrawler
{
private:
	Tag *curPos;
	Tag *root;
public:

	AddrSpaceCrawler();
	
	void goToRoot();

	Bool goUp();

	void goDown( const String &path );

	void goTo( const String &fullPath );

	String getCurPosPath();

	void browseBranches( std::vector< String > &branches );

	void browseLeafs( std::vector< String > &leafs, DWORD accessFilter = 0 );
	
	void browseLeafs( std::vector< TagBrowseInfo > &leafsArr );
	
	void browseBranches( std::vector< TagBrowseInfo > &branchesArr );
	
	void browse( std::vector< TagBrowseInfo > &arr );

}; // class AddrSpaceCrawler

} // namespace address_space
} // namespace opc
} // namespace frl

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_addr_space_crawler_h_
