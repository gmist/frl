#include "opc/impl/frl_opc_impl_async_io.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <OleCtl.h>
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "opc/frl_opc_async_request.h"
#include "opc/frl_opc_server.h"
#include "opc/frl_opc_group.h"

namespace frl { namespace opc { namespace impl {

AsyncIO::~AsyncIO()
{
}

HRESULT STDMETHODCALLTYPE
AsyncIO::Read(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [in] */ DWORD dwTransactionID,
	/* [out] */ DWORD *pdwCancelID,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if( phServer == NULL || pdwCancelID == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*pdwCancelID = 0;
	*ppErrors    = NULL;

	if( dwCount == 0 )
		return E_INVALIDARG;

	if( ! isConnected( IID_IOPCDataCallback ) )
		return CONNECT_E_NOCONNECTION;

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT result = S_OK;
	std::list<OPCHANDLE> handles;

	boost::mutex::scoped_lock guard( groupGuard );
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		GroupItemElemList::iterator it = itemList.find( phServer[i] );
		if( it == end )
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
		GroupElem tmp = GroupElem( dynamic_cast< Group* >( this ) );
		AsyncRequestListElem request( new AsyncRequest( tmp, async_request::READ, handles) );
		*pdwCancelID = request->getCancelID();
		request->setTransactionID( dwTransactionID );
		server->addAsyncRequest( request );
	}

	return result;
}

HRESULT STDMETHODCALLTYPE
AsyncIO::Write(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][in] */ VARIANT *pItemValues,
	/* [in] */ DWORD dwTransactionID,
	/* [out] */ DWORD *pdwCancelID,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if( phServer == NULL || pItemValues == NULL || pdwCancelID == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*pdwCancelID = 0;
	*ppErrors    = NULL;

	if( dwCount == 0 )
		return E_INVALIDARG;

	if( ! isConnected( IID_IOPCDataCallback ) )
		return CONNECT_E_NOCONNECTION;

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT result = S_OK;
	std::list< OPCHANDLE > handles;

	boost::mutex::scoped_lock guard( groupGuard );
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		GroupItemElemList::iterator it = itemList.find( phServer[i] );
		if( it == end )
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
		GroupElem tmp = GroupElem( dynamic_cast< Group* >( this ) );
		AsyncRequestListElem request( new AsyncRequest( tmp, async_request::WRITE ) );
		request->init( handles, pItemValues );
		*pdwCancelID = request->getCancelID();
		request->setTransactionID( dwTransactionID );
		server->addAsyncRequest( request );
	}
	return result;
}

HRESULT STDMETHODCALLTYPE
AsyncIO::Refresh2(
	/* [in] */ OPCDATASOURCE dwSource,
	/* [in] */ DWORD dwTransactionID,
	/* [out] */ DWORD *pdwCancelID )
{
	if( deleted )
		return E_FAIL;

	if( pdwCancelID == NULL )
		return E_INVALIDARG;

	if( dwSource != OPC_DS_CACHE && dwSource != OPC_DS_DEVICE )
		return E_INVALIDARG;

	*pdwCancelID = 0;

	boost::mutex::scoped_lock guard( groupGuard );

	if( ! isConnected( IID_IOPCDataCallback ) )
		return CONNECT_E_NOCONNECTION;

	if( ! actived )
		return E_FAIL;

	if( itemList.size() == 0 )
		return E_FAIL;

	std::list< OPCHANDLE > handles;
	GroupItemElemList::iterator end = itemList.end();
	for( GroupItemElemList::iterator it = itemList.begin(); it != end; ++it )
	{
		if( (*it).second->isActived() )
			handles.push_back( (*it).first );
	}

	if( handles.size() == 0 )
		return E_FAIL;

	GroupElem tmp = GroupElem( dynamic_cast< Group* >( this ) );
	AsyncRequestListElem request( new AsyncRequest( tmp, async_request::UPDATE, handles) );
	*pdwCancelID = request->getCancelID();
	request->setTransactionID( dwTransactionID );
	request->setSource( dwSource );
	doAsyncRefresh( request );
	return S_OK;
}

HRESULT STDMETHODCALLTYPE
AsyncIO::Cancel2( /* [in] */ DWORD dwCancelID )
{
	if( deleted )
		return E_FAIL;

	if( ! isConnected( IID_IOPCDataCallback ) )
		return CONNECT_E_NOCONNECTION;

	if( server->asyncRequestCancel( dwCancelID ) )
		return S_OK;
	return E_FAIL;
}

HRESULT STDMETHODCALLTYPE
AsyncIO::SetEnable( /* [in] */ BOOL bEnable )
{
	if( deleted )
		return E_FAIL;

	boost::mutex::scoped_lock guard( groupGuard );

	if( ! isConnected( IID_IOPCDataCallback ) )
		return CONNECT_E_NOCONNECTION;

	if( bEnable == TRUE || bEnable == VARIANT_TRUE )
		enabled = frl::True;
	else
		enabled = frl::False;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE
AsyncIO::GetEnable( /* [out] */ BOOL *pbEnable )
{
	if( deleted )
		return E_FAIL;

	if( pbEnable == NULL )
		return E_INVALIDARG;

	boost::mutex::scoped_lock guard( groupGuard );

	if( ! isConnected( IID_IOPCDataCallback ) )
		return CONNECT_E_NOCONNECTION;

	*pbEnable = enabled;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE
AsyncIO::ReadMaxAge(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][in] */ DWORD *pdwMaxAge,
	/* [in] */ DWORD dwTransactionID,
	/* [out] */ DWORD *pdwCancelID,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	// dirty hack!!!
	return Read( dwCount, phServer, dwTransactionID, pdwCancelID, ppErrors );
}

HRESULT STDMETHODCALLTYPE AsyncIO::WriteVQT(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][in] */ OPCITEMVQT *pItemVQT,
	/* [in] */ DWORD dwTransactionID,
	/* [out] */ DWORD *pdwCancelID,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if( phServer == NULL || pdwCancelID == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*pdwCancelID = 0;
	*ppErrors    = NULL;

	if( dwCount == 0 )
		return E_INVALIDARG;

	if( ! isConnected( IID_IOPCDataCallback ) )
		return CONNECT_E_NOCONNECTION;

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT result = S_OK;
	std::list< ItemHVQT > itemsHVQTList;

	boost::mutex::scoped_lock guard( groupGuard );
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		GroupItemElemList::iterator it = itemList.find( phServer[i] );
		if( it == end )
		{
			result = S_FALSE;
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			continue;
		}

		if( ! (*it).second->isWritable() )
		{
			result = S_FALSE;
			(*ppErrors)[i] = OPC_E_BADRIGHTS;
			continue;
		}

		if( pItemVQT[i].vDataValue.vt == VT_EMPTY )
		{
			result = S_FALSE;
			(*ppErrors)[i] = OPC_E_BADTYPE;
			continue;
		}

		ItemHVQT tmp;
		tmp.setHandle( (*it).first );
		tmp.setValue( pItemVQT[i].vDataValue );

		if( pItemVQT[i].bQualitySpecified )
		{
			tmp.setQuality( pItemVQT[i].wQuality );
		}

		if( pItemVQT[i].bTimeStampSpecified )
		{
			tmp.setTimeStamp( pItemVQT[i].ftTimeStamp );
		}

		itemsHVQTList.push_back( tmp );
		(*ppErrors)[i] = S_OK;
	}

	if( itemsHVQTList.size() > 0 )
	{
		GroupElem tmp = GroupElem( dynamic_cast< Group* >( this ) );
		AsyncRequestListElem request( new AsyncRequest( tmp, async_request::WRITE, itemsHVQTList ) );
		*pdwCancelID = request->getCancelID();
		request->setTransactionID( dwTransactionID );
		server->addAsyncRequest( request );
	}
	return result;
}

HRESULT STDMETHODCALLTYPE
AsyncIO::RefreshMaxAge(
	/* [in] */ DWORD dwMaxAge,
	/* [in] */ DWORD dwTransactionID,
	/* [out] */ DWORD *pdwCancelID )
{
	// dirty hack!!!
	if( dwMaxAge == 0xFFFFFFFF )
	{
		return Refresh2( OPC_DS_CACHE, dwTransactionID, pdwCancelID );
	}
	return Refresh2( OPC_DS_DEVICE, dwTransactionID, pdwCancelID );
}

} // namespace impl
} // namespace opc
} // FatRat Library

#endif
