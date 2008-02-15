#ifndef frl_opc_async_io2_h_
#define frl_opc_async_io2_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_async_request.h"

namespace frl
{
	namespace opc
	{
		template < class T >
		class AsyncIO2 : public IOPCAsyncIO2
		{
		public:
			HRESULT STDMETHODCALLTYPE Read( 
				/* [in] */ DWORD dwCount,
				/* [size_is][in] */ OPCHANDLE *phServer,
				/* [in] */ DWORD dwTransactionID,
				/* [out] */ DWORD *pdwCancelID,
				/* [size_is][size_is][out] */ HRESULT **ppErrors)
			{
				T* pT = static_cast<T*> (this);
				if( pT->deleted )
					return E_FAIL;

				if( phServer == NULL || pdwCancelID == NULL || ppErrors == NULL )
					return E_INVALIDARG;

				*pdwCancelID = NULL;
				*ppErrors    = NULL;

				if( dwCount == 0 )
					return E_INVALIDARG;

				if( ! pT->isConnected( IID_IOPCDataCallback ) )
					return CONNECT_E_NOCONNECTION;

				*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
				os::win32::com::zeroMemory< HRESULT >( *ppErrors );

				HRESULT result = S_OK;
				std::list<OPCHANDLE> handles;

				lock::ScopeGuard guard( pT->groupGuard );
				for( DWORD i = 0; i < dwCount; i++ )
				{
					std::map< OPCHANDLE, GroupItem* >::iterator it = pT->itemList.find( phServer[i] );
					if( it == pT->itemList.end() )
					{
						result = S_FALSE;
						(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
						continue;
					}
					handles.push_back( (*it).first );
					(*ppErrors)[i] = S_OK;
				}

				if( handles.size() > 0 )
				{
					AsyncRequest *request = new AsyncRequest( handles );
					*pdwCancelID = request->getCancelID();
					request->setTransactionID( dwTransactionID );
					pT->asyncReadList.push_back( request );
					pT->readEvent.signal();
				}

				return result;
			}

			HRESULT STDMETHODCALLTYPE Write( 
				/* [in] */ DWORD dwCount,
				/* [size_is][in] */ OPCHANDLE *phServer,
				/* [size_is][in] */ VARIANT *pItemValues,
				/* [in] */ DWORD dwTransactionID,
				/* [out] */ DWORD *pdwCancelID,
				/* [size_is][size_is][out] */ HRESULT **ppErrors)
			{
				T* pT = static_cast<T*> (this);
				if( pT->deleted )
					return E_FAIL;

				if( phServer == NULL || pItemValues == NULL || pdwCancelID == NULL || ppErrors == NULL )
					return E_INVALIDARG;

				*pdwCancelID = NULL;
				*ppErrors    = NULL;

				if( dwCount == 0 )
					return E_INVALIDARG;

				if( ! pT->isConnected( IID_IOPCDataCallback ) )
					return CONNECT_E_NOCONNECTION;

				*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
				os::win32::com::zeroMemory< HRESULT >( *ppErrors );

				HRESULT result = S_OK;
				std::list< OPCHANDLE > handles;

				lock::ScopeGuard guard( pT->groupGuard );
				for( DWORD i = 0; i < dwCount; i++ )
				{
					std::map< OPCHANDLE, GroupItem* >::iterator it = pT->itemList.find( phServer[i] );
					if( it == pT->itemList.end() )
					{
						result = S_FALSE;
						(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
						continue;
					}
					handles.push_back( (*it).first );
					(*ppErrors)[i] = S_OK;
				}

				if( handles.size() > 0 )
				{
					AsyncRequest *request = new AsyncRequest();
					request->init( handles, pItemValues );
					*pdwCancelID = request->getCancelID();
					request->setTransactionID( dwTransactionID );
					pT->asyncWriteList.push_back( request );
					pT->writeEvent.signal();
				}
				return result;
			}

			HRESULT STDMETHODCALLTYPE Refresh2( 
				/* [in] */ OPCDATASOURCE dwSource,
				/* [in] */ DWORD dwTransactionID,
				/* [out] */ DWORD *pdwCancelID)
			{
				T* pT = static_cast<T*> (this);
				if( pT->deleted )
					return E_FAIL;

				if( pdwCancelID == NULL )
					return E_INVALIDARG;

				if (dwSource != OPC_DS_CACHE && dwSource != OPC_DS_DEVICE)
					return E_INVALIDARG;

				*pdwCancelID = NULL;

				lock::ScopeGuard guard( pT->groupGuard );

				if( ! pT->isConnected( IID_IOPCDataCallback ) )
					return CONNECT_E_NOCONNECTION;

				if( ! pT->actived )
					return E_FAIL;

				if( pT->itemList.size() == 0 )
					return E_FAIL;

				if( pT->asyncRefreshList.size() )
				{
					for( std::list< AsyncRequest* >::iterator it = pT->asyncRefreshList.begin(); it != pT->asyncRefreshList.end(); ++it )
						delete (*it);
					pT->asyncRefreshList.clear();
				}
				
				std::list< OPCHANDLE > handles;
				for( std::map< OPCHANDLE, GroupItem* >::iterator it = pT->itemList.begin(); it != pT->itemList.end(); ++it )
				{
					if( (*it).second->isActived() )
						handles.push_back( (*it).first );
				}

				if( handles.size() == 0 )
					return E_FAIL;

				AsyncRequest *request = new AsyncRequest( handles );
				*pdwCancelID = request->getCancelID();
				request->setTransactionID( dwTransactionID );
				request->setSource( dwSource );
				pT->asyncRefreshList.push_back( request );
				pT->refreshEvent.signal();
				return S_OK;
			}

			HRESULT STDMETHODCALLTYPE Cancel2( 
				/* [in] */ DWORD dwCancelID)
			{
				T* pT = static_cast<T*> (this);
				if( pT->deleted )
					return E_FAIL;

				if( ! pT->isConnected( IID_IOPCDataCallback ) )
					return CONNECT_E_NOCONNECTION;

				Bool isExistRead = False;
				std::list< AsyncRequest* >::iterator it;
				lock::ScopeGuard guard( pT->groupGuard );
				for( it = pT->asyncReadList.begin(); it != pT->asyncReadList.end(); ++it )
				{
					if( (*it)->getCancelID() == dwCancelID )
					{
						isExistRead = True;
						break;
					}
				}
				if( isExistRead )
					(*it)->isCancelled( True );
				
				Bool isExistRefresh = False;
				for( it = pT->asyncRefreshList.begin(); it != pT->asyncRefreshList.end(); ++it )
				{
					if( (*it)->getCancelID() == dwCancelID )
					{
						isExistRefresh = True;
						break;
					}
				}
				if( isExistRefresh )
					(*it)->isCancelled( True );
				
				Bool isExistWrite = False;
				for( it = pT->asyncWriteList.begin(); it != pT->asyncWriteList.end(); ++it )
				{
					if( (*it)->getCancelID() == dwCancelID )
					{
						isExistWrite = True;
						break;
					}
				}
				if( isExistWrite )
					(*it)->isCancelled( True );
				if( isExistRead || isExistRefresh || isExistWrite )
					return S_OK;
				return E_FAIL;
			};

			HRESULT STDMETHODCALLTYPE SetEnable( 
				/* [in] */ BOOL bEnable)
			{
				T* pT = static_cast<T*> (this);
				if( pT->deleted )
					return E_FAIL;

				lock::ScopeGuard guard( pT->groupGuard );

				if( ! pT->isConnected( IID_IOPCDataCallback ) )
					return CONNECT_E_NOCONNECTION;

				if( bEnable == TRUE || bEnable == VARIANT_TRUE )
					pT->enabled = frl::True;
				else
					pT->enabled = frl::False;
				return S_OK;
			}

			HRESULT STDMETHODCALLTYPE GetEnable( 
				/* [out] */ BOOL *pbEnable)
			{
				T* pT = static_cast<T*> (this);
				if( pT->deleted )
					return E_FAIL;

				if( pbEnable == NULL )
					return E_INVALIDARG;

				lock::ScopeGuard guard( pT->groupGuard );

				if( ! pT->isConnected( IID_IOPCDataCallback ) )
					return CONNECT_E_NOCONNECTION;
				
				*pbEnable = pT->enabled;

				return S_OK;
			}
		};
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_async_io2_h_ */
