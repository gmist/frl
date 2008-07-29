#include "opc/impl/frl_opc_impl_group_state_mgt.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "opc/frl_opc_server.h"
#include "opc/frl_opc_group.h"

namespace frl { namespace opc { namespace impl {

GroupStateMgt::~GroupStateMgt()
{
}

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
							AsyncRequestListElem request( new AsyncRequest( tmp, async_request::UPDATE, handles) );
							doAsyncRefresh( request );
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

STDMETHODIMP GroupStateMgt::CloneGroup( /* [string][in] */ LPCWSTR szName, /* [in] */ REFIID riid, /* [iid_is][out] */ LPUNKNOWN *ppUnk )
{
	if( deleted )
		return E_FAIL;

	if( ppUnk == NULL )
		return E_INVALIDARG;

	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		String new_name = szName;
	#else
		String new_name = wstring2string( szName );
	#endif
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
