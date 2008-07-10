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

namespace frl
{
namespace opc
{

class EnumOPCItemAttributes : public IEnumOPCItemAttributes, public os::win32::com::Allocator
{
private:
	// reference counter
	#if( FRL_COMPILER == FRL_COMPILER_MSVC )
		volatile LONG refCount;
	#else
		LONG refCount;
	#endif

	std::vector<OPCITEMATTRIBUTES*> itemList; // Attributes array
	size_t curIndex; // Current element

	OPCITEMATTRIBUTES* copy( OPCITEMATTRIBUTES *i );
public:
	EnumOPCItemAttributes();
	virtual ~EnumOPCItemAttributes();
	void addItem( OPCHANDLE first,  const GroupItemElem &i );

	// the IUnknown Functions
	STDMETHODIMP QueryInterface( REFIID iid, LPVOID* ppInterface );
	STDMETHODIMP_(ULONG) AddRef( void);
	STDMETHODIMP_(ULONG) Release( void);

	// the IEnum Functions
	virtual STDMETHODIMP Next( ULONG celt, OPCITEMATTRIBUTES **ppItemArray, ULONG *pceltFetched );
	virtual STDMETHODIMP Skip ( ULONG celt );
	virtual STDMETHODIMP Reset( void );
	virtual STDMETHODIMP Clone( IEnumOPCItemAttributes **ppEnum );
}; // class EnumOPCItemAttributes

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_enum_item_attributes_h_
