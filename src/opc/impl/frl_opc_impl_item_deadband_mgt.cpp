#include "opc/impl/frl_opc_impl_item_deadband_mgt.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcerror.h"

namespace frl { namespace opc { namespace impl {

/*! Dtor */
ItemDeadBandMgt::~ItemDeadBandMgt()
{
}

/*!
	Overrides the deadband specified for the group for each item.

	\param[in] dwCount
		The number of items to be affected

	\param[in] phServer
		Array of Server items handles.

	\param[in] pPercentDeadband
		Array of deadband values.
		 Each value must be from 0.0 to 100.0, 
		 which is the percentage of the change allowed per update period.

	\param[out] ppErrors
		Array of HRESULT's. Indicates the results of setting the deadband for each item.

	\retval S_OK
		The function was successful.

	\retval S_FALSE
		The function was partially successful. 
		See the ppErrors to determine what happened

	\retval E_INVALIDARG
		An argument to the function was invalid.

	\retval OPC_E_DEADBANDNOTSUPPORTED
		The server does not support deadband.

	\retval E_FAIL
		The function was unsuccessful. 

	\section ItemDeadBandMgt_SetItemDeadband_ppErrors ppErrors codes
		\arg \b S_OK - 
			The function was successful.
		\arg \b OPC_E_INVALIDHANDLE - 
			The corresponding Item handle was invalid.

		\arg \b OPC_E_DEADBANDNOTSUPPORTED - 
			The item handle does not support deadband.

		\arg \b E_INVALIDARG - 
			Requested deadband was not in the range of 0.0 to 100.0.

	\section ItemDeadBandMgt_SetItemDeadband_Comments Comments
		\arg
			The percent change of an item value will cause a subscription callback for that value. 
			Only items in the group that have dwEUType of Analog can have a PercentDeadband set. 
			If the item does not have support deadband, then the ppError for that particular item will be set 
			to OPC_E_DEADBANDNOTSUPPORTED and the setting will have no effect on that particular item.
		\arg
			If the HRESULT is S_OK, then ppError can be ignored (all results in it are guaranteed to be S_OK).
		\arg
			If the HRESULT is any FAILED code then (as noted earlier) the server 
			should return NULL pointers for all OUT parameters.
		\arg
			If the HRESULT is S_FALSE, then the ppErrors array must be examined, 
			since at least one item has failed and at least one item was successful. 
			The client must examine the ppErrors array to make this determination.
		\arg
			The ppErrors array is allocated by the server and must be freed by the client.
		\arg
			The default deadband for the item is the group deadband. 
			See the Percent Deadband section for additional information.
			\todo Add Percent Deadband section
*/
STDMETHODIMP ItemDeadBandMgt::SetItemDeadband(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][in] */ FLOAT *pPercentDeadband,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if(	phServer == NULL ||
		pPercentDeadband == NULL ||
		ppErrors == NULL )
	{
		return E_INVALIDARG;
	}

	*ppErrors = NULL;

	if( dwCount == 0 )
	{
		return E_INVALIDARG;
	}

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	if( *ppErrors == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT result = S_OK;			
	boost::mutex::scoped_lock guard( groupGuard );
	GroupItemElemList::iterator it;
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		it = itemList.find( phServer[i] );
		if( it == end )
		{
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			result = S_FALSE;
			continue;
		}

		if( pPercentDeadband[i] < 0.0 || pPercentDeadband[i] > 100.0 )
		{
			(*ppErrors)[i] = E_INVALIDARG;
			result = S_FALSE;
			continue;
		}

		(*it).second->setDeadBand( pPercentDeadband[i] );
		(*ppErrors)[i] = S_OK;
	}
	return result;
}

/*!
	Gets the deadband values for each of the requested items.

	\param[in] dwCount
		The number of items to be affected

	\param[in] phServer
		Array of Server items handles.

	\param[out] ppPercentDeadband
		Array of deadband values.  Each successful value will be from 0.0 to 100.0, 
		which describes the percent deadband allowed for that particular item.

	\param[out] ppErrors
		Array of HRESULT's. Indicates the results of getting the deadband for each item.

	\retval S_OK
		The function was successful.

	\retval S_FALSE
		The function was partially successful. See the ppErrors to determine what happened
		( see \ref ItemDeadBandMgt_GetItemDeadband_ppErrors ).

	\retval E_INVALIDARG
		An argument to the function was invalid.

	\retval OPC_E_DEADBANDNOTSUPPORTED
		The server does not support deadband.

	\retval E_FAIL
		The function was unsuccessful.

	\section ItemDeadBandMgt_GetItemDeadband_ppErrors ppErrors codes
		\arg \b S_OK - 
			The function was successful.
		\arg \b OPC_E_INVALIDHANDLE - 
			The corresponding Item handle was invalid.
		\arg \b OPC_E_DEADBANDNOTSUPPORTED - 
			The item does not support deadband.
		\arg \b OPC_E_DEADBANDNOTSET - 
			The item deadband has not been set for this item.

	\section ItemDeadBandMgt_GetItemDeadband_Comments Comments
		\arg
			Only items in the group that have dwEUType of Analog can have a PercentDeadband. 
			If the item does not support deadband, then the ppError for that particular item 
			will be set to OPC_E_DEADBANDNOTSUPPORTED.
		\arg
			If SetItemDeadband did not previously set the PercentDeadband, 
			then no PercentDeadband for that particular item will be obtained 
			and the ppError for that item will be set to OPC_E_DEADBANDNOTSET.
		\arg
			If the HRESULT is S_OK, then ppError can be ignored 
			(all results in it are guaranteed to be S_OK).
		\arg
			If the HRESULT is any FAILED code then (as noted earlier) 
			the server should return NULL pointers for all OUT parameters.
		\arg
			If the HRESULT is S_FALSE, then the ppErrors array must be examined, 
			since at least one item has failed and at least one item was successful. 
			The client must examine the ppErrors array to make this determination.
		\arg
			The ppPercentDeadband and ppErrors arrays are allocated by 
			the server and must be freed by the client.
*/
STDMETHODIMP ItemDeadBandMgt::GetItemDeadband(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][size_is][out] */ FLOAT **ppPercentDeadband,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{	
	if( deleted )
		return E_FAIL;

	if(	phServer == NULL ||
		ppPercentDeadband == NULL ||
		ppErrors == NULL )
	{
		return E_INVALIDARG;
	}

	*ppPercentDeadband = NULL;
	*ppErrors = NULL;

	if( dwCount == 0 )
	{
		return E_INVALIDARG;
	}

	*ppPercentDeadband = os::win32::com::allocMemory< FLOAT >( dwCount );
	if( *ppPercentDeadband == NULL )
	{
		return E_OUTOFMEMORY;
	}

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	if( *ppErrors == NULL )
	{
		os::win32::com::freeMemory( *ppPercentDeadband );
		return E_OUTOFMEMORY;
	}

	HRESULT result = S_OK;
	boost::mutex::scoped_lock guard( groupGuard );
	Float tmpDeadBand;
	GroupItemElemList::iterator it;
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		it = itemList.find( phServer[i] );
		if( it == end )
		{
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			result = S_FALSE;
			continue;
		}

		tmpDeadBand = (*it).second->getDeadBand();
		if( tmpDeadBand == invalidDeadBand )
		{
			(*ppErrors)[i] = OPC_E_DEADBANDNOTSET;
			result = S_FALSE;
			continue;
		}

		(*ppPercentDeadband)[i] = tmpDeadBand;
		(*ppErrors)[i] = S_OK;
	}
	return result;
}

/*!
	Clears the individual item PercentDeadband, 
	effectively reverting them back to the deadband value set in the group.

	\param[in] dwCount
		The number of items to be affected

	\param[in] phServer
		Array of Server items handles.

	\param[out] ppErrors
		Array of HRESULT's. Indicates the results of clearing the deadband for each item 
		(see \ref ItemDeadBandMgt_ClearItemDeadband_ppErrors).

	\section ItemDeadBandMgt_ClearItemDeadband_ppErrors ppErrors codes
		\arg \b S_OK - 
			The function was successful.
		\arg \b OPC_E_INVALIDHANDLE -  
			The corresponding Item handle was invalid.
		\arg \b OPC_E_DEADBANDNOTSUPPORTED - 
			The item does not support deadband.
		\arg \b OPC_E_DEADBANDNOTSET - 
			The item deadband has not been set for this item.

	\section ItemDeadBandMgt_ClearItemDeadband_Comments Comments
		\arg
			Only items in the group that have dwEUType of Analog can have a PercentDeadband. 
			If the item does not support deadband, then the ppError for that particular 
			item will be set to OPC_E_DEADBANDNOTSUPPORTED.
		\arg
			If SetItemDeadband did not previously set the PercentDeadband, 
			then no PercentDeadband for that particular item will be obtained 
			and the ppError for that item will be set to OPC_E_DEADBANDNOTSET.
		\arg
			If the HRESULT is S_OK, then ppError can be 
			ignored (all results in it are guaranteed to be S_OK).
		\arg
			If the HRESULT is any FAILED code then (as noted earlier) 
			the server should return NULL pointers for all OUT parameters.
		\arg
			If the HRESULT is S_FALSE, then the ppErrors array must be examined, 
			since at least one item has failed and at least one item was successful. 
			The client must examine the ppErrors array to make this determination.
		\arg
			The ppErrors array is allocated by the server and must be freed by the client.
*/
STDMETHODIMP ItemDeadBandMgt::ClearItemDeadband(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if( phServer == NULL ||
		ppErrors == NULL )
	{
		return E_INVALIDARG;
	}

	*ppErrors = NULL;

	if( dwCount == 0 )
	{
		return E_INVALIDARG;
	}

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	if( *ppErrors == NULL )
	{
		return E_OUTOFMEMORY;
	}
	HRESULT result = S_OK;
	boost::mutex::scoped_lock guard( groupGuard );
	Float tmpDeadBand;
	GroupItemElemList::iterator it;
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		it = itemList.find( phServer[i] );
		if( it == end )
		{
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			result = S_FALSE;
			continue;
		}
		tmpDeadBand = (*it).second->getDeadBand();
		if( tmpDeadBand == invalidDeadBand )
		{
			(*ppErrors)[i] = OPC_E_DEADBANDNOTSET;
			result = S_FALSE;
			continue;
		}
		(*it).second->setDeadBand( invalidDeadBand );
	}
	return result;
}

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
