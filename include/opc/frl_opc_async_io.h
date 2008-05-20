#ifndef frl_opc_async_io_h_
#define frl_opc_async_io_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <OleCtl.h>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_async_request.h"

namespace frl
{
namespace opc
{

template < class T >
class AsyncIO : public IOPCAsyncIO3
{
public:
	virtual HRESULT STDMETHODCALLTYPE Read( 
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

		*pdwCancelID = 0;
		*ppErrors    = NULL;

		if( dwCount == 0 )
			return E_INVALIDARG;

		if( ! pT->isConnected( IID_IOPCDataCallback ) )
			return CONNECT_E_NOCONNECTION;

		*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
		os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

		HRESULT result = S_OK;
		std::list<OPCHANDLE> handles;

		lock::ScopeGuard guard( pT->groupGuard );
		for( DWORD i = 0; i < dwCount; ++i )
		{
			GroupItemElemList::iterator it = pT->itemList.find( phServer[i] );
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
			AsyncRequestListElem request( new AsyncRequest( pT->getServerHandle(), handles) );
			*pdwCancelID = request->getCancelID();
			request->setTransactionID( dwTransactionID );
			pT->server->addAsyncReadRequest( request );
			pT->server->asyncReadSignal();
		}

		return result;
	}

	virtual HRESULT STDMETHODCALLTYPE Write( 
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

		*pdwCancelID = 0;
		*ppErrors    = NULL;

		if( dwCount == 0 )
			return E_INVALIDARG;

		if( ! pT->isConnected( IID_IOPCDataCallback ) )
			return CONNECT_E_NOCONNECTION;

		*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
		os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

		HRESULT result = S_OK;
		std::list< OPCHANDLE > handles;

		lock::ScopeGuard guard( pT->groupGuard );
		for( DWORD i = 0; i < dwCount; ++i )
		{
			GroupItemElemList::iterator it = pT->itemList.find( phServer[i] );
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
			AsyncRequestListElem request( new AsyncRequest( pT->getServerHandle() ) );
			request->init( handles, pItemValues );
			*pdwCancelID = request->getCancelID();
			request->setTransactionID( dwTransactionID );
			pT->server->addAsyncWriteRequest( request );
			pT->server->asyncWriteSignal();
		}
		return result;
	}

	virtual HRESULT STDMETHODCALLTYPE Refresh2( 
		/* [in] */ OPCDATASOURCE dwSource,
		/* [in] */ DWORD dwTransactionID,
		/* [out] */ DWORD *pdwCancelID)
	{
		T* pT = static_cast<T*> (this);
		if( pT->deleted )
			return E_FAIL;

		if( pdwCancelID == NULL )
			return E_INVALIDARG;

		if( dwSource != OPC_DS_CACHE && dwSource != OPC_DS_DEVICE )
			return E_INVALIDARG;

		*pdwCancelID = 0;

		lock::ScopeGuard guard( pT->groupGuard );

		if( ! pT->isConnected( IID_IOPCDataCallback ) )
			return CONNECT_E_NOCONNECTION;

		if( ! pT->actived )
			return E_FAIL;

		if( pT->itemList.size() == 0 )
			return E_FAIL;
		
		std::list< OPCHANDLE > handles;
		GroupItemElemList::iterator end = pT->itemList.end();
		for( GroupItemElemList::iterator it = pT->itemList.begin(); it != end; ++it )
		{
			if( (*it).second->isActived() )
				handles.push_back( (*it).first );
		}

		if( handles.size() == 0 )
			return E_FAIL;

		AsyncRequestListElem request( new AsyncRequest( pT->getServerHandle(), handles) );
		*pdwCancelID = request->getCancelID();
		request->setTransactionID( dwTransactionID );
		request->setSource( dwSource );
		pT->doAsyncRefresh( request );
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Cancel2( 
		/* [in] */ DWORD dwCancelID)
	{
		T* pT = static_cast<T*> (this);
		if( pT->deleted )
			return E_FAIL;

		if( ! pT->isConnected( IID_IOPCDataCallback ) )
			return CONNECT_E_NOCONNECTION;

		if( pT->server->asyncRequestCancel( dwCancelID ) )
			return S_OK;
		return E_FAIL;
	};

	virtual HRESULT STDMETHODCALLTYPE SetEnable( 
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

	virtual HRESULT STDMETHODCALLTYPE GetEnable( 
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

	virtual HRESULT STDMETHODCALLTYPE ReadMaxAge( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ DWORD *pdwMaxAge,
		/* [in] */ DWORD dwTransactionID,
		/* [out] */ DWORD *pdwCancelID,
		/* [size_is][size_is][out] */ HRESULT **ppErrors)
	{
		// dirty hack!!!
		return Read( dwCount, phServer, dwTransactionID, pdwCancelID, ppErrors );
	}

	virtual HRESULT STDMETHODCALLTYPE WriteVQT( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ OPCITEMVQT *pItemVQT,
		/* [in] */ DWORD dwTransactionID,
		/* [out] */ DWORD *pdwCancelID,
		/* [size_is][size_is][out] */ HRESULT **ppErrors)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE RefreshMaxAge( 
		/* [in] */ DWORD dwMaxAge,
		/* [in] */ DWORD dwTransactionID,
		/* [out] */ DWORD *pdwCancelID)
	{
		// dirty hack!!!
		if( dwMaxAge == 0xFFFFFFFF )
		{
			return Refresh2( OPC_DS_CACHE, dwTransactionID, pdwCancelID );
		}
		return Refresh2( OPC_DS_DEVICE, dwTransactionID, pdwCancelID );
	}
};

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif /* frl_opc_async_io_h_ */
