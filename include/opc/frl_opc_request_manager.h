#ifndef frl_opc_request_manager_h_
#define frl_opc_request_manager_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <map>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/noncopyable.hpp>
#include "opc/frl_opc_async_request.h"

namespace frl
{
namespace opc
{

class RequestManager : private boost::noncopyable
{
private:
	std::map< OPCHANDLE, AsyncRequestListElem > request_map;
	boost::thread_group *thr_group;
	boost::mutex scopeGuard;
	void forceCleanup();
public:
	RequestManager();
	~RequestManager();
	void addReadRequest( AsyncRequestListElem &request );
	void addWriteRequest( AsyncRequestListElem &request );
	bool cancelRequest( OPCHANDLE handle );
	void doAsyncRead( AsyncRequestListElem &request );
	void doAsyncWrite( AsyncRequestListElem &request );
	void removeItemFromRequest( OPCHANDLE item_id );
	void removeGroupFromRequest( OPCHANDLE group_id );
}; // class RequestManager

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_request_manager_h_
