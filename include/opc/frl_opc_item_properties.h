#ifndef frl_opc_item_properties_h_
#define frl_opc_item_properties_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "../dependency/vendors/opc_foundation/opcerror.h"

namespace frl
{
	namespace opc
	{
		template< class T>
		class ItemProperties : public IOPCItemProperties
		{
		public:
			virtual HRESULT STDMETHODCALLTYPE QueryAvailableProperties( 
				/* [in] */ LPWSTR szItemID,
				/* [out] */ DWORD *pdwCount,
				/* [size_is][size_is][out] */ DWORD **ppPropertyIDs,
				/* [size_is][size_is][out] */ LPWSTR **ppDescriptions,
				/* [size_is][size_is][out] */ VARTYPE **ppvtDataTypes)
			{
				if (
					szItemID == NULL ||
					pdwCount == NULL ||
					ppPropertyIDs == NULL ||
					ppDescriptions == NULL ||
					ppvtDataTypes == NULL
					)
				{
					return E_INVALIDARG;
				}

				*pdwCount = 0;
				*ppPropertyIDs = NULL;
				*ppDescriptions = NULL;
				*ppvtDataTypes = NULL;

				CacheItem item;
				if( ! flatDataCache.getItem( szItemID, item ) )
					return OPC_E_INVALIDITEMID;
				*pdwCount = 1;
				*ppPropertyIDs  = (DWORD*)CoTaskMemAlloc( *pdwCount * sizeof(DWORD));
				*ppDescriptions = (LPWSTR*)CoTaskMemAlloc( *pdwCount * sizeof(LPWSTR));
				*ppvtDataTypes  = (VARTYPE*)CoTaskMemAlloc( *pdwCount * sizeof(VARTYPE));
				if( ppPropertyIDs == NULL || ppDescriptions ==NULL || ppvtDataTypes == NULL)
					return E_OUTOFMEMORY;

				*ppPropertyIDs[0] = item.getServerHandle();

				WCHAR* pCopy = NULL;
				size_t size = ((item.getID().length()+1) * sizeof(WCHAR));
				pCopy = new WCHAR[size];
				wcsncpy_s(pCopy, size, item.getID().c_str(), item.getID().length() );
				*ppDescriptions[0] = pCopy;
				*ppvtDataTypes[0] = item.getCanonicalDataType();
	
				return S_OK;
			}

			virtual HRESULT STDMETHODCALLTYPE GetItemProperties( 
				/* [in] */ LPWSTR szItemID,
				/* [in] */ DWORD dwCount,
				/* [size_is][in] */ DWORD *pdwPropertyIDs,
				/* [size_is][size_is][out] */ VARIANT **ppvData,
				/* [size_is][size_is][out] */ HRESULT **ppErrors)
			{
				if (	szItemID == NULL ||
						ppvData == NULL ||
						ppErrors  == NULL
					)
				{
					return E_INVALIDARG;
				}
				
				*ppvData  = NULL;
				*ppErrors = NULL;

				if( dwCount == 0 )
					return E_INVALIDARG;

				*ppvData = (VARIANT*)CoTaskMemAlloc( dwCount * sizeof(VARIANT) );
				*ppErrors = ( HRESULT*)CoTaskMemAlloc( dwCount * sizeof(HRESULT) );
				if( ppvData == NULL || ppErrors == NULL )
					return E_OUTOFMEMORY;

				CacheItem item;
				if( ! flatDataCache.getItem( szItemID, item ) )
					return OPC_E_INVALIDITEMID;

				HRESULT res = S_OK;
				for( DWORD i = 0; i < dwCount; i++ )
				{
					switch( pdwPropertyIDs[i] )
					{
					case 1:
						{
							VariantInit( &*ppvData[i] );
							V_VT(&*ppvData[i]) = VT_I2;
							V_I2(&*ppvData[i]) = item.getCanonicalDataType();
						}
						break;
					case 2:
						{
							VariantInit( &*ppvData[i] );
							HRESULT r1 = VariantChangeType( &*ppvData[i],&*ppvData[i],0, item.getReguestedDataType() );
							//*ppvData[i] =  // TODO
						}
						break;
					case 3:
						{
							VariantInit( &*ppvData[i] );
							V_VT(&*ppvData[i]) = VT_I2;
							V_I2(&*ppvData[i]); // TODO =
						}
						break;
					case 4:
						{
							VariantInit( &*ppvData[i] );
							V_VT(&*ppvData[i]) = VT_DATE;
							V_DATE(&*ppvData[i]); // TODO =
						}
						break;
					case 5:
						{
							VariantInit( &*ppvData[i] );
							V_VT(&*ppvData[i]) = VT_I4;
							V_I4(&*ppvData[i]) = item.getAccessRights();
						}
						break;
					case 6:
						{
							VariantInit( &*ppvData[i] );
							V_VT(&*ppvData[i]) = VT_R4;
							V_R4(&*ppvData[i]); // TODO =
						}
						break;
					default:
						*ppErrors[i] = OPC_E_INVALID_PID;
						res = S_FALSE;
						continue;
					}
				}
				return res;
			}

			virtual HRESULT STDMETHODCALLTYPE LookupItemIDs( 
				/* [in] */ LPWSTR szItemID,
				/* [in] */ DWORD dwCount,
				/* [size_is][in] */ DWORD *pdwPropertyIDs,
				/* [size_is][size_is][string][out] */ LPWSTR **ppszNewItemIDs,
				/* [size_is][size_is][out] */ HRESULT **ppErrors)
			{
				if (
					szItemID == NULL ||
					ppszNewItemIDs == NULL ||
					ppErrors == NULL
					)
				{
					return E_INVALIDARG;
				}

				*ppszNewItemIDs = NULL;
				*ppErrors = NULL;
				
				if (dwCount == 0)
				{
					return E_INVALIDARG;
				}
				CacheItem item;
				if( ! flatDataCache.getItem( szItemID, item ) )
					return OPC_E_INVALIDITEMID;

				*ppszNewItemIDs = (LPWSTR*)CoTaskMemAlloc( dwCount * sizeof(LPWSTR) );
				*ppErrors = ( HRESULT*)CoTaskMemAlloc( dwCount * sizeof(HRESULT) );
				if( ppszNewItemIDs == NULL || ppErrors == NULL )
					return E_OUTOFMEMORY;
				for( DWORD i = 0; i < dwCount; i++ )
				{
					size_t size = wcslen( szItemID );
					*ppszNewItemIDs[i] = new WCHAR[(size+1) * sizeof(WCHAR)];
					wcsncpy_s( *ppszNewItemIDs[i], (size+1)*sizeof(WCHAR), szItemID, size );
					ppErrors[i] = S_OK;
				}
				return E_NOTIMPL;
			}
		}; // class ItemProperties
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_item_properties_h_ */
