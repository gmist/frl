#ifndef frl_opc_server_base_h_
#define frl_opc_server_base_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <boost/noncopyable.hpp>
#include "opc/address_space/frl_opc_addr_space_crawler.h"
#include "opc/frl_opc_group_manager.h"
#include "opc/frl_opc_request_manager.h"

namespace frl{ namespace opc{

class OPCServerBase : private boost::noncopyable
{
protected:
	GroupManager group_manager;
	RequestManager request_manager;
	address_space::AddrSpaceCrawler crawler;
	boost::mutex scopeGuard;

public:
	virtual ~OPCServerBase();
	HRESULT setGroupName( const String &oldName, const String &newName );
	GroupElem cloneGroup( String &name , String &to_name );
	void addAsyncRequest( AsyncRequestListElem &request );
	Bool asyncRequestCancel( DWORD id );
	void removeItemFromRequestList( OPCHANDLE handle_ );
	void removeGroupFromRequestList( OPCHANDLE group_handle );
};

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_server_base_h_
