#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_server_factory.h"
#include "opc/frl_opc_server.h"

namespace frl
{
namespace opc
{

LONG OPCServerFactory::serverLocks = 0;

OPCServerFactory::OPCServerFactory()
	:	refCount( 1 ), outProc( False ), noMoreServers( True )
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

ULONG OPCServerFactory::AddRef( void )
{
	return ::InterlockedIncrement( &refCount );
}

ULONG OPCServerFactory::Release( void )
{
	LONG tmp = ::InterlockedDecrement( &refCount );
	if( tmp == 0 )
		delete this;
	return tmp;
}

HRESULT STDMETHODCALLTYPE OPCServerFactory::CreateInstance(
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

HRESULT STDMETHODCALLTYPE OPCServerFactory::LockServer( /* [in] */ BOOL fLock )
{
	if( fLock )
	{
		::InterlockedIncrement( &OPCServerFactory::serverLocks );
		if( outProc )
			CoAddRefServerProcess();
	}
	else
	{
		LONG tmp = ::InterlockedDecrement( &OPCServerFactory::serverLocks );
		if( outProc )
		{
			if( CoReleaseServerProcess() == 0 )
				noMoreServers = True;
		}
		if( tmp == 0 )
			noMoreServers = True;
	}
	return S_OK;
}

void OPCServerFactory::isOutProc( Bool isOutProc )
{
	outProc = isOutProc;
}

void OPCServerFactory::usageServer()
{
	noMoreServers = False;
}

frl::Bool OPCServerFactory::isServerInUse()
{
	return !noMoreServers;
}
} // namespace opc
} // namespace frl

#endif // FRL_PLATFORM_WIN32
