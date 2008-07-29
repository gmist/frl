#ifndef frl_opc_enum_item_attributes_h_
#define frl_opc_enum_item_attributes_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <vector>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_group_item.h"
#include "opc/address_space/frl_opc_address_space.h"
#include "opc/frl_opc_util.h"
#include "os/win32/com/frl_os_win32_com_allocator.h"

namespace frl{ namespace opc{ namespace impl{

class EnumOPCItemAttributes
	:	public IEnumOPCItemAttributes,
		public os::win32::com::Allocator
{
private:
	// reference counter
	#if( FRL_COMPILER == FRL_COMPILER_MSVC )
		volatile LONG refCount;
	#else
		LONG refCount;
	#endif

	std::vector<OPCITEMATTRIBUTES*> itemList; // attributes array
	size_t curIndex; // current element

	void copy( OPCITEMATTRIBUTES& dst, OPCITEMATTRIBUTES& src );
public:
	EnumOPCItemAttributes();
	virtual ~EnumOPCItemAttributes();
	void addItem( OPCHANDLE first,  const GroupItemElem& i );

	// the IUnknown functions
	STDMETHODIMP QueryInterface( REFIID iid, LPVOID* ppInterface );
	STDMETHODIMP_(ULONG) AddRef( void);
	STDMETHODIMP_(ULONG) Release( void);

	// the IEnum functions
	STDMETHODIMP Next( ULONG celt, OPCITEMATTRIBUTES** ppItemArray, ULONG* pceltFetched );
	STDMETHODIMP Skip ( ULONG celt );
	STDMETHODIMP Reset( void );
	STDMETHODIMP Clone( IEnumOPCItemAttributes** ppEnum );
}; // class EnumOPCItemAttributes

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_enum_item_attributes_h_
