#ifndef frl_opc_group_state_mgt_h_
#define frl_opc_group_state_mgt_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"

namespace frl
{
	namespace opc
	{
		template < class T >
		class GroupStateMgt : public IOPCGroupStateMgt
		{
		public:
			HRESULT STDMETHODCALLTYPE GetState( 
				/* [out] */ DWORD *pUpdateRate,
				/* [out] */ BOOL *pActive,
				/* [string][out] */ LPWSTR *ppName,
				/* [out] */ LONG *pTimeBias,
				/* [out] */ FLOAT *pPercentDeadband,
				/* [out] */ DWORD *pLCID,
				/* [out] */ OPCHANDLE *phClientGroup,
				/* [out] */ OPCHANDLE *phServerGroup)
			{
				return E_NOTIMPL;
			}

			HRESULT STDMETHODCALLTYPE SetState( 
				/* [in][unique] */ DWORD *pRequestedUpdateRate,
				/* [out] */ DWORD *pRevisedUpdateRate,
				/* [in][unique] */ BOOL *pActive,
				/* [in][unique] */ LONG *pTimeBias,
				/* [in][unique] */ FLOAT *pPercentDeadband,
				/* [in][unique] */ DWORD *pLCID,
				/* [in][unique] */ OPCHANDLE *phClientGroup)
			{
				return E_NOTIMPL;
			}

			HRESULT STDMETHODCALLTYPE SetName( 
				/* [string][in] */ LPCWSTR szName)
			{
				return E_NOTIMPL;
			}

			HRESULT STDMETHODCALLTYPE CloneGroup( 
				/* [string][in] */ LPCWSTR szName,
				/* [in] */ REFIID riid,
				/* [iid_is][out] */ LPUNKNOWN *ppUnk)
			{
				return E_NOTIMPL;
			}
		}; // class GroupStateMgt
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_group_state_mgt_h_ */
