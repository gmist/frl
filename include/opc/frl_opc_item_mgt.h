#ifndef frl_opc_item_mgt_h_
#define frl_opc_item_mgt_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "frl_lock.h"
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "opc/frl_opc_enum_item_attributes.h"
#include "opc/address_space/frl_opc_address_space.h"

namespace frl
{
	namespace opc
	{
		template< class T >
		class ItemMgt : public IOPCItemMgt
		{
		public:
			HRESULT STDMETHODCALLTYPE AddItems( 
				/* [in] */ DWORD dwCount,
				/* [size_is][in] */ OPCITEMDEF *pItemArray,
				/* [size_is][size_is][out] */ OPCITEMRESULT **ppAddResults,
				/* [size_is][size_is][out] */ HRESULT **ppErrors)
			{
				T* pT = static_cast<T*> (this);
				if( pT->deleted )
					return E_FAIL;
				
				if(pItemArray == NULL || ppAddResults == NULL || ppErrors == NULL)
					return E_INVALIDARG;

				if( dwCount == 0 ) 
					return E_INVALIDARG;

				*ppAddResults = util::allocMemory<OPCITEMRESULT>( dwCount );
				if( *ppAddResults == NULL )
					return E_OUTOFMEMORY;
				util::zeroMemory<OPCITEMRESULT>( *ppAddResults, dwCount );

				*ppErrors = util::allocMemory<HRESULT>( dwCount );
				if( *ppErrors == NULL ) 
				{ 
					CoTaskMemFree(*ppAddResults);
					return E_OUTOFMEMORY;
				}
				util::zeroMemory<HRESULT>( *ppErrors, dwCount );

				HRESULT res = S_OK;
				lock::Mutex::ScopeGuard guard( pT->groupGuard );
				for (DWORD ii = 0; ii < dwCount; ii++)
				{
					if (pItemArray[ii].szItemID == NULL || wcslen(pItemArray[ii].szItemID) == 0)
					{
						(*ppErrors)[ii] = OPC_E_INVALIDITEMID;
						res = S_FALSE;
						continue;
					}

					if( ! opcAddressSpace.isExistLeaf( pItemArray[ii].szItemID ) )
					{
						(*ppErrors)[ii] = OPC_E_UNKNOWNITEMID;
						res = S_FALSE;
						continue;
					}

					if( ! util::isValidType( pItemArray[ii].vtRequestedDataType ) )
					{
						(*ppErrors)[ii] = OPC_E_BADTYPE;
						res = S_FALSE;
						continue;
					}

					if( pT->itemList.find( pItemArray[ii].hClient) != pT->itemList.end() )
					{
						(*ppErrors)[ii] = E_FAIL;
						res = S_FALSE;
						continue;
					}

					GroupItem *item = new GroupItem();
					OPCHANDLE serverHandle = util::getUniqueServerHandle();
					item->Init( serverHandle, pItemArray[ii] );
					(*ppAddResults)[ii].hServer = serverHandle;

					address_space::Tag *tag = opcAddressSpace.getTag( pItemArray[ii].szItemID );
					(*ppAddResults)[ii].vtCanonicalDataType = tag->getCanonicalDataType();
					(*ppAddResults)[ii].dwAccessRights = tag->getAccessRights();
					(*ppAddResults)[ii].dwBlobSize = 0;
					(*ppAddResults)[ii].pBlob = NULL;
					pT->itemList.insert( std::pair< OPCHANDLE, GroupItem* > ( serverHandle, item ));
				}
				return res;
			}

			HRESULT STDMETHODCALLTYPE ValidateItems( 
				/* [in] */ DWORD dwCount,
				/* [size_is][in] */ OPCITEMDEF *pItemArray,
				/* [in] */ BOOL bBlobUpdate,
				/* [size_is][size_is][out] */ OPCITEMRESULT **ppValidationResults,
				/* [size_is][size_is][out] */ HRESULT **ppErrors)
			{
				T* pT = static_cast<T*> (this);
				if( pT->deleted )
					return E_FAIL;
				if (pItemArray == NULL || ppValidationResults == NULL || ppErrors == NULL )
					return E_INVALIDARG;
				*ppValidationResults = NULL;
				*ppErrors = NULL;

				if( dwCount == 0 )
					return E_INVALIDARG;
				
				*ppValidationResults = util::allocMemory< OPCITEMRESULT >( dwCount );
				if( *ppValidationResults == NULL )
					return E_OUTOFMEMORY;
				util::zeroMemory< OPCITEMRESULT >( *ppValidationResults, dwCount );

				*ppErrors = util::allocMemory< HRESULT >( dwCount );
				if( *ppErrors == NULL )
				{
					CoTaskMemFree( *ppValidationResults );
					return E_OUTOFMEMORY;
				}
				util::zeroMemory< HRESULT >( *ppErrors, dwCount );

				lock::Mutex::ScopeGuard guard( pT->groupGuard );
				HRESULT res = S_OK;
				/// check exist of the specified tags
				for( DWORD i=0;i<dwCount; ++i) 
				{
					if( ! opcAddressSpace.isExistLeaf( pItemArray[i].szItemID ) )
					{
						(*ppErrors)[i] = OPC_E_UNKNOWNITEMID;
						res = S_FALSE;
						continue;
					}

					// validate data type.
					if (! util::isValidType(pItemArray[i].vtRequestedDataType) )
					{
						(*ppErrors)[i] = OPC_E_BADTYPE;
						res = S_FALSE;
						continue;
					}

					address_space::Tag *item = opcAddressSpace.getTag( pItemArray[i].szItemID );					
					ppValidationResults[0][i].vtCanonicalDataType = item->getCanonicalDataType();
					ppValidationResults[0][i].dwAccessRights = item->getAccessRights();
					ppValidationResults[0][i].dwBlobSize = 0;
					ppValidationResults[0][i].pBlob = NULL;
					ppValidationResults[0][i].hServer = 	util::getUniqueServerHandle();
				}
				return res;
			}

			HRESULT STDMETHODCALLTYPE RemoveItems( 
				/* [in] */ DWORD dwCount,
				/* [size_is][in] */ OPCHANDLE *phServer,
				/* [size_is][size_is][out] */ HRESULT **ppErrors)
			{
				T* pT = static_cast<T*> (this);
				if( pT->deleted )
					return E_FAIL;
				if( phServer == NULL || ppErrors == NULL )
					return E_INVALIDARG;
				
				*ppErrors = NULL;
				if( dwCount == 0 )
					return E_INVALIDARG;

				*ppErrors = util::allocMemory<HRESULT>( dwCount );
				if( *ppErrors == NULL )
					return E_OUTOFMEMORY;
				util::zeroMemory<HRESULT>( *ppErrors, dwCount );
				HRESULT res = S_OK;
				
				std::map< OPCHANDLE, GroupItem* >::iterator it;
				lock::Mutex::ScopeGuard guard( pT->groupGuard );
				for( DWORD i=0; i<dwCount; i++)
				{
					it = pT->itemList.find( phServer[i] );
					if( it == pT->itemList.end() ) 
					{
						(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
						res = S_FALSE;
					}
					else 
					{
						pT->itemList.erase( it );
					}
				}
				return res;
			}

			HRESULT STDMETHODCALLTYPE SetActiveState( 
				/* [in] */ DWORD dwCount,
				/* [size_is][in] */ OPCHANDLE *phServer,
				/* [in] */ BOOL bActive,
				/* [size_is][size_is][out] */ HRESULT **ppErrors)
			{
				T* pT = static_cast<T*> (this);
				if( pT->deleted )
					return E_FAIL;
				if (phServer == NULL || ppErrors == NULL )
					return E_INVALIDARG;

				*ppErrors = NULL;
				if( dwCount == 0 )
					return E_INVALIDARG;

				*ppErrors = util::allocMemory< HRESULT >( dwCount );
				if( *ppErrors == NULL ) 
					return E_OUTOFMEMORY;
				util::zeroMemory<HRESULT>( *ppErrors, dwCount );

				HRESULT res = S_OK;
				std::map< OPCHANDLE, GroupItem* >::iterator it;
				lock::Mutex::ScopeGuard guard( pT->groupGuard );
				for (DWORD ii = 0; ii < dwCount; ii++)
				{
					it = pT->itemList.find( phServer[ii] );
					if ( it == pT->itemList.end() )
					{
						(*ppErrors)[ii] = OPC_E_INVALIDHANDLE;
						res = S_FALSE;
						continue;
					}
					(*it).second->isActived( ( bActive == TRUE ) );
					(*ppErrors)[ii] = S_OK;
				}
				return res;
			}

			HRESULT STDMETHODCALLTYPE SetClientHandles( 
				/* [in] */ DWORD dwCount,
				/* [size_is][in] */ OPCHANDLE *phServer,
				/* [size_is][in] */ OPCHANDLE *phClient,
				/* [size_is][size_is][out] */ HRESULT **ppErrors)
			{
				T* pT = static_cast<T*> (this);
				if( pT->deleted )
					return E_FAIL;
				if ( phServer == NULL || phClient == NULL || ppErrors == NULL )
					return E_INVALIDARG;
				*ppErrors = NULL;

				if( dwCount == 0 )
					return E_INVALIDARG;
				*ppErrors = util::allocMemory< HRESULT >( dwCount );
				if( *ppErrors == NULL )
					return E_OUTOFMEMORY;
				util::zeroMemory< HRESULT >( *ppErrors, dwCount );

				HRESULT res = S_OK;
				std::map< OPCHANDLE, GroupItem* >::iterator it;
				lock::Mutex::ScopeGuard guard( pT->groupGuard );
				for( DWORD i=0; i<dwCount; i++ )
				{
					it = pT->itemList.find( phServer[i] );
					if(it == pT->itemList.end())
					{
						(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
						res = S_FALSE;
					}
					else
						(*it).second->setClientHandle( phClient[i] );
				}
				return res;
			}

			HRESULT STDMETHODCALLTYPE SetDatatypes( 
				/* [in] */ DWORD dwCount,
				/* [size_is][in] */ OPCHANDLE *phServer,
				/* [size_is][in] */ VARTYPE *pRequestedDatatypes,
				/* [size_is][size_is][out] */ HRESULT **ppErrors)
			{
				T* pT = static_cast<T*> (this);
				if( pT->deleted )
					return E_FAIL;
				if( *phServer == NULL || *pRequestedDatatypes == NULL || *ppErrors == NULL )
					return E_INVALIDARG;
				*ppErrors = NULL;
				if( dwCount == 0 )
					return E_INVALIDARG;

				*ppErrors = util::allocMemory< HRESULT >( dwCount );
				if( *ppErrors == NULL )
					return E_OUTOFMEMORY;
				util::zeroMemory< HRESULT >( *ppErrors, dwCount );

				HRESULT res = S_OK;
				std::map< OPCHANDLE, GroupItem* >::iterator it;
				lock::Mutex::ScopeGuard guard( pT->groupGuard );
				for( DWORD i=0; i<dwCount; i++ )
				{
					it = pT->itemList.find( phServer[i] );
					if(it == pT->itemList.end())
					{
						(*ppErrors)[i] = E_FAIL;
						res = S_FALSE;
					}
					else
						(*it).second->setRequestDataType( pRequestedDatatypes[i] );
				}
				return res;
			}

			HRESULT STDMETHODCALLTYPE CreateEnumerator( 
				/* [in] */ REFIID riid,
				/* [iid_is][out] */ LPUNKNOWN *ppUnk)
			{
				T* pT = static_cast<T*> (this);
				if( pT->deleted )
					return E_FAIL;
				lock::Mutex::ScopeGuard guard( pT->groupGuard );
				if (ppUnk == NULL)
					return E_INVALIDARG;

				if ( riid == IID_IEnumOPCItemAttributes)
				{
					EnumOPCItemAttributes *temp = new EnumOPCItemAttributes();
					if (temp == NULL)
						return (E_OUTOFMEMORY);
					std::map< OPCHANDLE, GroupItem* >::iterator it;
					lock::Mutex::ScopeGuard guard( pT->groupGuard );
					for( it = pT->itemList.begin(); it != pT->itemList.end(); ++it )
					{
						OPCHANDLE h = it->first;
						GroupItem* i = (*it).second;
						temp->AddItem ( h, i );
					}
					return temp->QueryInterface(riid,(void**)ppUnk);
				}
				return S_FALSE;
			}
		}; // class ItemMgt
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_item_mgt_h_ */
