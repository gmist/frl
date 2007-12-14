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
#include "frl_lock.h"

namespace frl
{
	namespace opc
	{
		class OPCServer;

		class Group :
			public GroupStateMgt< Group >,
			public ItemMgt< Group >,
			public SyncIO< Group >,
			public AsyncIO2< Group >
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
		protected:
			REFCLSID GetCLSID();
		public:
			// IUnknown implementation
			HRESULT STDMETHODCALLTYPE 
			QueryInterface( /* [in] */ REFIID iid, /* [iid_is][out] */ void** ppInterface );
			ULONG STDMETHODCALLTYPE AddRef( void);
			ULONG STDMETHODCALLTYPE Release( void);
			HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown** ippUnknown, const CLSID* pClsid);
			LONG getRefCount();

			// Constructors
			Group();
			Group( const String &groupName );
			Group( const Group &group );
			~Group(); // Destructor
			void Init();
			void setServerHandle( OPCHANDLE handle );
			OPCHANDLE getServerHandle();
			void setServerPtr( OPCServer *serverPtr );
			const String getName();
			Bool isDeleted();
			void isDeleted( Bool deleteFlag );
			void setName( const String &newName );
		}; // class Group
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_group_h_ */
