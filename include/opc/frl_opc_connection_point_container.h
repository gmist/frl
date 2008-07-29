#ifndef frl_opc_connection_point_container_h_
#define frl_opc_connection_point_container_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <OCIdl.h>
#include <list>
#include <boost/thread/mutex.hpp>
#include "frl_types.h"
#include "frl_smart_ptr.h"

namespace frl{ namespace opc{

class ConnectionPoint;
typedef ComPtr< ConnectionPoint > ConnectionPointElem;
typedef std::list< ConnectionPointElem > ConnectionPointList;

class ConnectionPointContainer : public IConnectionPointContainer
{
private:
	ConnectionPointList points;
	boost::mutex cpGuard;

protected:
	void registerInterface( const IID& interface_ );
	void unregisterInterface( const IID& interface_ );
	Bool isConnected( const IID &interface_ );

public:

	ConnectionPointContainer();
	virtual ~ConnectionPointContainer();			

	HRESULT getCallback( const IID& interface_, IUnknown** callBack_ );

	// IConnectionPointContainer implementation
	STDMETHODIMP EnumConnectionPoints( 
		/* [out] */ IEnumConnectionPoints** ppEnum );

	STDMETHODIMP FindConnectionPoint( 
		/* [in] */ REFIID riid,
		/* [out] */ IConnectionPoint** ppCP );

	virtual void OnAdvise(REFIID riid, DWORD dwCookie) {}
	virtual void OnUnadvise(REFIID riid, DWORD dwCookie) {}
};

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_connection_point_container_h_
