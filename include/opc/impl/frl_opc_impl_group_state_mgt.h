#ifndef frl_opc_impl_group_state_mgt_h_
#define frl_opc_impl_group_state_mgt_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_group_base.h"

namespace frl { namespace opc { namespace impl {

/*!
	\brief IOPCGroupStateMgt and IOPCGroupStateMgt2 implementation.
	\details
	\arg
		IOPCGroupStateMgt allows the client to manage the overall state of the group. 
		Primarily this allows changes to the update rate and active state of the group.
	\arg
		This IOPCGroupStateMgt2 interface was added to enhance the existing IOPCGroupStateMgt interface. 
		IOPCGroupStateMgt2 inherits from IOPCGroupStateMgt and therefore all IOPCGroupStateMgt 
		methods defined in IOPCGroupStateMgt are also part of this interface and will 
		not be documented here.  Please refer to the IOPCGroupStateMgt interface methods 
		for further details. It is expected that Data Access 3.0 only servers, 
		will implement this interface as opposed to IOPCGroupStateMgt. 
		The purpose of this interface is to set/get the keep-alive time for a subscription. 
		When a subscription has a non-zero keep-alive time, the server will insure 
		that the client receives a callback on the subscription minimally at the rate 
		indicated by the keep-alive time, even when there are no new events to report. 
		By providing callbacks at a minimum known frequency, 
		the client can be assured of the health of the server 
		and subscription without resorting to pinging the server with calls to GetStatus().
*/

class GroupStateMgt
	:	public IOPCGroupStateMgt2,
		virtual public opc::GroupBase
{
public:
	virtual ~GroupStateMgt();
	STDMETHODIMP GetState( 
		/* [out] */ DWORD *pUpdateRate,
		/* [out] */ BOOL *pActive,
		/* [string][out] */ LPWSTR *ppName,
		/* [out] */ LONG *pTimeBias,
		/* [out] */ FLOAT *pPercentDeadband,
		/* [out] */ DWORD *pLCID,
		/* [out] */ OPCHANDLE *phClientGroup,
		/* [out] */ OPCHANDLE *phServerGroup);

	STDMETHODIMP SetState( 
		/* [in][unique] */ DWORD *pRequestedUpdateRate,
		/* [out] */ DWORD *pRevisedUpdateRate,
		/* [in][unique] */ BOOL *pActive,
		/* [in][unique] */ LONG *pTimeBias,
		/* [in][unique] */ FLOAT *pPercentDeadband,
		/* [in][unique] */ DWORD *pLCID,
		/* [in][unique] */ OPCHANDLE *phClientGroup);

	STDMETHODIMP SetName( 
		/* [string][in] */ LPCWSTR szName);

	STDMETHODIMP CloneGroup( 
		/* [string][in] */ LPCWSTR szName,
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ LPUNKNOWN *ppUnk);

	virtual STDMETHODIMP SetKeepAlive( 
		/* [in] */ DWORD dwKeepAliveTime,
		/* [out] */ DWORD *pdwRevisedKeepAliveTime);

	virtual STDMETHODIMP GetKeepAlive( 
		/* [out] */ DWORD *pdwKeepAliveTime);
}; // class GroupStateMgt

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_impl_group_state_mgt_h_
