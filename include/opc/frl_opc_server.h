#ifndef frl_opc_server_h_
#define frl_opc_server_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <map>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_common.h"
#include "opc/frl_opc_item_properties.h"
#include "opc/frl_opc_browse_server_address_space.h"
#include "opc/frl_opc_item_mgt.h"
#include "frl_lock.h"
#include "opc/frl_opc_group_state_mgt.h"
#include "opc/frl_opc_connection_point_container.h"
#include "frl_non_copyable.h"

namespace frl
{
	namespace opc
	{
		class Group;
		template < class T>  class  GroupStateMgt;
		class OPCServer
			:	public OPCCommon,
				public IOPCServer,
				public ItemProperties< OPCServer >,
				public BrowseServerAddressSpace< OPCServer >,
				public ConnectionPointContainer,
				private NonCopyable
		{
		friend GroupStateMgt<Group>;

		private:
			std::map< OPCHANDLE, frl::opc::Group* > groupItem;
			std::map< String, OPCHANDLE > groupItemIndex;
			lock::Mutex scopeGuard;
			volatile LONG refCount;

		public:
			OPCSERVERSTATUS m_ServerStatus;

			OPCServer();
			Bool setGroupName( const String &oldName, const String &newName );
			HRESULT cloneGroup( const String &name, const String &cloneName, Group **group );
			HRESULT addNewGroup( Group **group );

			// IUnknown implementation
			STDMETHODIMP QueryInterface( REFIID iid, LPVOID* ppInterface);
			STDMETHODIMP_(ULONG) AddRef( void);
			STDMETHODIMP_(ULONG) Release( void);

			// IOPCServer implementation
			HRESULT STDMETHODCALLTYPE AddGroup( 
				/* [string][in] */ LPCWSTR szName,
				/* [in] */ BOOL bActive,
				/* [in] */ DWORD dwRequestedUpdateRate,
				/* [in] */ OPCHANDLE hClientGroup,
				/* [in][unique] */ LONG *pTimeBias,
				/* [in][unique] */ FLOAT *pPercentDeadband,
				/* [in] */ DWORD dwLCID,
				/* [out] */ OPCHANDLE *phServerGroup,
				/* [out] */ DWORD *pRevisedUpdateRate,
				/* [in] */ REFIID riid,
				/* [iid_is][out] */ LPUNKNOWN *ppUnk);

			HRESULT STDMETHODCALLTYPE GetErrorString( 
				/* [in] */ HRESULT dwError,
				/* [in] */ LCID dwLocale,
				/* [string][out] */ LPWSTR *ppString);

			HRESULT STDMETHODCALLTYPE GetGroupByName( 
				/* [string][in] */ LPCWSTR szName,
				/* [in] */ REFIID riid,
				/* [iid_is][out] */ LPUNKNOWN *ppUnk);

			HRESULT STDMETHODCALLTYPE GetStatus( 
				/* [out] */ OPCSERVERSTATUS **ppServerStatus);

			HRESULT STDMETHODCALLTYPE RemoveGroup( 
				/* [in] */ OPCHANDLE hServerGroup,
				/* [in] */ BOOL bForce);

			HRESULT STDMETHODCALLTYPE CreateGroupEnumerator( 
				/* [in] */ OPCENUMSCOPE dwScope,
				/* [in] */ REFIID riid,
				/* [iid_is][out] */ LPUNKNOWN *ppUnk);

		}; // OPCServer
	} // namespace opc
} // namespace FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_server_h_
