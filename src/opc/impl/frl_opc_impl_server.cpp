#include "opc/impl/frl_opc_impl_server.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "opc/frl_opc_enum_string.h"
#include "opc/frl_opc_server.h"
#include "opc/frl_opc_group.h"
#include "opc/frl_opc_enum_group.h"

namespace frl { namespace opc { namespace impl {

/*!
	Dtor
*/
OPCServer::~OPCServer()
{
}

/*!
	Set the new state of the server.

	\param newState
		The new state.

	\throw InvalidServerState
		If newState is not member of enum OPCSERVERSTATE.
*/
void OPCServer::setServerState( OPCSERVERSTATE newState )
{
	if( newState < OPC_STATUS_RUNNING || newState > OPC_STATUS_TEST )
		FRL_THROW_S_CLASS( InvalidServerState );
	serverStatus.dwServerState = newState;
	CoFileTimeNow( &serverStatus.ftCurrentTime );
}

/*!
	Returns the current state of the server.
*/
OPCSERVERSTATE OPCServer::getServerState()
{
	return serverStatus.dwServerState;
}

/*!
	Add a Group to a Server.

	\param szName
		Name of the group. The name must be unique among the other groups created by this client. 
		If no name is provided (szName is pointer to a NUL string) the server will generate a unique name. 
		The server generated name will also be unique relative to any existing public groups.

	\param bActive
		FALSE if the Group is to be created as inactive. TRUE if the Group is to be created as active.

	\param dwRequestedUpdateRate
		Client Specifies the fastest rate at which data changes may be sent 
		to OnDataChange for items in this group. This also indicates the desired accuracy of Cached Data.
		This is intended only to control the behavior of the interface. 
		How the server deals with the update rate and how often it actually polls the hardware internally 
		is an implementation detail.  Passing 0 indicates the server should use the fastest practical rate. 
		The rate is specified in milliseconds.

	\param hClientGroup
		Client provided handle for this group 
		(refer to description of data types, parameters, and structures for more information about this parameter).

	\param pTimeBias
		Pointer to Long containing the initial TimeBias (in minutes) for the Group. 
		Pass a NULL Pointer if you wish the group to use the default system TimeBias. 
		See discussion of TimeBias in General Properties Section See \ref OPCServer_AddGroup_Comments "Comments" below.

	\param pPercentDeadband
		The percent change in an item value that will cause a subscription callback for that value to a client. 
		This parameter only applies to items in the group that have dwEUType of Analog.
		(See discussion of Percent Deadband in General Properties Section).
		A NULL pointer is equivalent to 0.0.

	\param dwLCID
		The language to be used by the server when returning values (including EU enumeration's) 
		as text for operations on this group. 
		This could also include such things as alarm or status conditions or digital contact states.

	\param phServerGroup
		Place to store the unique server generated handle to the newly created group.
		The client will use the server provided handle for many of the subsequent functions 
		that the client requests the server to perform on the group.

	\param pRevisedUpdateRate
		The server returns the value it will actually use for the UpdateRate which may differ from the RequestedUpdateRate. 
		Note that this may also be slower than the rate at which the server is internally obtaining the data and updating the cache. 
		In general the server should 'round up' the requested rate to the next available supported rate. 
		The rate is specified in milliseconds. 
		Server returns HRESULT of OPC_S_UNSUPPORTEDRATE when it returns
		a value in revisedUpdateRate that is different than RequestedUpdateRate.

	\param riid
		The type of interface desired (e.g. IID_IOPCItemMgt)

	\param ppUnk
		Where to store the returned interface pointer. NULL is returned for any FAILED HRESULT.

	\retval S_OK
		The operation succeeded.

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory.

	\retval E_INVALIDARG
		An argument to the function was invalid.

	\retval OPC_E_DUPLICATENAME
		Duplicate name not allowed.

	\retval OPC_S_UNSUPPORTEDRATE
		Server does not support specified rate,  server returns the rate that it can support in the revised update rate.

	\retval E_NOINTERFACE
		The interface(riid) asked for is not supported by the server.

	\section Behavior
		A Group is a logical container for a client to organize and manipulate data items.
		The server will create a group object, and return a pointer to the interface requested by the client. 
		If the client requests an optional interface that the server does not support, 
		the server is expected to return an error indicating the interface is not supported.
		The requested update rate / revised update rate behavior should be deterministic between client / server sessions. 
		The client expects that for the same server configuration or workspace;
		adding a group with a requested update rate will always result in the same RevisedRate independent
		of the number of clients or items that have been added.

	\section OPCServer_AddGroup_Comments Comments
		\arg The expected object lifetime behavior is as follows. Even if all the interfaces are released, 
		the group will not be deleted until RemoveGroup is called. 
		One way for the server to implement this is to assign the group an initial reference count of 2; 
		one for the 'Add' and one for the Interface that was created. However, 
		clients should not make assumptions about the Group's reference count.
		\arg The client should not call RemoveGroup without releasing all interfaces for the group. 
		The client should also not release the server without removing all private groups. 
		\arg Since the server is the 'container' for the groups it is permissible for the server 
		to forcibly remove any remaining groups at the time all of the server interfaces are released. 
		(This should not be necessary for a well behaved client).
		\arg See also the CreateGroupEnumerator function.
		\arg The level of localization supported (dwLCID) is entirely server specific.  Servers which do not support dynamic localization can ignore this parameter.
		\arg See the MoveToPublic function for additional requirements related to public groups.
		\arg The default TimeBias for the group will be that of the system in which the group is created.


*/
STDMETHODIMP OPCServer::AddGroup(
	/* [string][in] */ LPCWSTR szName,
	/* [in] */ BOOL bActive,
	/* [in] */ DWORD dwRequestedUpdateRate,
	/* [in] */ OPCHANDLE hClientGroup,
	/* [in][unique] */ LONG *pTimeBias,
	/* [in][unique] */ FLOAT *pPercentDeadband,
	/* [in] */ DWORD dwLCID,
	/* [out] */ OPCHANDLE *phServerGroup,
	/* [out] */ DWORD *pRevisedUpdateRate,
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ LPUNKNOWN *ppUnk )
{
	if( phServerGroup == NULL || pRevisedUpdateRate == NULL || ppUnk == NULL )
		return E_INVALIDARG;

	*phServerGroup = 0;
	*pRevisedUpdateRate = 0;
	*ppUnk = NULL;

	if( pPercentDeadband )
	{
		if( *pPercentDeadband < 0 || *pPercentDeadband > 100 )
		{
			return E_INVALIDARG;
		}
	}

	GroupElem new_group;
	String name;
	boost::mutex::scoped_lock guard( scopeGuard );
	if( szName == NULL || wcslen( szName ) == 0 )
	{
		name = util::getUniqueName();
	}
	else
	{
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			name = szName;
		#else
			name = wstring2string( szName );
		#endif
	}

	try
	{
		new_group = group_manager.addGroup( name );
	}
	catch( GroupManager::IsExistGroup& )
	{
		return OPC_E_DUPLICATENAME;
	}

	LONG lTimeBias = 0;
	if (pTimeBias == NULL)
	{
		TIME_ZONE_INFORMATION cZoneInfo;
		GetTimeZoneInformation(&cZoneInfo);
		lTimeBias = cZoneInfo.Bias;
	}
	else
	{
		lTimeBias = *pTimeBias;
	}

	new_group->setServerPtr( dynamic_cast< opc::OPCServer*>( this ) );
	HRESULT res = new_group->SetState(
		&dwRequestedUpdateRate,
		pRevisedUpdateRate,
		&bActive,
		&lTimeBias,
		pPercentDeadband,
		&dwLCID,
		&hClientGroup );

	if( FAILED(res) )
	{
		group_manager.removeGroup( name );
		return res;
	}
	HRESULT queryResult = new_group->QueryInterface( riid, (void**)ppUnk );
	if( FAILED( queryResult ) || *ppUnk == NULL)
	{
		group_manager.removeGroup( name );
		return E_NOINTERFACE;
	}

	if( phServerGroup )
	{
		*phServerGroup = new_group->getServerHandle();
	}
	return res;
}

/*!
	Returns the error string for a server specific error code.

	\param dwError
		A server specific error code that the client application had returned from an interface function
		from the server, and for which the client application is requesting the server's textual representation.

	\param dwLocale
		The locale for the returned string.

	\param ppString
		Pointer to pointer where server supplied result will be saved.

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory;

	\retval E_INVALIDARG
		An argument to the function was invalid. (For example, the error code specified is not valid).

	\retval S_OK
		The operation succeeded.

	\section Comments
		\arg This is essentially the same function as is found in the newer IOPCCommon.
		\arg Note that if this method is called on a remote server, an RPC error may result. 
		\arg For this reason it is probably good practice for the client to attempt to call a local Win32 function if this function fails.
		\arg The expected behavior is that this will include handling of Win32 errors as well (such as RPC errors).
		\arg The Client must free the returned string.
		\arg It is recommended that the server put any OPC specific strings into an external resource to simplify translation.
		\arg To get the default value for the system, the dwLocale should be LOCALE_SYSTEM_DEFAULT.
*/
STDMETHODIMP OPCServer::GetErrorString(
	/* [in] */ HRESULT dwError,
	/* [in] */ LCID dwLocale,
	/* [string][out] */ LPWSTR *ppString )
{
	if( ppString == NULL )
	{
		return E_INVALIDARG;
	}
	*ppString = NULL;
	return util::getErrorString( dwError, dwLocale, &ppString );
}

/*!
	Given the name of a private group (created earlier by the same client), return an additional interface pointer. 
	Use GetPublicGroupByName to attach to public groups.

	\param szName
		The name of the group. That is the group must have been created by the caller.

	\param riid
		The type of interface desired for the group (e.g. IOPCItemMgt)

	\param ppUnk
		Pointer to where the group interface pointer should be returned. 
		NULL is returned for any HRESULT other than S_OK.

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory

	\retval E_INVALIDARG
		An argument to the function was invalid.

	\retval S_OK
		The operation succeeded.

	\retval E_NOINTERFACE
		The interface(riid) asked for is not supported by the server.

	\section Comments
		\arg This function can be used to reconnect to a private group for which all interface pointers have been released.
		\arg The client must release the returned interface when it is done with it.
		\arg If needed, the client can obtain the hServerGroup Handle via IOPCGroupStateMgt::GetState.
*/
STDMETHODIMP OPCServer::GetGroupByName(
	/* [string][in] */ LPCWSTR szName,
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ LPUNKNOWN *ppUnk )
{
	if( ppUnk == NULL )
		return E_INVALIDARG;
	*ppUnk = NULL;

	if( szName == NULL || wcslen( szName ) == 0 )
		return E_INVALIDARG;

	boost::mutex::scoped_lock guard( scopeGuard );

	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		String name = szName;
	#else
		String name = wstring2string( szName );
	#endif

	GroupElem group;
	try
	{
		group = group_manager.getGroup( name );
	}
	catch( GroupManager::NotExistGroup& )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = group->QueryInterface( riid, (void**)ppUnk );
	if( FAILED(hr) || *ppUnk == NULL)
	{
		*ppUnk = NULL;
		return E_NOINTERFACE;
	}
	return S_OK;
}

/*!
	Returns current status information for the server.

	\param ppServerStatus
		Pointer to where the OPCSERVERSTATUS structure pointer should be returned. 
		The structure is allocated by the server.

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory.

	\retval E_INVALIDARG
		An arguments to the function was invalid.

	\retval S_OK
		Operation succeeded.

	\section Comments
		\arg The OPCSERVERSTATUS is described later in this specification. \todo Add ref to OPCSERVERSTATUS
		\arg Client must free the structure as well as the VendorInfo string within the structure.
		\arg Periodic calls to GetStatus would be a good way for the client to determine that the server is still connected and available.
*/
STDMETHODIMP OPCServer::GetStatus( /* [out] */ OPCSERVERSTATUS **ppServerStatus )
{
	if(ppServerStatus == NULL)
		return E_INVALIDARG;

	boost::mutex::scoped_lock guard( scopeGuard );
	OPCSERVERSTATUS *stat = os::win32::com::allocMemory<OPCSERVERSTATUS>();
	memcpy( stat, &serverStatus, sizeof(OPCSERVERSTATUS) );
	CoFileTimeNow(&( stat->ftCurrentTime));

	*ppServerStatus = stat;
	stat->szVendorInfo = os::win32::com::allocMemory<WCHAR>( (wcslen(serverStatus.szVendorInfo)+1) * sizeof(WCHAR) );
	memcpy( stat->szVendorInfo, serverStatus.szVendorInfo, (wcslen( serverStatus.szVendorInfo) + 1) * sizeof(WCHAR) );
	stat->dwGroupCount = (DWORD) group_manager.getGroupCount();
	return S_OK;
}

/*!
	Deletes the Group

	\param hServerGroup
		Handle for the group to be removed.

	\param bForce
		Forces deletion of the group even if references are outstanding.

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory.

	\retval E_INVALIDARG
		An argument to the function was invalid.

	\retval OPC_S_INUSE
		Was not be removed because reference exist. 
		Group will be marked to deleted and will be removed automatically 
		by the server when all reference to this object are released.

	\section Comments
		\arg
		A group is not deleted when all the client interfaces are released, since the server itself maintains a reference to the group. 
		The client may still call GetGroupByName after all the interfaces have been released. 
		RemoveGroup() causes the server to release it's 'last' reference to the group, 
		which results in the group being truly deleted.

		\arg
		In general, a well behaved client will call this function only after releasing all interfaces.

		\arg
		If interfaces still exist, Remove group will mark the group as 'deleted'. 
		Any further calls to this group via these interfaces will return E_FAIL. 
		When all the interfaces are released, the group will actually be deleted. 
		If bForce is TRUE then the group is deleted unconditionally even if references (interfaces) still exist. 
		Subsequent use of such interfaces will result in an access violation.
	
		\arg
		This function should not be called for Public Groups.
*/
STDMETHODIMP OPCServer::RemoveGroup( /* [in] */ OPCHANDLE hServerGroup, /* [in] */ BOOL bForce )
{
	boost::mutex::scoped_lock guard( scopeGuard );
	try
	{
		request_manager.removeGroupFromRequest( hServerGroup );
		if( ! group_manager.removeGroup( hServerGroup ) && ! bForce )
		{
			return OPC_S_INUSE;
		}
	}
	catch( GroupManager::NotExistGroup& )
	{
		return E_FAIL;
	}
	return S_OK;
}

/*!
	Create various enumerators for the groups provided by Server.

	\param dwScope
		Indicates the class of groups to be enumerated
		OPC_ENUM_PRIVATE_CONNECTIONS or
		OPC_ENUM_PRIVATE enumerates all of the private groups created by the client 
		OPC_ENUM_PUBLIC_CONNECTIONS or
		OPC_ENUM_PUBLIC enumerates all of the public groups available in the server 
		OPC_ENUM_ALL_CONNECTIONS or 
		OPC_ENUM_ALL enumerates all private groups and all public groups

	\param riid
		The interface requested. This must be IID_IEnumUnknown or IID_IEnumString.

	\param ppUnk
		Where to return the interface. 
		NULL is returned for any HRESULT other than S_OK or S_FALSE.

	\retval E_FAIL
		The operation failed.

	\retval E_OUTOFMEMORY
		Not enough memory.

	\retval E_INVALIDARG
		An argument to the function  was invalid.

	\retval S_OK
		The operation succeeded.

	\retval S_FALSE
		There is nothing to enumerate (there are no groups which satisfy the request). 
		However an empty Enumerator is still returned and must be released. 
		\b Note: In previous versions of the spec there has been some ambiguity about the behavior in the case of S_FALSE. 
		For this reason, it is recommended that when S_FALSE is returned by the server, 
		clients test the returned interface pointer for NULL prior to calling Release on it.

	\retval E_NOINTERFACE
		The interface (riid) asked for is not supported by the server.

	\section Comments
		\arg
		Connected means an interface pointer exists.

		\arg
		Servers which do not support public groups will simply behave as if they had no public groups. 
		That is they will NOT return E_INVALIDARG if the scope includes public groups.

		\arg
		IEnumUnknown creates an additional interface pointer to each group in the enumeration 
		(even if the client already has a connection to the group). 
		If the server has a large number of public groups available then this may involve considerable overhead 
		as well as requiring additional cleanup by the client. 
		In general, enumerating groups by name will be much faster.

		\arg
		In the case of IEnumUnknown (per the COM specification) the client must also release 
		all of the returned IUnknown pointers when he is done with them.
*/
STDMETHODIMP OPCServer::CreateGroupEnumerator(
	/* [in] */ OPCENUMSCOPE dwScope,
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ LPUNKNOWN *ppUnk )
{
	boost::mutex::scoped_lock guard( scopeGuard );
	if( riid == IID_IEnumUnknown )
	{
		std::vector< GroupElem > unkn;
		if( dwScope != OPC_ENUM_PUBLIC && dwScope != OPC_ENUM_PUBLIC_CONNECTIONS )
		{
			try
			{
				unkn = group_manager.getGroupEnum();
			}
			catch( GroupManager::NotExistGroup& )
			{
			}
		}

		EnumGroup *enumGroup;
		if( unkn.size() )
		{
			enumGroup = new EnumGroup( unkn );
		}
		else
		{
			enumGroup = new EnumGroup();
		}
		HRESULT result = enumGroup->QueryInterface( riid, (void**)ppUnk );
		if( FAILED( result ) )
			delete enumGroup;
		return unkn.size() ? S_OK : S_FALSE;
	}
	if( riid == IID_IEnumString )
	{
		std::vector< String > nameList;
		if( dwScope != OPC_ENUM_PUBLIC && dwScope != OPC_ENUM_PUBLIC_CONNECTIONS )
		{
			try
			{
				nameList = group_manager.getNamesEnum();
			}
			catch( GroupManager::NotExistGroup& )
			{
			}
		}

		EnumString *enumString = new EnumString();
		if( nameList.size() )
		{
			enumString->init( nameList );
		}
		HRESULT result = enumString->QueryInterface( riid, (void**)ppUnk );
		if( FAILED( result ) )
			delete enumString;
		return nameList.size() ? S_OK : S_FALSE;
	}
	return E_NOINTERFACE;
}

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
