#ifndef frl_opc_group_base_h_
#define frl_opc_group_base_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <boost/noncopyable.hpp>
#include "opc/frl_opc_serv_handle_counter.h"
#include "opc/frl_opc_connection_point_container.h"
#include "opc/frl_opc_group_item.h"
#include "opc/frl_opc_async_request.h"

namespace frl{ namespace opc{

class OPCServer;
class GroupBase
	:	virtual public ServerHandleCounter,
		virtual public ConnectionPointContainer,
		private boost::noncopyable
{
protected:
	String name;
	OPCHANDLE clientHandle;
	OPCServer* server;

	Bool actived;
	Bool enabled;
	Bool deleted;
	DWORD updateRate;
	LONG timeBias;
	FLOAT deadband;
	DWORD localeID;
	DWORD keepAlive;
	FILETIME lastUpdate;

	boost::mutex groupGuard;
	GroupItemElemList itemList;
public:
	GroupBase();
	GroupBase( const String &groupName );
	virtual ~GroupBase();
	void Init();
	void setServerPtr( OPCServer *serverPtr );
	const String getName();
	Bool isDeleted();
	void isDeleted( Bool deleteFlag );
	void setName( const String &newName );
	OPCHANDLE getClientHandle();
	DWORD getUpdateRate();
	FILETIME getLastUpdate();
	ULONGLONG getLastUpdateTick();
	void renewUpdateRate();
	void onUpdateTimer();
	void doAsyncRead( IOPCDataCallback* callBack, const AsyncRequestListElem &request );
	void doAsyncRefresh( const AsyncRequestListElem &request );
	void doAsyncWrite( IOPCDataCallback* callBack, const AsyncRequestListElem &request );
};

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_group_base_h_
