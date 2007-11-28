#ifndef frl_opc_group_state_mgt_h_
#define frl_opc_group_state_mgt_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_server.h"

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
				T* pT = static_cast<T*> (this);
				if( pT->deleted )
					return E_FAIL;

				*ppName = util::duplicateString( pT->name );
				*pActive = pT->actived;
				*pUpdateRate = pT->updateRate;
				*phServerGroup = pT->serverHandle;
				*phClientGroup = pT->clientHandle;
				*pTimeBias = pT->timeBias;
				*pPercentDeadband = pT->deadband;
				*pLCID = pT->localeID;
				return S_OK;
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
				T* pT = static_cast<T*> (this);
				if( pT->deleted )
					return E_FAIL;

				HRESULT hResult = S_OK;
				
				if(pRevisedUpdateRate != NULL)
					*pRevisedUpdateRate = 0;

				// validate deadband
				if( pPercentDeadband != NULL )
				{
					if( *pPercentDeadband < 0 || *pPercentDeadband > 100 )
						return E_INVALIDARG;
					pT->deadband = *pPercentDeadband;
				}

				// set update rate
				if( pRequestedUpdateRate != NULL )
				{
					static const int maxUpdateRate = 100;
					DWORD dwUpdateRate = *pRequestedUpdateRate;
					if (dwUpdateRate == 0 || dwUpdateRate%maxUpdateRate != 0)
					{
						dwUpdateRate = maxUpdateRate*(dwUpdateRate/maxUpdateRate+1);
						// indicate that the requested rate will not be used.
						hResult = OPC_S_UNSUPPORTEDRATE;
					}

					// reset tick baseline.
					pT->tickOffset = -2;
					*pRevisedUpdateRate = pT->updateRate = dwUpdateRate;
				}

				// set other parameters.
				if( pTimeBias != NULL )
					pT->timeBias   = *pTimeBias;
				if (pLCID != NULL)
					pT->localeID = *pLCID;
				if( phClientGroup != NULL )
					pT->clientHandle = *phClientGroup; 

				if( pActive != NULL )
				{
					// TODO: insert registration for updates

					pT->actived = (*pActive == TRUE);
				}

				return hResult;
			}

			HRESULT STDMETHODCALLTYPE SetName( 
				/* [string][in] */ LPCWSTR szName)
			{
				T* pT = static_cast<T*> (this);
				if( pT->deleted )
					return E_FAIL;

				Bool res = pT->server->setGroupName( pT->name, szName);
				if( ! res )
					return E_INVALIDARG;
				pT->name = szName;
				return S_OK;
			}

			HRESULT STDMETHODCALLTYPE CloneGroup( 
				/* [string][in] */ LPCWSTR szName,
				/* [in] */ REFIID riid,
				/* [iid_is][out] */ LPUNKNOWN *ppUnk)
			{
				T* pT = static_cast<T*> (this);
				if( pT->deleted )
					return E_FAIL;
				
				if (ppUnk == NULL)
					return E_INVALIDARG;

				Group* group = NULL;
				HRESULT result = pT->server->cloneGroup( pT->name, szName, &group );
				if( FAILED( result ) )
					return result;
				result = group->QueryInterface( riid, (void**)ppUnk );
				
				if( FAILED( result ) )
				{
					pT->server->RemoveGroup( group->getServerHandle(), FALSE );
				}
				return result;
			}
		}; // class GroupStateMgt
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_group_state_mgt_h_ */
