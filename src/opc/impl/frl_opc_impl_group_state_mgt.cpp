#include "opc/impl/frl_opc_impl_group_state_mgt.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <boost/make_shared.hpp>
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "opc/frl_opc_server.h"
#include "opc/frl_opc_group.h"

namespace frl { namespace opc { namespace impl {

/*! Dtor */
GroupStateMgt::~GroupStateMgt()
{
}

/*!
	Get the current state of the group.

	\param pUpdateRate
		The current update rate. The Update Rate is in milliseconds

	\param pActive
		The current active state of the group.

	\param ppName
		The current name of the group

	\param pTimeBias
		The TimeZone Bias of the group (in minutes)

	\param pPercentDeadband
		The percent change in an item value that will cause an exception 
		report of that value to a client. This parameter only applies to 
		items in the group that have dwEUType of Analog. 
		[See discussion of Percent Deadband in General Properties Section]
		\todo Add General Properties Section

	\param pLCID
		The current LCID for the group.

	\param phClientGroup
		The client supplied group handle

	\param phServerGroup
		The server generated group handle

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory

	\retval E_INVALIDARG
		An argument to the function was invalid.

	\retval S_OK
		The operation succeeded.

	\section GroupStateMgt_GetState_Comments Comments
	\arg
		This function is typically called to obtain the current 
		values of this information prior to calling SetState. 
		This information was all supplied by or returned to 
		the client when the group was created. 
		This function is also useful for debugging.
	\arg
		All out arguments must be valid pointers. 
		The marshaling mechanism requires valid pointers 
		for proper behavior. NULL pointers will throw an RPC exception.
	\arg
		The client must free the returned ppName string
*/
STDMETHODIMP GroupStateMgt::GetState( /* [out] */ DWORD *pUpdateRate, /* [out] */ BOOL *pActive, /* [string][out] */ LPWSTR *ppName, /* [out] */ LONG *pTimeBias, /* [out] */ FLOAT *pPercentDeadband, /* [out] */ DWORD *pLCID, /* [out] */ OPCHANDLE *phClientGroup, /* [out] */ OPCHANDLE *phServerGroup )
{
	if( deleted )
		return E_FAIL;
	
	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		*ppName = util::duplicateString( name );
	#else
		*ppName = util::duplicateString( string2wstring( name ) );
	#endif

	*pActive = actived;
	*pUpdateRate = updateRate;
	*phServerGroup = getServerHandle();
	*phClientGroup = clientHandle;
	*pTimeBias = timeBias;
	*pPercentDeadband = deadband;
	*pLCID = localeID;
	return S_OK;
}

/*!
	Client can set various properties of the group. Pointers to 
	'in' items are used so that the client can omit properties 
	he does not want to change by passing a NULL pointer.
	\b Note:
	The pRevisedUpdateRate argument must contain a valid pointer.

	\param pRequestedUpdateRate
		New update rate requested for the group by the client (milliseconds).

	\param pRevisedUpdateRate
		Closest update rate the server is able to provide for this group.

	\param pActive
		TRUE (non-zero) to active the group. FALSE (0) to deactivate the group.

	\param pTimeBias
		TimeZone Bias if Group (in minutes). See GroupStateMgt_SetState "Comments" under.

	\param pPercentDeadband
		The percent change in an item value that will cause an 
		exception report of that value to a client. 
		This parameter only applies to items in the group 
		that have dwEUType of Analog. 
		See discussion of Percent Deadband in the General Information Section.
		\todo Add General Information Section

	\param pLCID
		The Localization ID to be used by the group.

	\param phClientGroup
		New client supplied handle for the group. 
		This handle is returned in the data stream provided 
		to the client's IAdvise by the Groups IDataObject.

	\section GroupStateMgt_SetState Comments
		\arg
			For public groups, the server maintains unique state information 
			for each client for Active, pUpdateRate, TimeZone. 
			That is, the public groups behave as if each client had it's own private copy.
		\arg
			Refer to Data Acquisition Section for details on the behavior 
			of an OPC server with respect to the Synchronous 
			and Asynchronous interfaces and Active state of groups.
			\todo Add Data Acquisition Section
		\arg
			As noted in AddGroup the level of localization supported (dwLCID) 
			is entirely server specific. Servers which do not support 
			dynamic localization can ignore this parameter.
*/
STDMETHODIMP GroupStateMgt::SetState( /* [in][unique] */ DWORD *pRequestedUpdateRate, /* [out] */ DWORD *pRevisedUpdateRate, /* [in][unique] */ BOOL *pActive, /* [in][unique] */ LONG *pTimeBias, /* [in][unique] */ FLOAT *pPercentDeadband, /* [in][unique] */ DWORD *pLCID, /* [in][unique] */ OPCHANDLE *phClientGroup )
{
	if( deleted )
		return E_FAIL;

	HRESULT hResult = S_OK;
	if( pRevisedUpdateRate != NULL )
		*pRevisedUpdateRate = 0;

	// validate deadband
	if( pPercentDeadband != NULL )
	{
		if( *pPercentDeadband < 0 || *pPercentDeadband > 100 )
			return E_INVALIDARG;
		deadband = *pPercentDeadband;
	}

	boost::mutex::scoped_lock guard( groupGuard );

	if( pRequestedUpdateRate != NULL )
	{
		static const int maxUpdateRate = 100;
		DWORD dwUpdateRate = *pRequestedUpdateRate;
		if( dwUpdateRate == 0 || dwUpdateRate%maxUpdateRate != 0 )
		{
			dwUpdateRate = maxUpdateRate*( dwUpdateRate/maxUpdateRate + 1 );
			hResult = OPC_S_UNSUPPORTEDRATE;
		}

		*pRevisedUpdateRate = updateRate = dwUpdateRate;
	}

	if( pTimeBias != NULL )
		timeBias = *pTimeBias;

	if( pLCID != NULL )
		localeID = *pLCID;

	if( phClientGroup != NULL )
		clientHandle = *phClientGroup; 

	if( pActive != NULL )
	{
		Bool oldState = actived;
		if( (*pActive == TRUE ) || (*pActive == VARIANT_TRUE ) )
			actived = True;
		else
			actived = False;

		if( actived )
		{
			if( ! oldState )
			{
				if( itemList.size() )
				{
					if( isConnected( IID_IOPCDataCallback ) )
					{
						std::list< OPCHANDLE > handles;
						GroupItemElemList::iterator end = itemList.end();
						for( GroupItemElemList::iterator it = itemList.begin(); it != end; ++it )
						{
							if( (*it).second->isActived() )
								handles.push_back( (*it).first );
						}
						if( handles.size() )
						{
							GroupElem tmp = GroupElem( dynamic_cast< Group* >( this ) );
							//AsyncRequestListElem request( new AsyncRequest( tmp, async_request::UPDATE, handles) );
							doAsyncRefresh( boost::make_shared< AsyncRequest >( tmp, async_request::UPDATE, handles ) );
						}
					}
				}
				renewUpdateRate();
			}
		}
		else
		{
			server->removeGroupFromRequestList( getServerHandle() );
		}
	}
	return hResult;
}

/*!
	Change the name of a private group. The name must be unique. 
	The name cannot be changed for public groups.

	\param szName
		New name for group.

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory

	\retval E_INVALIDARG
		An argument to the function was invalid.

	\retval S_OK
		The operation succeeded.

	\retval OPC_E_DUPLICATENAME
		Duplicate name not allowed.

	\section GroupStateMgt_SetName_Comments Comments
		\arg
			Group names are required to be unique with 
			respect to an individual client to server connection.
*/
STDMETHODIMP GroupStateMgt::SetName( /* [string][in] */ LPCWSTR szName )
{
	if( deleted )
		return E_FAIL;

	if( szName == NULL || wcslen( szName ) == 0 )
		return E_INVALIDARG;

	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		String new_name = szName;
	#else
		String new_name = wstring2string( szName );
	#endif

	HRESULT res = server->setGroupName( name, new_name );
	if( FAILED( res ) )
		return res;
	name = name;
	return S_OK;
}

/*!
		Creates a second copy of a group with a unique name.
		This works for both public and private groups. However, 
		the new group is always a private group. 
		All of the group and item properties are duplicated 
		(as if the same set of AddItems calls had been made for the new group). 
		That is, the new group contains the same update rate, items, 
		group and item client handles, requested data types, etc as the original group. 
		Once the new group is created it is entirely independent of the old group. 
		You can add and delete items from it without affecting the old group.

		\b Note: Properties \b NOT copied to the new group are
		\arg
			Active Status of the new group is initially set to FALSE
		\arg
			A new ServerHandle for the group is produced.
		\arg
			New Item SeverHandles may also be assigned by the server. 
			The client should query for these if it needs them.
		\arg
			The new group will \b NOT be connected to any Advise or Connection point sinks. 
			The client would need to establish new connections for the new group.

	\param szName
		Name of the group. 
		The name must be unique among the other groups 
		created by this client. 
		If no name is provided (szName is a pointer to a NUL string) 
		the server will generate a unique name. 
		The server generated name will also be unique relative to any existing public groups.

	\param riid
		Requested interface type.

	\param ppUnk
		Place to return interface pointer. 
		NULL is returned for any HRESULT other than S_OK

	\retval S_OK
		The operation succeeded.

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory

	\retval E_INVALIDARG
		An argument to the function was invalid.

	\retval OPC_E_DUPLICATENAME
		Duplicate name not allowed.

	\retval E_NOINTERFACE
		The interface(riid) asked for is not supported by the server.

		\section GroupStateMgt_CloneGroup_Comments Comments
		\arg
		This represents a new group which is independent of the original group. 
		See AddGroup for a discussion of Group object lifetime issues. 
		As with AddGroup the group must be deleted with RemoveGroup when the client is done with it.
		\arg
		The client must also release the returned interface when it is no longer needed.
		\arg
		The primary use or intent of this function is to create a private duplicate 
		of a public group which can then be modified by the client.
*/
STDMETHODIMP GroupStateMgt::CloneGroup( /* [string][in] */ LPCWSTR szName, /* [in] */ REFIID riid, /* [iid_is][out] */ LPUNKNOWN *ppUnk )
{
	if( deleted )
		return E_FAIL;

	if( ppUnk == NULL )
		return E_INVALIDARG;

	String new_name;
	if( szName == NULL || wcslen( szName ) == 0 )
	{
		new_name = util::getUniqueName();
	}
	else
	{
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			new_name = szName;
		#else
			new_name = wstring2string( szName );
		#endif
	}

	GroupElem group;
	try
	{
		group = server->cloneGroup( name, new_name );
	}
	catch( GroupManager::IsExistGroup& )
	{
		return OPC_E_DUPLICATENAME;
	}
	catch( GroupManager::NotExistGroup& )
	{
		return E_INVALIDARG;
	}

	HRESULT result = group->QueryInterface( riid, (void**)ppUnk );

	if( FAILED( result ) )
	{
		server->RemoveGroup( group->getServerHandle(), FALSE );
	}

	return result;
}

/*!
	Clients can set the keep-alive time for a subscription to cause the server to 
	provide client callbacks on the subscription when there are no new events to report. 
	Clients can then be assured of the health of the server and subscription 
	without resorting to pinging the server with calls to GetStatus().
	Using this facility, a client can expect a callback (data or keep-alive)
	 within the specified keep-alive time.
	Servers shall reset their keep-alive timers when real data is sent 
	(i.e. it is not acceptable to constantly send the keep-alive callback 
	at a fixed period equal to the keep-alive time irrespective of data callbacks).

	\param dwKeepAliveTime
		The maximum amount of time (in milliseconds) between subscription callbacks.
		A value of zero indicates the client does not wish to receive any empty keep-alive callbacks.

	\param pdwRevisedKeepAliveTime
		The KeepAliveTime the server is actually providing, which may differ from .dwKeepAliveTime.

	\retval S_OK
		The function was successful.

	\retval E_INVALIDARG
		A bad parameter was passed.

	\retval E_FAIL
		The function was unsuccessful. 

	\retval OPC_S_UNSUPPORTEDRATE
		The function succeeded but pdwRevisedKeepAlive does not equal dwKeepAlive.

	\section GroupStateMgt_SetKeep_Alive_Comments Comments
		\arg
		The keep-alive callback consists of a call to 
		IOPCDataCallback:: OnDataChange() with dwCount set to zero.
		\arg
		Keep-alive callbacks will not occur when the subscription is inactive.
		\arg
		Keep-alive callbacks do not affect the value of 
		OPCSERVERSTATUS::ftLastUpdateTime returned by IOPCServer::GetStatus().
*/
STDMETHODIMP GroupStateMgt::SetKeepAlive( /* [in] */ DWORD dwKeepAliveTime, /* [out] */ DWORD *pdwRevisedKeepAliveTime )
{
	if( pdwRevisedKeepAliveTime == NULL )
		return E_INVALIDARG;

	if( deleted )
		return E_FAIL;

	if( dwKeepAliveTime == 0 )
	{
		keepAlive = *pdwRevisedKeepAliveTime = 0;
		return S_OK;
	}

	static const DWORD maxKeepAliveTime = 100000; // 100 sec

	DWORD tmp = dwKeepAliveTime; // for non blocked change Group::keepAlive
	if( tmp%maxKeepAliveTime != 0 )
	{
		tmp = maxKeepAliveTime * ( tmp / maxKeepAliveTime + 1 );
	}
	keepAlive = *pdwRevisedKeepAliveTime = tmp;

	if( tmp != dwKeepAliveTime )
		return OPC_S_UNSUPPORTEDRATE;
	return S_OK;
}

/*!
	Returns the currently active keep-alive time for the subscription.

	\param pdwKeepAliveTime
		The maximum amount of time (in milliseconds) between subscription callbacks.
		A value of zero indicates the server will not send any empty keep-alive callbacks.

	\retval S_OK
		The function was successful.

	\retval E_FAIL
		The function was unsuccessful.

	\section GroupStateMgt_GetKeepAlive_Comments Comments
		\arg
		If SetKeepAlive() has never been called, the returned value will be zero.
*/
STDMETHODIMP GroupStateMgt::GetKeepAlive( /* [out] */ DWORD *pdwKeepAliveTime )
{
	if( pdwKeepAliveTime == NULL )
		return E_INVALIDARG;

	if( deleted )
		return E_FAIL;

	*pdwKeepAliveTime = keepAlive;
	return S_OK;
}

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
