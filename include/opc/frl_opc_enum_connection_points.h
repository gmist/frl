#ifndef frl_opc_enum_connection_points_h_
#define frl_opc_enum_connection_points_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <OCIdl.h>
#include <list>
#include <vector>
#include "os/win32/com/frl_os_win32_com_allocator.h"
#include "frl_smart_ptr.h"

namespace frl{ namespace opc{

class ConnectionPoint;
typedef ComPtr< ConnectionPoint > ConnectionPointElem;
typedef std::list< ConnectionPointElem > ConnectionPointList;
typedef std::vector< ConnectionPointElem > ConnectionPointVec;

class EnumConnectionPoints : public IEnumConnectionPoints, public os::win32::com::Allocator
{
private:
	// reference counter
	#if( FRL_COMPILER == FRL_COMPILER_MSVC )
		volatile LONG refCount;
	#else
		LONG refCount;
	#endif

	size_t currentIndex;	//Current element
	ConnectionPointVec points;
public:
	
	EnumConnectionPoints();
	EnumConnectionPoints( const EnumConnectionPoints& other );
	EnumConnectionPoints( const ConnectionPointList& pointsList );
	virtual ~EnumConnectionPoints();

	// IUnknown implementation
	STDMETHODIMP QueryInterface( REFIID iid, LPVOID* ppInterface);
	STDMETHODIMP_(ULONG) AddRef( void );
	STDMETHODIMP_(ULONG) Release( void );

	// IEnumConnectionPoints implementation
	STDMETHODIMP Next( ULONG celt, LPCONNECTIONPOINT* rgelt, ULONG* pceltFetched );
	STDMETHODIMP Skip( ULONG celt );
	STDMETHODIMP Reset();
	STDMETHODIMP Clone( IEnumConnectionPoints** ppEnum );
};

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_enum_connection_points_h_
