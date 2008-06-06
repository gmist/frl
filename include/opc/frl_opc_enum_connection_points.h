#ifndef frl_opc_enum_connection_points_h_
#define frl_opc_enum_connection_points_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include <OCIdl.h>
#include <list>
#include "os/win32/com/frl_os_win32_com_allocator.h"
#include "frl_smart_ptr.h"

namespace frl
{
namespace opc
{

class ConnectionPoint;
typedef ComPtr< ConnectionPoint > ConnectionPointElem;
typedef std::list< ConnectionPointElem > ConnectionPointList;

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
	ConnectionPointList points;
public:
	
	EnumConnectionPoints();
	EnumConnectionPoints( const ConnectionPointList &pointsList );
	virtual ~EnumConnectionPoints();

	// IUnknown implementation
	STDMETHODIMP QueryInterface( REFIID iid, LPVOID* ppInterface);
	STDMETHODIMP_(ULONG) AddRef( void);
	STDMETHODIMP_(ULONG) Release( void);

	// IEnumConnectionPoints implementation
	virtual HRESULT STDMETHODCALLTYPE Next( 
		/* [in] */ ULONG cConnections,
		/* [length_is][size_is][out] */ LPCONNECTIONPOINT *ppCP,
		/* [out] */ ULONG *pcFetched);

	virtual HRESULT STDMETHODCALLTYPE Skip( 
		/* [in] */ ULONG cConnections);

	virtual HRESULT STDMETHODCALLTYPE Reset( void);

	virtual HRESULT STDMETHODCALLTYPE Clone( 
		/* [out] */ IEnumConnectionPoints **ppEnum);
};

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_enum_connection_points_h_
