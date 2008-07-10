#include "opc/frl_opc_connection_point.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <OleCtl.h>
#include "opc/frl_opc_connection_point_container.h"

namespace frl
{
namespace opc
{
ConnectionPoint::ConnectionPoint()
	:	refCount( 0 ), iid_interface( GUID_NULL), container( NULL ), callBack( NULL ), cookie( 0 )
{
}

ConnectionPoint::ConnectionPoint( const IID &iid, ConnectionPointContainer *container_ )
	:	refCount( 0 ), iid_interface( iid ), container( container_ ), callBack( NULL ),  cookie( 0 )
{
}

ConnectionPoint::~ConnectionPoint()
{
	if( callBack != NULL )
		callBack->Release();
}

STDMETHODIMP ConnectionPoint::QueryInterface( REFIID iid, LPVOID* ppInterface )
{
	if( ppInterface == NULL )
		return E_INVALIDARG;

	*ppInterface = NULL;

	if ( iid == IID_IConnectionPoint || iid == IID_IUnknown )
	{
		*ppInterface = dynamic_cast< IConnectionPoint* >( this );
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

ULONG ConnectionPoint::AddRef( void )
{
	return ::InterlockedIncrement( &refCount );
}

ULONG ConnectionPoint::Release( void )
{
	LONG tmp = ::InterlockedDecrement( &refCount );
	if( tmp == 0 )
		delete this;
	return tmp;
}

const IID& ConnectionPoint::getInterface()
{
	return iid_interface;
}

HRESULT STDMETHODCALLTYPE ConnectionPoint::GetConnectionInterface( /* [out] */ IID *pIID )
{
	if (pIID == NULL)
		return E_INVALIDARG;

	*pIID = iid_interface;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ConnectionPoint::GetConnectionPointContainer( /* [out] */ IConnectionPointContainer **ppCPC )
{
	if (ppCPC == NULL)
		return E_INVALIDARG;

	if( container != NULL )
		return container->QueryInterface( IID_IConnectionPointContainer, (void**)ppCPC );

	return E_FAIL;
}

HRESULT STDMETHODCALLTYPE ConnectionPoint::Advise( /* [in] */ IUnknown *pUnkSink, /* [out] */ DWORD *pdwCookie )
{
	if( pUnkSink == NULL || pdwCookie == NULL )
		return E_POINTER;

	*pdwCookie = 0;

	if( callBack != NULL)
		return CONNECT_E_ADVISELIMIT;

	HRESULT hResult = pUnkSink->QueryInterface( iid_interface, (void**)&callBack );

	if( hResult == E_NOINTERFACE )
		return hResult;

	if( FAILED( hResult ) )
		return hResult;

	cookie = *(reinterpret_cast< DWORD* >( callBack ));
	*pdwCookie = cookie;

	if( container != NULL )
	{
		container->AddRef();
		container->OnAdvise( iid_interface, cookie );
		container->Release();
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE ConnectionPoint::Unadvise( /* [in] */ DWORD dwCookie )
{
	if( cookie != dwCookie || callBack == NULL )
		return CONNECT_E_NOCONNECTION;

	callBack->Release();
	callBack = NULL;
	cookie   = 0;

	if( container != NULL )
	{
		container->AddRef();
		container->OnUnadvise( iid_interface, dwCookie);
		container->Release();
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE ConnectionPoint::EnumConnections( /* [out] */ IEnumConnections **ppEnum )
{
	if( ppEnum == NULL )
		return E_POINTER;
	*ppEnum = NULL;
	//return QueryInterface( IID_IEnumConnections, (void**)ppEnum );
	return E_NOTIMPL;
}

frl::Bool ConnectionPoint::isConnected()
{
	return cookie != 0;
}

IUnknown* ConnectionPoint::getCallBack()
{
	return callBack;
}


} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
