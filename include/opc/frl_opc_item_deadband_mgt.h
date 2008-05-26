#ifndef frl_opc_item_deadband_mgt_h_
#define frl_opc_item_deadband_mgt_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"

namespace frl
{
namespace opc
{
template< class T >
class ItemDeadBandMgt : public IOPCItemDeadbandMgt
{
public:
virtual HRESULT STDMETHODCALLTYPE SetItemDeadband( 
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][in] */ FLOAT *pPercentDeadband,
	/* [size_is][size_is][out] */ HRESULT **ppErrors)
	{
		T* pT = static_cast<T*> (this);
		if( pT->deleted )
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
		lock::ScopeGuard guard( pT->groupGuard );
		for( DWORD i = 0; i < dwCount; ++i )
		{
			GroupItemElemList::iterator it = pT->itemList.find( phServer[i] );
			if( it == pT->itemList.end() )
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

virtual HRESULT STDMETHODCALLTYPE GetItemDeadband( 
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][size_is][out] */ FLOAT **ppPercentDeadband,
	/* [size_is][size_is][out] */ HRESULT **ppErrors)
	{
		T* pT = static_cast<T*> (this);
		if( pT->deleted )
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
		lock::ScopeGuard guard( pT->groupGuard );
		Float tmpDeadBand;
		for( DWORD i = 0; i < dwCount; ++i )
		{
			GroupItemElemList::iterator it = pT->itemList.find( phServer[i] );
			if( it == pT->itemList.end() )
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

virtual HRESULT STDMETHODCALLTYPE ClearItemDeadband( 
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][size_is][out] */ HRESULT **ppErrors)
	{
		T* pT = static_cast<T*> (this);
		if( pT->deleted )
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
		lock::ScopeGuard guard( pT->groupGuard );
		Float tmpDeadBand;
		for( DWORD i = 0; i < dwCount; ++i )
		{
			GroupItemElemList::iterator it = pT->itemList.find( phServer[i] );
			if( it == pT->itemList.end() )
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
};
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_item_deadband_mgt_h_
