#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <algorithm>
#include <OleCtl.h>
#include "opc/frl_opc_connection_point_container.h"
#include "opc/frl_opc_enum_connection_points.h"
#include "opc/frl_opc_connection_point.h"
#include "frl_util_functors.h"

namespace frl
{
namespace opc
{
ConnectionPointContainer::ConnectionPointContainer()
{
}

ConnectionPointContainer::~ConnectionPointContainer()
{
}

HRESULT STDMETHODCALLTYPE ConnectionPointContainer::EnumConnectionPoints( /* [out] */ IEnumConnectionPoints **ppEnum )
{
	if (ppEnum == NULL)
		return E_POINTER;
	
	opc::EnumConnectionPoints* pEnumCPs = new opc::EnumConnectionPoints( points );
	if( pEnumCPs == NULL )
		return E_OUTOFMEMORY;

	HRESULT hResult = pEnumCPs->QueryInterface( IID_IEnumConnectionPoints, (void**)ppEnum );
	if( FAILED( hResult ) )
		delete pEnumCPs;
	return hResult;
}

HRESULT STDMETHODCALLTYPE ConnectionPointContainer::FindConnectionPoint( /* [in] */ REFIID riid, /* [out] */ IConnectionPoint **ppCP )
{
	if( ppCP == NULL )
		return E_POINTER;

	lock::ScopeGuard guard( cpGuard );
	for( ConnectionPointList::iterator it = points.begin(); it != points.end(); ++it )
	{
		if( (*it)->getInterface() == riid )
			return (*it)->QueryInterface( IID_IConnectionPoint, (void**)ppCP );
	}

	return CONNECT_E_NOCONNECTION;
}

frl::Bool ConnectionPointContainer::isConnected( const IID &interface_ )
{
	lock::ScopeGuard guard( cpGuard );
	for( ConnectionPointList::iterator it = points.begin(); it != points.end(); ++it )
	{
		if( (*it)->getInterface() == interface_ )
			return (*it)->isConnected();
	}
	return False;
}

void ConnectionPointContainer::registerInterface( const IID& interface_ )
{
	lock::ScopeGuard guard( cpGuard );
	for( ConnectionPointList::iterator it = points.begin(); it != points.end(); ++it )
	{
		if( (*it)->getInterface() == interface_ )
			return; // already registration
	}
	points.push_back( ConnectionPointElem( new ConnectionPoint( interface_, this ) ) );
}

void ConnectionPointContainer::unregisterInterface( const IID& interface_ )
{
	lock::ScopeGuard guard( cpGuard );
	for( ConnectionPointList::iterator it = points.begin(); it != points.end(); ++it )
	{
		if( (*it)->getInterface() == interface_ )
		{
			points.erase( it );
			return;
		}
	}
}

HRESULT ConnectionPointContainer::getCallback( const IID& interface_, IUnknown** callBack_ )
{
	lock::ScopeGuard guard( cpGuard );
	for( ConnectionPointList::iterator it = points.begin(); it != points.end(); ++it )
	{
		if( (*it)->getInterface() == interface_ )
		{
			IUnknown *unkn = (*it)->getCallBack();
			if( unkn != NULL )
				return unkn->QueryInterface( interface_, (void**)callBack_ );
		}
	}
	return E_FAIL;
}
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
