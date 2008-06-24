#ifndef frl_opc_group_state_mgt_h_
#define frl_opc_group_state_mgt_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_server.h"

namespace frl
{
namespace opc
{

template < class T >
class GroupStateMgt : public IOPCGroupStateMgt2
{
public:
	HRESULT STDMETHODCALLTYPE GetState( 
		/* [out] */ DWORD *pUpdateRate,
		/* [out] */ BOOL *pActive,
		/* [string][out] */ LPWSTR *ppName,
		/* [out] */ LONG *pTimeBias,
		/* [out] */ FLOAT *pPercentDeadband,
		/* [out] */ DWORD *pLCID,
		/* [out] */ OPCHANDLE *phClientGroup,
		/* [out] */ OPCHANDLE *phServerGroup)
	{
		T* pT = static_cast<T*> (this);
		if( pT->deleted )
			return E_FAIL;
		
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		*ppName = util::duplicateString( pT->name );
		#else
		*ppName = util::duplicateString( string2wstring( pT->name ) );
		#endif
		*pActive = pT->actived;
		*pUpdateRate = pT->updateRate;
		*phServerGroup = pT->getServerHandle();
		*phClientGroup = pT->clientHandle;
		*pTimeBias = pT->timeBias;
		*pPercentDeadband = pT->deadband;
		*pLCID = pT->localeID;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetState( 
		/* [in][unique] */ DWORD *pRequestedUpdateRate,
		/* [out] */ DWORD *pRevisedUpdateRate,
		/* [in][unique] */ BOOL *pActive,
		/* [in][unique] */ LONG *pTimeBias,
		/* [in][unique] */ FLOAT *pPercentDeadband,
		/* [in][unique] */ DWORD *pLCID,
		/* [in][unique] */ OPCHANDLE *phClientGroup)
	{
		T* pT = static_cast<T*> (this);
		if( pT->deleted )
			return E_FAIL;

		HRESULT hResult = S_OK;
		
		if(pRevisedUpdateRate != NULL)
			*pRevisedUpdateRate = 0;

		// validate deadband
		if( pPercentDeadband != NULL )
		{
			if( *pPercentDeadband < 0 || *pPercentDeadband > 100 )
				return E_INVALIDARG;
			pT->deadband = *pPercentDeadband;
		}

		boost::mutex::scoped_lock guard( pT->groupGuard );

		if( pRequestedUpdateRate != NULL )
		{
			static const int maxUpdateRate = 100;
			DWORD dwUpdateRate = *pRequestedUpdateRate;
			if( dwUpdateRate == 0 || dwUpdateRate%maxUpdateRate != 0 )
			{
				dwUpdateRate = maxUpdateRate*( dwUpdateRate/maxUpdateRate + 1 );
				hResult = OPC_S_UNSUPPORTEDRATE;
			}

			*pRevisedUpdateRate = pT->updateRate = dwUpdateRate;
		}

		if( pTimeBias != NULL )
			pT->timeBias = *pTimeBias;

		if( pLCID != NULL )
			pT->localeID = *pLCID;

		if( phClientGroup != NULL )
			pT->clientHandle = *phClientGroup; 

		if( pActive != NULL )
		{
			Bool oldState = pT->actived;
			if( (*pActive == TRUE ) || (*pActive == VARIANT_TRUE ) )
				pT->actived = True;
			else
				pT->actived = False;

			if( pT->actived )
			{
				if( ! oldState )
				{
					if( pT->itemList.size() )
					{
						if( pT->isConnected( IID_IOPCDataCallback ) )
						{
							std::list< OPCHANDLE > handles;
							GroupItemElemList::iterator end = pT->itemList.end();
							for( GroupItemElemList::iterator it = pT->itemList.begin(); it != end; ++it )
							{
								if( (*it).second->isActived() )
									handles.push_back( (*it).first );
							}
							if( handles.size() )
							{
								GroupElem tmp = GroupElem( pT );
								AsyncRequestListElem request( new AsyncRequest( tmp, async_request::UPDATE, handles) );
								pT->doAsyncRefresh( request );
							}
						}
					}
					pT->renewUpdateRate();
				}
			}
			else
			{
				pT->server->removeGroupFromRequestList( pT->getServerHandle() );
			}
		}
		return hResult;
	}

	HRESULT STDMETHODCALLTYPE SetName( 
		/* [string][in] */ LPCWSTR szName)
	{
		T* pT = static_cast<T*> (this);
		if( pT->deleted )
			return E_FAIL;

		if( szName == NULL || wcslen( szName ) == 0 )
			return E_INVALIDARG;

		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			String name = szName;
		#else
			String name = wstring2string( szName );
		#endif

		HRESULT res = pT->server->setGroupName( pT->name, name );
		if( FAILED( res ) )
			return res;
		pT->name = name;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE CloneGroup( 
		/* [string][in] */ LPCWSTR szName,
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ LPUNKNOWN *ppUnk)
	{
		T* pT = static_cast<T*> (this);
		if( pT->deleted )
			return E_FAIL;

		if( ppUnk == NULL )
			return E_INVALIDARG;

		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			String name = szName;
		#else
			String name = wstring2string( szName );
		#endif
		GroupElem group;
		try
		{
			group = pT->server->cloneGroup( pT->name, name );
		}
		catch( GroupManager::IsExistGroup& )
		{
			return OPC_E_DUPLICATENAME;
		}
		catch( GroupManager::NotExistGroup& )
		{
			return E_INVALIDARG;
		}

		#if( FRL_COMPILER == FRL_COMPILER_MSVC )
		HRESULT result = group->QueryInterface( riid, (void**)ppUnk );
		#else
		HRESULT result = group.get()->QueryInterface( riid, (void**)ppUnk );
		#endif

		if( FAILED( result ) )
		{
			#if( FRL_COMPILER == FRL_COMPILER_MSVC )
			pT->server->RemoveGroup( group->getServerHandle(), FALSE );
			#else
			pT->server->RemoveGroup( group.get()->getServerHandle(), FALSE );
			#endif
		}

		return result;
	}
	
	virtual HRESULT STDMETHODCALLTYPE SetKeepAlive( 
		/* [in] */ DWORD dwKeepAliveTime,
		/* [out] */ DWORD *pdwRevisedKeepAliveTime)
	{
		if( pdwRevisedKeepAliveTime == NULL )
			return E_INVALIDARG;
		
		T* pT = static_cast<T*> (this);
		if( pT->deleted )
			return E_FAIL;

		if( dwKeepAliveTime == 0 )
		{
			pT->keepAlive = *pdwRevisedKeepAliveTime = 0;
			return S_OK;
		}

		static const DWORD maxKeepAliveTime = 100000; // 100 sec

		DWORD tmp = dwKeepAliveTime; // for non blocked change Group::keepAlive
		if( tmp%maxKeepAliveTime != 0 )
		{
			tmp = maxKeepAliveTime * ( tmp / maxKeepAliveTime + 1 );
		}
		pT->keepAlive = *pdwRevisedKeepAliveTime = tmp;
		
		if( tmp != dwKeepAliveTime )
			return OPC_S_UNSUPPORTEDRATE;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetKeepAlive( 
		/* [out] */ DWORD *pdwKeepAliveTime)
	{
		if( pdwKeepAliveTime == NULL )
			return E_INVALIDARG;

		T* pT = static_cast<T*> (this);
		if( pT->deleted )
			return E_FAIL;

		*pdwKeepAliveTime = pT->keepAlive;
		return S_OK;
	}
}; // class GroupStateMgt
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif /* frl_opc_group_state_mgt_h_ */
