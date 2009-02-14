#ifndef frl_opc_impl_common_h_
#define frl_opc_impl_common_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "frl_types.h"
#include "../dependency/vendors/opc_foundation/opccomn.h"

namespace frl { namespace opc { namespace impl {

/*!
	\class OPCCommon
	\brief IOPCCommon implementation.
	\details IOPCCommon interface is used by all OPC Server types (DataAccess, Alarm&Event, Historical Data).
	It provides the ability to set  and query a LocaleID which would be in effect for the particular client/server session.
	That is, the actions of one client do not affect any other clients.
	As with other interfaces such as IUnknown, the instance of this interface for each server is unique.
	That is, an OPC Data Access server object and and OPC Alarms and Events server object might both provide an implementation of IOPCCommon.
	A client which is maintaining connections to both servers would, as with any other interface, use the interfaces on these two objects independently.
*/
class OPCCommon
	:	public IOPCCommon
{
private:
	String clientName;
	DWORD lcid;

public:

	OPCCommon();
	virtual ~OPCCommon();

	STDMETHODIMP SetLocaleID( 
		/* [in] */ LCID dwLcid);

	STDMETHODIMP GetLocaleID( 
		/* [out] */ LCID *pdwLcid);

	STDMETHODIMP QueryAvailableLocaleIDs( 
		/* [out] */ DWORD *pdwCount,
		/* [size_is][size_is][out] */ LCID **pdwLcid);

	STDMETHODIMP GetErrorString( 
		/* [in] */ HRESULT dwError,
		/* [string][out] */ LPWSTR *ppString);

	STDMETHODIMP SetClientName( 
		/* [string][in] */ LPCWSTR szName);
}; // OPCCommon

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_impl_common_h_
