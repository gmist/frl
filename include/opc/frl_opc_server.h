#ifndef frl_opc_server_h_
#define frl_opc_server_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <map>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "lock/frl_mutex.h"
#include "opc/frl_opc_common.h"
#include "opc/frl_opc_item_properties.h"
#include "opc/frl_opc_browse_server_address_space.h"
#include "opc/frl_opc_connection_point_container.h"
#include "os/win32/com/frl_os_win32_com_allocator.h"
#include "frl_non_copyable.h"
#include "opc/address_space/frl_opc_addr_space_crawler.h"
#include "opc/frl_opc_browse.h"
#include "opc/frl_opc_item_io.h"

namespace frl
{
namespace opc
{
class Group;

class OPCServer
	:	public OPCCommon,
		public IOPCServer,
		public ItemProperties,
		public BrowseServerAddressSpace< OPCServer >,
		public ConnectionPointContainer,
		private NonCopyable,
		public os::win32::com::Allocator,
		public BrowseImpl< OPCServer >,
		public ItemIO< OPCServer >
{
friend class BrowseServerAddressSpace< OPCServer >;
friend class BrowseImpl< OPCServer >;
friend class ItemIO< OPCServer >;

private:
	// reference counter
	#if( FRL_COMPILER == FRL_COMPILER_MSVC )
	volatile LONG refCount;
	#else
	LONG refCount;
	#endif

	std::map< OPCHANDLE, frl::opc::Group* > groupItem;
	std::map< String, frl::opc::Group* > groupItemIndex;
	lock::Mutex scopeGuard;
	OPCSERVERSTATUS serverStatus;
	address_space::AddrSpaceCrawler crawler;

public:
	FRL_EXCEPTION_CLASS( InvalidServerState );

	OPCServer();
	~OPCServer();
	HRESULT setGroupName( const String &oldName, const String &newName );
	HRESULT cloneGroup( const String &name, const String &cloneName, Group **group );
	HRESULT addNewGroup( Group **group );
	void setServerState( OPCSERVERSTATE newState );
	OPCSERVERSTATE getServerState();

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
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_server_h_
