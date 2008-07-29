#include "opc/frl_opc_connection_point_container.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <algorithm>
#include <OleCtl.h>
#include <boost/foreach.hpp>
#include "opc/frl_opc_enum_connection_points.h"
#include "opc/frl_opc_connection_point.h"

namespace frl{ namespace opc{

ConnectionPointContainer::ConnectionPointContainer()
{
}

ConnectionPointContainer::~ConnectionPointContainer()
{
}

STDMETHODIMP ConnectionPointContainer::EnumConnectionPoints(
	/* [out] */ IEnumConnectionPoints **ppEnum )
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

STDMETHODIMP ConnectionPointContainer::FindConnectionPoint(
	/* [in] */ REFIID riid,
	/* [out] */ IConnectionPoint **ppCP )
{
	if( ppCP == NULL )
		return E_POINTER;

	boost::mutex::scoped_lock guard( cpGuard );
	BOOST_FOREACH( ConnectionPointElem& el, points )
	{
		if( el->getInterface() == riid )
			return el->QueryInterface( IID_IConnectionPoint, (void**)ppCP );
	}
	return CONNECT_E_NOCONNECTION;
}

frl::Bool ConnectionPointContainer::isConnected( const IID &interface_ )
{
	boost::mutex::scoped_lock guard( cpGuard );
	BOOST_FOREACH( ConnectionPointElem& el, points )
	{
		if( el->getInterface() == interface_ )
			return el->isConnected();
	}
	return False;
}

void ConnectionPointContainer::registerInterface( const IID& interface_ )
{
	boost::mutex::scoped_lock guard( cpGuard );
	BOOST_FOREACH( ConnectionPointElem& el, points )
	{
		if( el->getInterface() == interface_ )
			return; // already registration
	}
	points.push_back( ConnectionPointElem( new ConnectionPoint( interface_, this ) ) );
}

void ConnectionPointContainer::unregisterInterface( const IID& interface_ )
{
	boost::mutex::scoped_lock guard( cpGuard );
	ConnectionPointList::iterator end = points.end();
	for( ConnectionPointList::iterator it = points.begin(); it != end; ++it )
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
	boost::mutex::scoped_lock guard( cpGuard );
	BOOST_FOREACH( ConnectionPointElem&el, points )
	{
		if( el->getInterface() == interface_ )
		{
			IUnknown *unkn = el->getCallBack();
			if( unkn != NULL )
				return unkn->QueryInterface( interface_, (void**)callBack_ );
		}
	}
	return E_FAIL;
}

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
