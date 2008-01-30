#ifndef frl_opc_sync_io_h_
#define frl_opc_sync_io_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"

namespace frl
{
	namespace opc
	{
		template< class T >
		class SyncIO : public IOPCSyncIO
		{
		public:
			HRESULT STDMETHODCALLTYPE Read( 
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

				*ppItemValues = util::allocMemory< OPCITEMSTATE >( dwCount );
				if( ppItemValues == NULL )
					return E_OUTOFMEMORY;
				util::zeroMemory< OPCITEMSTATE >( *ppItemValues, dwCount );

				*ppErrors =  util::allocMemory< HRESULT >( dwCount );
				if( ppErrors == NULL )
				{
					util::freeMemory( ppItemValues );
					return E_OUTOFMEMORY;
				}
				util::zeroMemory< HRESULT >( *ppErrors, dwCount );

				HRESULT result = S_OK;

				lock::Mutex::ScopeGuard guard( pT->groupGuard );
				for( DWORD i = 0; i < dwCount; i++ )
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
					catch( frl::opc::address_space::NotExistTag &ex )
					{
						ex.~NotExistTag();
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
						(*ppItemValues)[i].wQuality = OPC_QUALITY_GOOD;

					(*ppItemValues)[i].hClient = (*it).second->getClientHandle();
					(*ppItemValues)[i].ftTimeStamp = (*it).second->getTimeStamp();
				}

				return result;
			};

			HRESULT STDMETHODCALLTYPE Write( 
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

				*ppErrors =  util::allocMemory< HRESULT >( dwCount );
				if( ppErrors == NULL )
					return E_OUTOFMEMORY;
				util::zeroMemory< HRESULT >( *ppErrors );

				HRESULT result = S_OK;
				lock::Mutex::ScopeGuard guard( pT->groupGuard );
				for( DWORD i = 0; i < dwCount; i++ )
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
					{
						result = S_FALSE;
						continue;
					}
				}
				return result;
			};
		};
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_sync_io_h_ */
