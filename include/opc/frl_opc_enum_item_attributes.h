#ifndef frl_opc_enum_item_attributes_h_
#define frl_opc_enum_item_attributes_h_
#include "frl_platform.h"
#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <vector>
#include <atlstr.h>
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_group_item.h"

namespace frl
{
	namespace opc
	{
		class EnumOPCItemAttributes : public IEnumOPCItemAttributes
		{
		private:
			std::vector<OPCITEMATTRIBUTES*> itemList;
			size_t curIndex; //Current element			
			LONG volatile refCount; // Enum Object reference count		
		public:
			EnumOPCItemAttributes()
				:	curIndex( 0 ), refCount( 0 )
			{

			}
			~EnumOPCItemAttributes()
			{
				for( size_t i=0; i< itemList.size(); ++i  )
				{
					if( itemList[i] != NULL ) 
					{
						if( itemList[i]->szItemID != NULL )
						{
							SysFreeString( itemList[i]->szItemID );
							itemList[i]->szItemID = NULL;
						}
						CoTaskMemFree( itemList[i] );
					}
				}
				itemList.erase(itemList.begin(), itemList.end() );
			}

			// the IUnknown Functions
			STDMETHODIMP QueryInterface( REFIID iid, LPVOID* ppInterface )
			{				
				if ( ppInterface == NULL)
				{				
					return (E_INVALIDARG);
				}

				if ( iid == IID_IUnknown )
				{				
					*ppInterface = (IUnknown*) this;
				}
				else if (iid == IID_IEnumOPCItemAttributes)
				{				
					*ppInterface = this;
				}
				else
				{					
					*ppInterface = NULL;
					return (E_NOINTERFACE);
				}

				AddRef();
				return S_OK;
			}

			STDMETHODIMP_(ULONG) AddRef( void)
			{
				return ::InterlockedIncrement( &refCount );
			}

			STDMETHODIMP_(ULONG) Release( void)
			{
				LONG ret = ::InterlockedDecrement( &refCount );
				if( ret == 0 )
					delete this;
				return ret;
			}

			OPCITEMATTRIBUTES* Copy( OPCITEMATTRIBUTES *i )
			{
				if( i == NULL )
					return NULL;

				OPCITEMATTRIBUTES *c = (OPCITEMATTRIBUTES *)CoTaskMemAlloc( sizeof(OPCITEMATTRIBUTES) );
				ZeroMemory( c, sizeof(OPCITEMATTRIBUTES) );
				c->bActive = i->bActive;
				c->dwAccessRights = i->dwAccessRights;
				c->dwBlobSize = i->dwBlobSize;
				c->dwEUType = i->dwEUType;
				c->hClient = i->hClient;
				c->hServer = i->hServer;
				c->pBlob = i->pBlob;
				c->szAccessPath = SysAllocString( i->szAccessPath );
				c->szItemID = SysAllocString( i->szItemID );
				VariantCopy(&c->vEUInfo , &i->vEUInfo );
				c->vtCanonicalDataType = i->vtCanonicalDataType;
				c->vtRequestedDataType = i->vtRequestedDataType;
				return c;
			}

			void AddItem( OPCHANDLE first,  GroupItem* i )
			{
				OPCITEMATTRIBUTES *oi = (OPCITEMATTRIBUTES *)CoTaskMemAlloc( sizeof(OPCITEMATTRIBUTES) );
				ZeroMemory( oi, sizeof(OPCITEMATTRIBUTES) );
				oi->bActive = i->isActived();
				oi->hClient = i->getClientHandle();
				oi->hServer = first;
				
				if( i->getItemID().empty() )
				{
					oi->szItemID = SysAllocString(L""); // NULL
				}
				else
				{
					LPWSTR itemID = new WCHAR[ i->getItemID().size() ];
					wcscpy_s( itemID, i->getItemID().size(), i->getItemID().c_str() );				
					oi->szItemID = itemID;
				}

				if( i->getAccessPath().empty() )
				{
					oi->szAccessPath = SysAllocString(L""); //NULL;
				}
				else
				{
					LPWSTR accessPath = new WCHAR[ i->getAccessPath().size() ];
					wcscpy_s( accessPath, i->getAccessPath().size(), i->getAccessPath().c_str() );				
					oi->szItemID = accessPath;
				}				
				
				CacheItem caheItem;
				flatDataCache.getItem( i->getItemID(), caheItem );				
				oi->dwAccessRights = caheItem.getAccessRights();
				oi->dwBlobSize = 0;
				oi->pBlob = NULL;
				oi->vtCanonicalDataType = caheItem.getCanonicalDataType();
				oi->vtRequestedDataType = i->getReguestDataType();				
				itemList.push_back( oi );
			}

			// the IEnum Functions
			STDMETHODIMP Next( ULONG celt, OPCITEMATTRIBUTES **ppItemArray, ULONG *pceltFetched )
			{
				if( ppItemArray == NULL || celt == 0 ) 
					return E_INVALIDARG;

				if( pceltFetched != NULL ) 
					*pceltFetched = 0;				

				if( ppItemArray == NULL || ( celt != 1 && pceltFetched == NULL) )
					return E_POINTER;

				size_t i;
				for( i = 0; i < celt; ++i )
				{
					if( curIndex >= itemList.size() ) break;
					ppItemArray[i] = Copy( itemList[curIndex] );
					if( pceltFetched != NULL )
						(*pceltFetched)++;
					curIndex++;
				}

				if( i == celt )
					return S_OK;
				else
					return S_FALSE;
			}

			STDMETHODIMP Skip ( ULONG celt )
			{
				curIndex += celt;
				if( itemList.size() <= curIndex )
					return S_FALSE;
				return S_OK;
			}

			STDMETHODIMP Reset( void )
			{
				curIndex = 0;
				return S_OK;
			}

			STDMETHODIMP Clone( IEnumOPCItemAttributes **ppEnum )
			{
				// check for invalid arguments.
				if (ppEnum == NULL)
				{
					return E_INVALIDARG;
				}

				// allocate enumerator.
				EnumOPCItemAttributes* pEnum = new EnumOPCItemAttributes();
				if( pEnum == NULL )
					return E_OUTOFMEMORY;

				for( size_t i = 0; i < itemList.size(); i++ )
				{
					OPCITEMATTRIBUTES *pItem = itemList[i];
					OPCITEMATTRIBUTES *newItem = new OPCITEMATTRIBUTES();
					if( newItem != NULL )
					{
						newItem = Copy( pItem );
						pEnum->itemList.push_back( newItem );
					}
				}
				pEnum->curIndex = curIndex;
				*ppEnum = pEnum;
				return S_OK;
			}
		};
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_enum_item_attributes_h_ */
