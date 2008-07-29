#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_server_factory.h"
#include "opc/frl_opc_server.h"

namespace frl{ namespace opc{

LONG OPCServerFactory::serverLocks = 0;

OPCServerFactory::OPCServerFactory()
	:	refCount( 1 ),
		outProc( False )
{
}

OPCServerFactory::~OPCServerFactory()
{
}

STDMETHODIMP OPCServerFactory::QueryInterface( REFIID iid, LPVOID* ppInterface )
{
	if( ppInterface == NULL )
		return E_INVALIDARG;

	*ppInterface = NULL;

	if( iid == IID_IClassFactory || iid == IID_IUnknown )
	{
		*ppInterface = dynamic_cast< IClassFactory* >( this );
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) OPCServerFactory::AddRef( void )
{
	return ::InterlockedIncrement( &refCount );
}

STDMETHODIMP_(ULONG) OPCServerFactory::Release( void )
{
	LONG tmp = ::InterlockedDecrement( &refCount );
	if( tmp == 0 )
		delete this;
	return tmp;
}

STDMETHODIMP OPCServerFactory::CreateInstance(
	/* [unique][in] */ IUnknown *pUnkOuter,
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ void **ppvObject )
{
	if( ( pUnkOuter != NULL ) && ( riid != IID_IUnknown ) )
		return CLASS_E_NOAGGREGATION ;
	frl::opc::OPCServer *server = new frl::opc::OPCServer();
	
	if( server == NULL )
		return E_OUTOFMEMORY;

	HRESULT result = server->QueryInterface( riid, ppvObject );
	if( FAILED( result ) )
	{
		delete server;
		return result;
	}
	server->setServerState( OPC_STATUS_RUNNING );
	return result;
}

STDMETHODIMP OPCServerFactory::LockServer( /* [in] */ BOOL fLock )
{
	if( fLock )
	{
		::InterlockedIncrement( &OPCServerFactory::serverLocks );
		if( outProc )
			CoAddRefServerProcess();
	}
	else
	{
		::InterlockedDecrement( &OPCServerFactory::serverLocks );
		if( outProc )
			CoReleaseServerProcess();
	}
	return S_OK;
}

void OPCServerFactory::isOutProc( Bool isOutProc )
{
	outProc = isOutProc;
}

frl::Bool OPCServerFactory::isServerInUse()
{
	return OPCServerFactory::serverLocks > 0;
}

} // namespace opc
} // namespace frl

#endif // FRL_PLATFORM_WIN32
