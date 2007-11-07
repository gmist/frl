#ifndef frl_opc_server_h_
#define frl_opc_server_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <atlbase.h>
#include <atlcom.h>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_common.h"
#include "opc/frl_opc_item_properties.h"
#include "opc/frl_opc_browse_server_address_space.h"

namespace frl
{
	namespace opc
	{
		class OPCServer
			:	public CComObjectRootEx<CComMultiThreadModel>,
				public CComCoClass<OPCServer>,
				public OPCCommon,
				public IConnectionPointContainerImpl<OPCServer>,
				public IConnectionPointImpl< OPCServer, &__uuidof(IOPCShutdown) >,
				public IOPCServer,
				public ItemProperties< OPCServer >,
				public BrowseServerAddressSpace< OPCServer >
		{		
		public:

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

		};
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_server_h_ */
