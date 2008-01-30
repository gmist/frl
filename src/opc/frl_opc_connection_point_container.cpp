#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <OleCtl.h>
#include "opc/frl_opc_connection_point_container.h"
#include "opc/frl_opc_enum_connection_points.h"
#include "opc/frl_opc_connection_point.h"

namespace frl
{
	namespace opc
	{
		ConnectionPointContainer::ConnectionPointContainer()
		{
		}

		ConnectionPointContainer::~ConnectionPointContainer()
		{
			for( std::vector< ConnectionPoint* >::iterator it = points.begin(); it != points.end(); ++it )
				delete (*it);
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

			lock::Mutex::ScopeGuard guard( cpGuard );
			std::vector< ConnectionPoint* >::iterator it;
			for( it = points.begin(); it != points.end(); ++it )
			{
				if( (*it)->getInterface() == riid )
					return (*it)->QueryInterface(IID_IConnectionPoint, (void**)ppCP);
			}

			return CONNECT_E_NOCONNECTION;
		}

		frl::Bool ConnectionPointContainer::isConnected( const IID &interface_ )
		{
			lock::Mutex::ScopeGuard guard( cpGuard );
			for( std::vector< ConnectionPoint* >::iterator it = points.begin(); it != points.end(); ++it )
			{
				if( (*it)->getInterface() == interface_ )
					return (*it)->isConnected();
			}
			return False;
		}

		void ConnectionPointContainer::registerInterface( const IID& interface_ )
		{
			lock::Mutex::ScopeGuard guard( cpGuard );
			ConnectionPoint* addedPoint = new ConnectionPoint( interface_, this );
			addedPoint->AddRef();
			points.push_back( addedPoint );
		}

		void ConnectionPointContainer::unregisterInterface( const IID& interface_ )
		{
			lock::Mutex::ScopeGuard guard( cpGuard );
			for( std::vector< ConnectionPoint* >::iterator it = points.begin(), remIt; it != points.end(); it = remIt )
			{
				remIt = it;
				++remIt;
				if( (*it)->getInterface() == interface_ )
				{
					ConnectionPoint* tmpPoint = (*it);
					points.erase( it );
					tmpPoint->deleting();
					return;
				}
			}
		}

		HRESULT ConnectionPointContainer::getCallback( const IID& interface_, IUnknown** callBack_ )
		{
			lock::Mutex::ScopeGuard guard( cpGuard );
			for( std::vector< ConnectionPoint* >::iterator it = points.begin(); it != points.end(); ++it )
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
} // namespace FatRat Library

#endif // FRL_PLATFORM_WIN32
