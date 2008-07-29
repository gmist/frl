#ifndef frl_opc_impl_browse_server_address_space_h_
#define frl_opc_impl_browse_server_address_space_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <boost/thread/mutex.hpp>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_server_base.h"

namespace frl { namespace opc { namespace impl {

class BrowseServerAddressSpace
	:	public IOPCBrowseServerAddressSpace,
		virtual public opc::OPCServerBase
{
private:
	boost::mutex bsaScopeGuard;

public:

	virtual ~BrowseServerAddressSpace();

	// IOPCBrowseServerAddressSpace implementation
	STDMETHODIMP QueryOrganization( 
		/* [out] */ OPCNAMESPACETYPE *pNameSpaceType );

	STDMETHODIMP ChangeBrowsePosition( 
		/* [in] */ OPCBROWSEDIRECTION dwBrowseDirection,
		/* [string][in] */ LPCWSTR szString );

	STDMETHODIMP BrowseOPCItemIDs( 
		/* [in] */ OPCBROWSETYPE dwBrowseFilterType,
		/* [string][in] */ LPCWSTR szFilterCriteria,
		/* [in] */ VARTYPE vtDataTypeFilter,
		/* [in] */ DWORD dwAccessRightsFilter,
		/* [out] */ LPENUMSTRING *ppIEnumString );

	STDMETHODIMP GetItemID( 
		/* [in] */ LPWSTR szItemDataID,
		/* [string][out] */ LPWSTR *szItemID );

	STDMETHODIMP BrowseAccessPaths( 
		/* [string][in] */ LPCWSTR szItemID,
		/* [out] */ LPENUMSTRING *ppIEnumString );
}; // class BrowseServerAddressSpace

} // namespace impl
}
}

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_impl_browse_server_address_space_h_
