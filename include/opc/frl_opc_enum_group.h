#ifndef frl_opc_enum_group_h_
#define frl_opc_enum_group_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <Windows.h>
#include <vector>
#include "opc/frl_opc_group_manager.h"
#include "os/win32/com/frl_os_win32_com_allocator.h"

namespace frl
{
namespace opc
{

class EnumGroup : public  IEnumUnknown, public os::win32::com::Allocator
{
private:
	// reference counter
	#if( FRL_COMPILER == FRL_COMPILER_MSVC )
		volatile LONG refCount;
	#else
		LONG refCount;
	#endif

	size_t currentIndex;	//Current element
	std::vector< GroupElem > groupList;

public:
	EnumGroup();
	EnumGroup( const std::vector< GroupElem > &groups );
	virtual ~EnumGroup();

	// IUnknown implementation
	STDMETHODIMP QueryInterface( REFIID iid, LPVOID* ppInterface);
	STDMETHODIMP_(ULONG) AddRef( void);
	STDMETHODIMP_(ULONG) Release( void);
	
	// the OPCEnum Functions
	virtual STDMETHODIMP Next (
		ULONG Requested,
		IUnknown **ppGrp,
		ULONG *pActual
		);

	virtual STDMETHODIMP Skip (
		ULONG celt
		);

	virtual STDMETHODIMP Reset(
		void
		);

	virtual STDMETHODIMP Clone( 
		IEnumUnknown **ppEnum
		);
}; // class EnumGroup

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_enum_group_h_
