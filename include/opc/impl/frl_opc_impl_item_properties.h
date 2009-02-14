#ifndef frl_opc_impl_item_properties_h_
#define frl_opc_impl_item_properties_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"

namespace frl { namespace opc { namespace impl {

/*!
	\brief
		IOPCItemProperties implementation.
	\details
		IOPCItemProperties interface can be used by clients to browse the available properties 
		(also referred to as attributes or parameters) associated with an 
		ITEMID and to read the current values of these properties. 
		In some respects the functionality is similar to that provided by BrowseServerAddressSpace, 
		by EnumItemAttributes and by the SyncIO::Read function. 
		It differs from these interfaces in two important respects; 
		(a) it is intended be much easier to use and 
		(b) it is not optimized for efficient access to large amounts of data. 
		Rather it is intended to allow an application to easily browse 
		and read small amounts of additional information specific to a particular ITEMID. 

		The design of this interface is based upon the assumption is that many ITEMIDs 
		are associated with other ITEMIDs which represent related values such 
		as Engineering Units range or Description or perhaps Alarm Status. 
		For example the system might be built internally of "records" 
		which represent complex objects (like PID Controllers, Timers, Counters, Analog Inputs, etc). 
		These record items would have properties 
		(like current value, setpoint, hi alarm limit, low alarm limit, description, etc). 

		As a result, this interface allows a flexible and convenient way to browse, 
		locate and read this related information without imposing 
		any particular design structure on the underlying system.

		It also allows such information to be read without the need to create and manage OPCGroups.

		In most cases, a system like the one above (i.e. one composed internally of "records") 
		would also expose a hierarchical address space to OPC in the form of A100 
		as a "branch" and A100.CV, A100.SP, A100.OUT, A100.DESC as "leafs". 
		In other words, the properties of an item which happens to be 
		a record will generally map into lower level ITEMIDS. 
		Another way to look at this is that things that have properties 
		like A100 are going to be things that show up as "Branch" Nodes in the OPC Browser 
		and things that are properties are going to show up as 'Leaf' nodes in the OPC Browser.

		Note that the A100 item could in fact be embedded in a higher level "Plant.Building.Line" 
		hierarchy however for the moment we will ignore this as it is not relevant to this discussion.

		So, the general intent of this interface is to provide a way, 
		given an ITEMID of any one of a number of related its properties 
		(like A100.CV or A100.DESC or even A100), to identify the other related properties.

		Before we begin however it should be noted that the first 6 properties 
		(the OPC Specific Property Set 1) are "special cases" in that they represent data 
		that would exist within the OPC Server if this item were added to 
		an OPC Group and do not represent properties of the "real" tag record in the underlying system. 
		As a result, these particular property IDs will generally behave differently 
		in the methods on this interface as described below.
*/
class OPCItemProperties
	:	public IOPCItemProperties
{
public:
	virtual ~OPCItemProperties();

	STDMETHODIMP QueryAvailableProperties( 
		/* [in] */ LPWSTR szItemID,
		/* [out] */ DWORD *pdwCount,
		/* [size_is][size_is][out] */ DWORD **ppPropertyIDs,
		/* [size_is][size_is][out] */ LPWSTR **ppDescriptions,
		/* [size_is][size_is][out] */ VARTYPE **ppvtDataTypes);

	STDMETHODIMP GetItemProperties( 
		/* [in] */ LPWSTR szItemID,
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ DWORD *pdwPropertyIDs,
		/* [size_is][size_is][out] */ VARIANT **ppvData,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);

	STDMETHODIMP LookupItemIDs( 
		/* [in] */ LPWSTR szItemID,
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ DWORD *pdwPropertyIDs,
		/* [size_is][size_is][string][out] */ LPWSTR **ppszNewItemIDs,
		/* [size_is][size_is][out] */ HRESULT **ppErrors);
}; // class OPCItemProperties

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_impl_item_properties_h_
