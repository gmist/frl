#ifndef frl_opc_group_h_
#define frl_opc_group_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <map>
#include <boost/thread/mutex.hpp>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_item_mgt.h"
#include "opc/frl_opc_group_state_mgt.h"
#include "opc/frl_opc_sync_io.h"
#include "opc/frl_opc_async_io.h"
#include "opc/frl_opc_group_item.h"
#include "opc/frl_opc_connection_point_container.h"
#include "opc/frl_opc_timer.h"
#include "opc/frl_opc_async_request.h"
#include <boost/noncopyable.hpp>
#include "os/win32/com/frl_os_win32_com_allocator.h"
#include "opc/frl_opc_item_deadband_mgt.h"
#include "opc/frl_opc_serv_handle_counter.h"

namespace frl
{
namespace opc
{

class Group :
	public ServerHandleCounter,
	public GroupStateMgt< Group >,
	public ItemMgt< Group >,
	public SyncIO< Group >,
	public AsyncIO< Group >,
	public ConnectionPointContainer,
	private boost::noncopyable,
	public os::win32::com::Allocator,
	public ItemDeadBandMgt< Group >
{
friend class GroupStateMgt< Group >;
friend class ItemMgt< Group >;
friend class SyncIO< Group >;
friend class AsyncIO< Group >;
friend class ItemDeadBandMgt< Group >;
private:
	// reference counter
	#if( FRL_COMPILER == FRL_COMPILER_MSVC )
	volatile LONG refCount;
	#else
	LONG refCount;
	#endif

	const CLSID* clsid;
	String name;
	OPCHANDLE clientHandle;
	OPCServer *server;

	Bool actived;
	Bool enabled;
	Bool deleted;
	DWORD updateRate;
	LONG timeBias;
	FLOAT deadband;
	DWORD localeID;
	DWORD keepAlive;
	FILETIME lastUpdate;
	LONGLONG tickOffset;

	boost::mutex groupGuard;
	GroupItemElemList itemList;

	Timer< Group > timerUpdate;

public:
	// IUnknown implementation
	HRESULT STDMETHODCALLTYPE 
	QueryInterface( /* [in] */ REFIID iid, /* [iid_is][out] */ void** ppInterface );
	ULONG STDMETHODCALLTYPE AddRef( void);
	ULONG STDMETHODCALLTYPE Release( void);
	HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown** ippUnknown, const CLSID* pClsid);

	// Constructors
	Group();
	Group( const String &groupName );
	~Group(); // Destructor
	void Init();
	void setServerPtr( OPCServer *serverPtr );
	const String getName();
	Bool isDeleted();
	void isDeleted( Bool deleteFlag );
	void setName( const String &newName );
	LONG getRefCount();
	void onUpdateTimer();
	void doAsyncRead( IOPCDataCallback* callBack, const AsyncRequestListElem &request );
	void doAsyncRefresh( const AsyncRequestListElem &request );
	void doAsyncWrite( IOPCDataCallback* callBack, const AsyncRequestListElem &request );
	Group* clone();
	OPCHANDLE getClientHandle();
}; // class Group

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif /* frl_opc_group_h_ */
