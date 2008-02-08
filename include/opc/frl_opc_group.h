#ifndef frl_opc_group_h_
#define frl_opc_group_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <map>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_item_mgt.h"
#include "opc/frl_opc_group_state_mgt.h"
#include "opc/frl_opc_sync_io.h"
#include "opc/frl_opc_async_io2.h"
#include "opc/frl_opc_group_item.h"
#include "opc/frl_opc_connection_point_container.h"
#include "opc/frl_opc_timer.h"
#include "opc/frl_opc_async_request.h"
#include "frl_lock.h"
#include "frl_non_copyable.h"
#include "os/win32/com//frl_os_win32_com_allocator.h"

namespace frl
{
	namespace opc
	{
		class Group :
			public GroupStateMgt< Group >,
			public ItemMgt< Group >,
			public SyncIO< Group >,
			public AsyncIO2< Group >,
			public ConnectionPointContainer,
			private NonCopyable,
			public os::win32::com::Allocator
		{
		friend class GroupStateMgt< Group >;
		friend class ItemMgt< Group >;
		friend class SyncIO< Group >;
		friend class AsyncIO2< Group >;
		private:
			volatile LONG refCount; // reference counter
			const CLSID* clsid;

			String name;
			OPCHANDLE serverHandle;
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
		
			lock::Mutex groupGuard;
			std::map<OPCHANDLE, GroupItem* > itemList;
			std::list< AsyncRequest* > asyncReadList;
			std::list< AsyncRequest* > asyncWriteList;
			std::list< AsyncRequest* > asyncRefreshList;

			Timer< Group > timerRead;
			Timer< Group > timerWrite;
			Timer< Group > timerRefresh;
			Timer< Group > timerUpdate;

			lock::Event readEvent;
			lock::Event writeEvent;
			lock::Event refreshEvent;
		protected:
			REFCLSID GetCLSID();
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
			Group( const Group &group );
			~Group(); // Destructor
			void Init();
			OPCHANDLE getServerHandle();
			void setServerPtr( OPCServer *serverPtr );
			const String getName();
			Bool isDeleted();
			void isDeleted( Bool deleteFlag );
			void setName( const String &newName );
			LONG getRefCount();
			void onReadTimer();
			void onWriteTimer();
			void onRefreshTimer();
			void onUpdateTimer();
			void doAsyncRead( IOPCDataCallback** callBack, const AsyncRequest *request );
			void doAsyncRefresh( IOPCDataCallback** callBack, const AsyncRequest *request );
			void doAsyncWrite( IOPCDataCallback** callBack, const AsyncRequest *request );
			Group* clone();
		}; // class Group
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_group_h_ */
