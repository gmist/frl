#ifndef frl_opc_impl_browse_server_address_space_h_
#define frl_opc_impl_browse_server_address_space_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <boost/thread/mutex.hpp>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_server_base.h"

namespace frl { namespace opc { namespace impl {

/*!
	\brief
		IOPCBrowseServerAddressSpace implementation.
	\details
		This interface provides a way for clients to browse the available data items in the server, 
		giving the user a list of the valid definitions for an ITEM ID. 
		It allows for either flat or hierarchical address spaces and is designed to work well over 
		a network. It also insulates the client from the syntax of a server vendor specific ITEM ID.

		NOTE: Version 1.0A of the specification stated that each instance of this interface was 
		a separate object (like an enumerator), which would have allowed multiple 
		independent browse sessions by the same client on the server address space. 
		This turns out to be in violation of the rules of COM and as a result it does 
		not work in combination with DCOM. 
		In practice, this interface MUST be implemented (like any other interface) 
		as a separate interface on the single underlying Data Access Object. 
		The text of this section has been modified to reflect this. 
		Note that the "footprint" of the interface is unchanged for 2.0.

		Note that the Data Access Server object maintains state information related to browsing 
		(i.e. the current position in the address hierarchy) on behalf of the client using this interface. 
		Since there is just one underlying Server object, there is just a single copy of this state information. 
		Therefore the client CANNOT create a separate and independent browser object by doing 
		a second QueryInterface for IOPCBrowseServerAddressSpace 
		(Doing this would simply give him a second copy of the original interface). 
		If a second, independent browser object is required by a client, the client would need to create a 
		second OPC Data Access Object and perform a QueryInterface for IOPCBrowseServerAddressSpace on that object.

		It is assumed that the underlying server address space is either flat or hierarchical. 
		A flat space will always be presented to the client as Flat. 
		A hierarchical space can be presented to the client as either flat or hierarchical.

		A hierarchical presentation of the server address space would behave much like a file system, 
		where the directories are the branches or paths, and the files represent the leaves or items. 
		 For example, a server could present a control system by showing all the control networks, 
		 then all of the devices on a selected network, and then all of the classes of data within a device, 
		 then all of the data items of that class.  A further breakdown into vendor specific 
		 "Units" and "Lines" might be appropriate for a BATCH system.

		The browse position is initially set to the "root" of the address space. 
		The client can optionally choose a starting point within a hierarchical space 
		by calling ChangeBrowsePosition using OPC_BROWSE_TO.  For a FLAT space this is ignored. 
		For a HIERARCHICAL space you may pass any partial path (or a pointer to a NUL string to indicate the root). 
		This sets an initial position from which you can browse up or down.

		The Client can browse the items below (contained in) the current position via BrowseOPCItemIDs. 
		For a hierarchical space you can specify BRANCH (which returns things on that level with children) 
		or LEAF (things on that level without children)- or FLAT (everything including children of children). 
		This gives you back a String enumerator.

		This browse can also be filtered by a vendor specific filter string, by data type, or by Access Rights.

		In a hierarchy, the enumerator will return "short" strings; the name of the "child". 
		These short strings will generally not be sufficient for AddItem. 
		The client should always convert this short string to a "fully qualified" string via GetItemID. 
		For example the short string might be TIC101; the fully qualified string might be 
		AREA1.REACTOR5.TIC101. Note that the Server fills in any needed delimiters.

		This ItemID can optionally be passed to BrowseAccessPaths to get a list 
		of valid access paths to this item. (this returns another string enumerator).

		If the client browsed for BRANCHs (things with children) then he can pass 
		the result (short string) to ChangeBrowsePosition to move "down". 
		This method can also move "up" in which case the short string is not used.

Examples of a Hierarchical Space:

\section BrowseAddressSpace_Ex1 Example 1
\code
<ROOT>
	AREA1 (branch)
		REACTOR10 (branch)
			TIC1001 (branch)
				CURRENT_VALUE (leaf)
				SETPOINT
				ALARM_STATUS
				LOOP_DESCIPTION
			TIC1002
		CURRENT_VALUE
		etc...
		REACTOR11
			etc...
	AREA2
		etc...
\endcode

\section BrowseAddressSpace_Ex2 Example 2
\code
<ROOT>
	PLC_STATION_1 (branch)
		ANALOG_VALUES (branch)
			40001 (leaf)
			40002
			etc...
\endcode
*/
class BrowseServerAddressSpace
	:	public IOPCBrowseServerAddressSpace,
		virtual public opc::OPCServerBase
{
private:
	boost::mutex bsaScopeGuard;

public:

	virtual ~BrowseServerAddressSpace();

	STDMETHODIMP QueryOrganization( 
		/* [out] */ OPCNAMESPACETYPE *pNameSpaceType );

	STDMETHODIMP ChangeBrowsePosition( 
		/* [in] */ OPCBROWSEDIRECTION dwBrowseDirection,
		/* [string][in] */ LPCWSTR szString );

	STDMETHODIMP BrowseOPCItemIDs( 
		/* [in] */ OPCBROWSETYPE dwBrowseFilterType,
		/* [string][in] */ LPCWSTR szFilterCriteria,
		/* [in] */ VARTYPE vtDataTypeFilter,
		/* [in] */ DWORD dwAccessRightsFilter,
		/* [out] */ LPENUMSTRING *ppIEnumString );

	STDMETHODIMP GetItemID( 
		/* [in] */ LPWSTR szItemDataID,
		/* [string][out] */ LPWSTR *szItemID );

	STDMETHODIMP BrowseAccessPaths( 
		/* [string][in] */ LPCWSTR szItemID,
		/* [out] */ LPENUMSTRING *ppIEnumString );
}; // class BrowseServerAddressSpace

} // namespace impl
}
}

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_impl_browse_server_address_space_h_
