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

				if (phServer == NULL || ppItemValues == NULL || ppErrors == NULL)
					return E_INVALIDARG;
				
				if (dwSource != OPC_DS_CACHE && dwSource != OPC_DS_DEVICE)
					return E_INVALIDARG;

				*ppErrors = NULL;

				if (dwCount == 0)
					return E_INVALIDARG;

				*ppItemValues = util::allocMemory< OPCITEMSTATE >( dwCount );
				if( ppItemValues == NULL )
					return E_OUTOFMEMORY;
				util::zeroMemory< OPCITEMSTATE >( *ppItemValues );

				*ppErrors =  util::allocMemory< HRESULT >( dwCount );
				if( ppErrors == NULL )
					return E_OUTOFMEMORY;
				util::zeroMemory< HRESULT >( *ppErrors );

				HRESULT result = S_OK;

				for( DWORD i = 0; i < dwCount; i++ )
				{
					std::map< OPCHANDLE, GroupItem* >::iterator it = pT->itemList.find( phServer[i] );
					if( it == pT->itemList.end() )
					{
						result = S_FALSE;
						(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
						continue;
					}

					if ( !pT->actived )
					{
						(*ppItemValues)[i].wQuality = OPC_QUALITY_OUT_OF_SERVICE;
						result = S_FALSE;
						(*ppErrors)[i] = E_FAIL;
						continue;
					}

					(*ppErrors)[i] = (*it).second->readValue( (*ppItemValues)[i].vDataValue );

					if( FAILED( (*ppErrors)[i] ) )
					{
						result = S_FALSE;
						continue;
					}
					
					(*ppItemValues)[i].hClient = (*it).second->getClientHandle();
					(*ppItemValues)[i].wQuality = OPC_QUALITY_GOOD;
					//(*ppItemValues)[i].ftTimeStamp = value.m_Time;
					//VariantInit( &(*ppItemValues)[i].vDataValue );
					//VariantCopy( &(*ppItemValues)[i].vDataValue, &value.m_Value );
					//(*ppErrors)[i] = S_OK;
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

				return E_NOTIMPL;
			};
		};
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_sync_io_h_ */
