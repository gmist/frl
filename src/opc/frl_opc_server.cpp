#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include <algorithm>
#include "sys/frl_sys_util.h"
#include "opc/frl_opc_server.h"
#include "opc/frl_opc_enum_group.h"
#include "frl_exception.h"
#include "opc/frl_opc_server_factory.h"
#include "frl_util_functors.h"
#include "os/win32/com/frl_os_win32_com_uuidof.h"
#include <boost/lambda/lambda.hpp>
#include "opc/frl_opc_group.h"

namespace frl
{
namespace opc
{

OPCServer::OPCServer()
{
	// call opcAddressSpace::getInstance().finalConstruct first !
	if( ! opcAddressSpace::getInstance().isInit() )
		FRL_THROW_S_CLASS( address_space::AddressSpace::NotFinalConstruct );

	os::win32::com::zeroMemory<OPCSERVERSTATUS>( &serverStatus );
	CoFileTimeNow( &serverStatus.ftStartTime );
	serverStatus.szVendorInfo = L"Serg Baburin";
	serverStatus.dwServerState = OPC_STATUS_NOCONFIG;
	serverStatus.dwBandWidth = 0xFFFFFFFF;
	serverStatus.wMajorVersion = 2;
	registerInterface( IID_IOPCShutdown );
	factory.LockServer( TRUE );
}

OPCServer::~OPCServer()
{
	factory.LockServer( FALSE );
}

void OPCServer::addAsyncRequest( AsyncRequestListElem &request )
{
	request_manager.addRequest( request );
}

STDMETHODIMP OPCServer::QueryInterface( REFIID iid, LPVOID* ppInterface )
{
	if( ppInterface == NULL )
		return E_POINTER;

	*ppInterface = NULL;

	if( iid == __uuidof( IOPCCommon ) || iid == IID_IUnknown )
	{
		*ppInterface = (dynamic_cast< IOPCCommon* >( this ) );
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCServer ) )
	{
		*ppInterface = (dynamic_cast< IOPCServer* >( this ) );
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCItemProperties ) )
	{
		*ppInterface = (dynamic_cast< IOPCItemProperties* >( this ) );
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCBrowseServerAddressSpace ) )
	{
		*ppInterface = (dynamic_cast< IOPCBrowseServerAddressSpace* >( this ) );
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IConnectionPointContainer ) )
	{
		*ppInterface = (dynamic_cast< IConnectionPointContainer* >( this ) );
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCBrowse ) )
	{
		*ppInterface = (dynamic_cast< IOPCBrowse* >( this ) );
		AddRef();
		return S_OK;
	}

	if( iid == __uuidof( IOPCItemIO ) )
	{
		*ppInterface = (dynamic_cast< IOPCItemIO* >( this ) );
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG OPCServer::AddRef( void )
{
	return ::InterlockedIncrement( &refCount );
}

ULONG OPCServer::Release( void )
{
	LONG tmp = ::InterlockedDecrement( &refCount );
	if( tmp == 0 )
		delete this;
	return tmp;
}

void OPCServer::setServerState( OPCSERVERSTATE newState )
{
	if( newState < OPC_STATUS_RUNNING || newState > OPC_STATUS_TEST )
		FRL_THROW_S_CLASS( OPCServer::InvalidServerState );
	serverStatus.dwServerState = newState;
	CoFileTimeNow( &serverStatus.ftCurrentTime );
}

OPCSERVERSTATE OPCServer::getServerState()
{
	return serverStatus.dwServerState;
}

frl::Bool OPCServer::asyncRequestCancel( DWORD id )
{	
	return ( request_manager.cancelRequest( id ) );
}

void OPCServer::removeItemFromRequestList( OPCHANDLE item_handle )
{
	request_manager.removeItemFromRequest( item_handle );
}

/*OPCServerBase* OPCServer::getOPCServerBase()
{
	return dynamic_cast< OPCServerBase* >( this );
}

OPCServerImpl* OPCServer::getOPCServerImpl()
{
	return dynamic_cast< OPCServerImpl* >( this );
}*/

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
