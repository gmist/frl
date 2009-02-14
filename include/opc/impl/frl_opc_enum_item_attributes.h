#ifndef frl_opc_enum_item_attributes_h_
#define frl_opc_enum_item_attributes_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <vector>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "os/win32/com/frl_os_win32_com_allocator.h"
#include "opc/frl_opc_item_attributes.h"

namespace frl{ namespace opc{ namespace impl{

/*!
	\brief
		IEnumOPCItemAttributes implementation.
	\details
		IEnumOPCItemAttributes allows a client to find out the contents (items) of a group and the attributes of those items.\n
		NOTE: most of the returned information was either supplied by or returned to the client at the time it called AddItem.\n
		The optional EU information (see the OPCITEMATTRIBUTES discussion) may be very useful to some clients. 
		This interface is also useful for debugging or for enumerating the contents of a public group.\n
		This interface is returned only by IOPCItemMgt::CreateEnumerator. It is not available through query interface.\n
		Since enumeration is a standard interface this is described only briefly.\n
		See the OLE Programmer's reference for Enumerators for a list and discussion of error codes. 
		\todo Add OLE Programmer's reference
*/
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

	std::vector< opc::ItemAttributes > itemList; // attributes array
	size_t curIndex; // current element

public:
	EnumOPCItemAttributes();
	EnumOPCItemAttributes( const EnumOPCItemAttributes& other );
	virtual ~EnumOPCItemAttributes();
	void addItem( const std::pair< OPCHANDLE, GroupItemElem >& newItem );

	// the IUnknown methods
	STDMETHODIMP QueryInterface( REFIID iid, LPVOID* ppInterface );
	STDMETHODIMP_(ULONG) AddRef( void);
	STDMETHODIMP_(ULONG) Release( void);

	// the IEnum methods
	STDMETHODIMP Next( ULONG celt, OPCITEMATTRIBUTES** ppItemArray, ULONG* pceltFetched );
	STDMETHODIMP Skip ( ULONG celt );
	STDMETHODIMP Reset( void );
	STDMETHODIMP Clone( IEnumOPCItemAttributes** ppEnumItemAttributes );
}; // class EnumOPCItemAttributes

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_enum_item_attributes_h_
