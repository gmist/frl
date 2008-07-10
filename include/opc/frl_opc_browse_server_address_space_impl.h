#ifndef frl_opc_browse_server_address_space_impl_h_
#define frl_opc_browse_server_address_space_impl_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <boost/thread/mutex.hpp>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_server_base.h"

namespace frl
{
namespace opc
{

class BrowseServerAddressSpaceImpl
	:	virtual public IOPCBrowseServerAddressSpace,
		virtual public opc::OPCServerBase
{
private:
	boost::mutex bsaScopeGuard;
public:

	virtual ~BrowseServerAddressSpaceImpl();

	// IOPCBrowseServerAddressSpace implementation
	HRESULT STDMETHODCALLTYPE QueryOrganization( 
		/* [out] */ OPCNAMESPACETYPE *pNameSpaceType );

	HRESULT STDMETHODCALLTYPE ChangeBrowsePosition( 
		/* [in] */ OPCBROWSEDIRECTION dwBrowseDirection,
		/* [string][in] */ LPCWSTR szString );

	HRESULT STDMETHODCALLTYPE BrowseOPCItemIDs( 
		/* [in] */ OPCBROWSETYPE dwBrowseFilterType,
		/* [string][in] */ LPCWSTR szFilterCriteria,
		/* [in] */ VARTYPE vtDataTypeFilter,
		/* [in] */ DWORD dwAccessRightsFilter,
		/* [out] */ LPENUMSTRING *ppIEnumString );

	HRESULT STDMETHODCALLTYPE GetItemID( 
		/* [in] */ LPWSTR szItemDataID,
		/* [string][out] */ LPWSTR *szItemID );

	HRESULT STDMETHODCALLTYPE BrowseAccessPaths( 
		/* [string][in] */ LPCWSTR szItemID,
		/* [out] */ LPENUMSTRING *ppIEnumString );
};

}
}

#endif // FRL_PLATFORM_WIN32
#endif /* frl_opc_browse_server_address_space_impl_h_ */
