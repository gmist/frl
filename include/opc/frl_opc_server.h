#ifndef frl_opc_server_h_
#define frl_opc_server_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <atlbase.h>
#include <atlcom.h>
#include <Guiddef.h>
#include <map>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_common.h"
#include "opc/frl_opc_item_properties.h"
#include "opc/frl_opc_browse_server_address_space.h"
#include "opc/frl_opc_item_mgt.h"
#include "frl_lock.h"
#include "opc/frl_opc_group_state_mgt.h"

namespace frl
{
	namespace opc
	{
		class Group;
		template < class T>  class  GroupStateMgt;
		class OPCServer
			:	public CComObjectRootEx<CComMultiThreadModel>,
				public CComCoClass<OPCServer>,
				public OPCCommon,
				public IOPCServer,
				public IConnectionPointContainerImpl<OPCServer>,
				public IConnectionPointImpl< OPCServer, &__uuidof(IOPCShutdown)>,
				public ItemProperties< OPCServer >,
				public BrowseServerAddressSpace< OPCServer >
		{
		friend Group;
		friend GroupStateMgt<Group>;
		private:
			std::map< OPCHANDLE, frl::opc::Group* > groupItem;
			std::map< String, OPCHANDLE > groupItemIndex;
			lock::Mutex scopeGuard;
		public:
			OPCSERVERSTATUS m_ServerStatus;

			BEGIN_CATEGORY_MAP(OPCServer)
				IMPLEMENTED_CATEGORY(CATID_OPCDAServer20)
			END_CATEGORY_MAP()

			BEGIN_COM_MAP(OPCServer)
				COM_INTERFACE_ENTRY(IOPCCommon)
				COM_INTERFACE_ENTRY(IOPCServer)
				COM_INTERFACE_ENTRY(IOPCItemProperties)
				COM_INTERFACE_ENTRY(IOPCBrowseServerAddressSpace)
				COM_INTERFACE_ENTRY(IConnectionPointContainer)
			END_COM_MAP()

			BEGIN_CONNECTION_POINT_MAP(OPCServer)
				CONNECTION_POINT_ENTRY(__uuidof(IOPCShutdown))
			END_CONNECTION_POINT_MAP()

			OPCServer();
			Bool setGroupName( const String &oldName, const String &newName );
			HRESULT cloneGroup( const String &name, const String &cloneName, Group **group );
			HRESULT addNewGroup( Group **group );

			//////////////////////////////////////////////////////////////////////////
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

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_server_h_ */
