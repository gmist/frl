#ifndef frl_opc_sync_io_h_
#define frl_opc_sync_io_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"

using namespace frl::opc::address_space;

namespace frl
{
namespace opc
{
template< class T >
class SyncIO : public IOPCSyncIO2
{
public:
	virtual HRESULT STDMETHODCALLTYPE Read( 
		/* [in] */ OPCDATASOURCE dwSource,
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][size_is][out] */ OPCITEMSTATE **ppItemValues,
		/* [size_is][size_is][out] */ HRESULT **ppErrors)
	{
		T* pT = static_cast<T*> (this);
		if( pT->deleted )
			return E_FAIL;

		if( phServer == NULL || ppItemValues == NULL || ppErrors == NULL )
			return E_INVALIDARG;
		
		if( dwSource != OPC_DS_CACHE && dwSource != OPC_DS_DEVICE )
			return E_INVALIDARG;

		*ppErrors = NULL;

		if (dwCount == 0)
			return E_INVALIDARG;

		*ppItemValues = os::win32::com::allocMemory< OPCITEMSTATE >( dwCount );
		if( ppItemValues == NULL )
			return E_OUTOFMEMORY;
		os::win32::com::zeroMemory< OPCITEMSTATE >( *ppItemValues, dwCount );

		*ppErrors =  os::win32::com::allocMemory< HRESULT >( dwCount );
		if( ppErrors == NULL )
		{
			os::win32::com::freeMemory( ppItemValues );
			return E_OUTOFMEMORY;
		}
		os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

		HRESULT result = S_OK;

		lock::ScopeGuard guard( pT->groupGuard );
		for( DWORD i = 0; i < dwCount; ++i )
		{
			std::map< OPCHANDLE, GroupItem* >::iterator it = pT->itemList.find( phServer[i] );
			if( it == pT->itemList.end() )
			{
				result = S_FALSE;
				(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
				continue;
			}

			if( ! ( (*it).second->getAccessRights() & OPC_READABLE ) )
			{
				result = S_FALSE;
				(*ppErrors)[i] = OPC_E_BADRIGHTS;
				continue;
			}

			try
			{
				if( dwSource == OPC_DS_CACHE )
					( *ppErrors )[i] = ( (*it).second->getCachedValue().copyTo( (*ppItemValues)[i].vDataValue ) );
				else
					( *ppErrors )[i] = ( (*it).second->readValue() ).copyTo( (*ppItemValues)[i].vDataValue );
			}
			catch( Tag::NotExistTag& )
			{
				( *ppErrors )[i] = OPC_E_INVALIDHANDLE;
			}

			if( FAILED( ( *ppErrors)[i] ) )
			{
				result = S_FALSE;
				continue;
			}

			if ( ( ! (*it).second->isActived() || ! pT->actived ) && dwSource == OPC_DS_CACHE )
				(*ppItemValues)[i].wQuality = OPC_QUALITY_OUT_OF_SERVICE;
			else
				(*ppItemValues)[i].wQuality = (*it).second->getQuality();

			(*ppItemValues)[i].hClient = (*it).second->getClientHandle();
			(*ppItemValues)[i].ftTimeStamp = (*it).second->getTimeStamp();
		}

		return result;
	};

	virtual HRESULT STDMETHODCALLTYPE Write( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ VARIANT *pItemValues,
		/* [size_is][size_is][out] */ HRESULT **ppErrors)
	{
		T* pT = static_cast<T*> (this);
		if( pT->deleted )
			return E_FAIL;

		if (phServer == NULL || pItemValues == NULL || ppErrors == NULL)
			return E_INVALIDARG;

		*ppErrors = NULL;

		if (dwCount == 0)
			return E_INVALIDARG;

		*ppErrors =  os::win32::com::allocMemory< HRESULT >( dwCount );
		if( ppErrors == NULL )
			return E_OUTOFMEMORY;
		os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

		HRESULT result = S_OK;
		lock::ScopeGuard guard( pT->groupGuard );
		for( DWORD i = 0; i < dwCount; ++i )
		{
			std::map< OPCHANDLE, GroupItem* >::iterator it = pT->itemList.find( phServer[i] );
			if( it == pT->itemList.end() )
			{
				result = S_FALSE;
				(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
				continue;
			}

			if( ! ( (*it).second->getAccessRights() & OPC_WRITEABLE ) )
			{
				result = S_FALSE;
				(*ppErrors)[i] = OPC_E_BADRIGHTS;
				continue;
			}

			if( pItemValues[i].vt == VT_EMPTY )
			{
				result = S_FALSE;
				(*ppErrors)[i] = OPC_E_BADTYPE;
				continue;
			}
			
			(*ppErrors)[i] = (*it).second->writeValue( pItemValues[i] );

			if( FAILED( (*ppErrors)[i] ) )
				result = S_FALSE;
		}
		return result;
	}

	virtual HRESULT STDMETHODCALLTYPE ReadMaxAge( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ DWORD *pdwMaxAge,
		/* [size_is][size_is][out] */ VARIANT **ppvValues,
		/* [size_is][size_is][out] */ WORD **ppwQualities,
		/* [size_is][size_is][out] */ FILETIME **ppftTimeStamps,
		/* [size_is][size_is][out] */ HRESULT **ppErrors)
	{
		T* pT = static_cast<T*> (this);
		if( pT->deleted )
			return E_FAIL;
		if ( phServer == NULL || 
			pdwMaxAge == NULL || 
			ppvValues == NULL || 
			ppwQualities == NULL || 
			ppftTimeStamps == NULL ||
			ppErrors == NULL )
		{
			return E_INVALIDARG;
		}
		*ppErrors = NULL;
		if( dwCount == 0 )
			return E_INVALIDARG;

		*ppvValues = os::win32::com::allocMemory< VARIANT >( dwCount );
		*ppwQualities = os::win32::com::allocMemory< WORD >( dwCount );
		*ppftTimeStamps = os::win32::com::allocMemory< FILETIME >( dwCount );
		*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );

		os::win32::com::zeroMemory< VARIANT >( *ppvValues, dwCount );
		os::win32::com::zeroMemory< WORD >( *ppwQualities, dwCount );
		os::win32::com::zeroMemory< FILETIME >( *ppftTimeStamps, dwCount );
		os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );
		
		HRESULT result = S_OK;

		lock::ScopeGuard guard( pT->groupGuard );
		for( DWORD i = 0; i < dwCount; ++i )
		{
			std::map< OPCHANDLE, GroupItem* >::iterator it = pT->itemList.find( phServer[i] );
			if( it == pT->itemList.end() )
			{
				result = S_FALSE;
				(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
				continue;
			}
			if( ! ( (*it).second->getAccessRights() & OPC_READABLE ) )
			{
				result = S_FALSE;
				(*ppErrors)[i] = OPC_E_BADRIGHTS;
				continue;
			}
			try
			{
				( *ppErrors )[i] = ( (*it).second->readValue() ).copyTo( (*ppvValues)[i] );
			}
			catch( Tag::NotExistTag& )
			{
				( *ppErrors )[i] = OPC_E_INVALIDHANDLE;
			}

			if( FAILED( ( *ppErrors)[i] ) )
			{
				result = S_FALSE;
				continue;
			}

			(*ppwQualities)[i] = (*it).second->getQuality();
			(*ppftTimeStamps)[i] = (*it).second->getTimeStamp();
		}
		return result;
	}

	virtual HRESULT STDMETHODCALLTYPE WriteVQT( 
		/* [in] */ DWORD dwCount,
		/* [size_is][in] */ OPCHANDLE *phServer,
		/* [size_is][in] */ OPCITEMVQT *pItemVQT,
		/* [size_is][size_is][out] */ HRESULT **ppErrors)
	{
		T* pT = static_cast< T* >( this );
		if( pT->deleted )
			return E_FAIL;

		if( phServer == NULL || pItemVQT == NULL || ppErrors == NULL )
			return E_INVALIDARG;

		*ppErrors = NULL;

		if( dwCount == 0 )
			return E_INVALIDARG;

		// write items.
		*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
		os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );
		
		HRESULT res = S_OK;
		for( DWORD i = 0; i < dwCount; ++i )
		{
			if( pItemVQT[i].vDataValue.vt == VT_EMPTY )
			{
				res = S_FALSE;
				(*ppErrors)[i] = OPC_E_BADTYPE;
				continue;	
			}

			std::map< OPCHANDLE, GroupItem* >::iterator it = pT->itemList.find( phServer[i] );
			if( it == pT->itemList.end() )
			{
				res = S_FALSE;
				(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
				continue;
			}

			if( ! ( (*it).second->getAccessRights() & OPC_WRITEABLE ) )
			{
				res = S_FALSE;
				(*ppErrors)[i] = OPC_E_BADRIGHTS;
				continue;
			}

			(*ppErrors)[i] = (*it).second->writeValue( pItemVQT[i].vDataValue );

			if( FAILED( (*ppErrors)[i] ) )
			{
				res = S_FALSE;
				continue;
			}

			if( pItemVQT[i].bQualitySpecified )
			{
				(*it).second->setQuality( pItemVQT[i].wQuality );
			}

			if( pItemVQT[i].bTimeStampSpecified )
			{
				(*it).second->setTimeStamp( pItemVQT[i].ftTimeStamp );
			}
		}
		return res;
	}
};
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif /* frl_opc_sync_io_h_ */
