#ifndef frl_opc_item_properties_h_
#define frl_opc_item_properties_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "opc/address_space/frl_opc_address_space.h"
#include "opc/frl_opc_util.h"
#include "opc/frl_opc_com_variant.h"

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

				if( wcslen( szItemID ) == 0 )
					return OPC_E_INVALIDITEMID;

				if( opcAddressSpace.isExistBranch( szItemID ) )
					return S_OK;

				if( ! opcAddressSpace.isExistLeaf( szItemID ) )
					return OPC_E_UNKNOWNITEMID;

				address_space::Tag *item = opcAddressSpace.getTag( szItemID );
				*pdwCount = 6;

				*ppPropertyIDs = util::allocMemory<DWORD>( *pdwCount );
				if( *ppPropertyIDs == NULL )
					return E_OUTOFMEMORY;
				util::zeroMemory<DWORD>( *ppPropertyIDs, *pdwCount );

				*ppDescriptions = util::allocMemory< LPWSTR >( *pdwCount );
				if( *ppDescriptions == NULL )
				{
					util::freeMemory( *ppPropertyIDs );
					return E_OUTOFMEMORY;
				}
				util::zeroMemory< LPWSTR >( *ppDescriptions, *pdwCount );

				*ppvtDataTypes = util::allocMemory< VARTYPE >( *pdwCount );
				if( *ppvtDataTypes == NULL )
				{
					util::freeMemory( *ppPropertyIDs );
					util::freeMemory( *ppDescriptions );
					return E_OUTOFMEMORY;
				}
				util::zeroMemory< VARTYPE >( *ppvtDataTypes, *pdwCount );

				// Data type
				(*ppPropertyIDs)[0] = OPC_PROPERTY_DATATYPE;
				(*ppDescriptions)[0] = util::duplicateString( OPC_PROPERTY_DESC_DATATYPE );
				(*ppvtDataTypes)[0] = VT_I2;

				// value
				(*ppPropertyIDs)[1] = OPC_PROPERTY_VALUE;
				(*ppDescriptions)[1] = util::duplicateString( OPC_PROPERTY_DESC_VALUE );
				(*ppvtDataTypes)[1] = item->getCanonicalDataType();

				// quality
				(*ppPropertyIDs)[2] = OPC_PROPERTY_QUALITY;
				(*ppDescriptions)[2] = util::duplicateString( OPC_PROPERTY_DESC_QUALITY );
				(*ppvtDataTypes)[2] = VT_I2;


				// time stamp
				(*ppPropertyIDs)[3] = OPC_PROPERTY_TIMESTAMP;
				(*ppDescriptions)[3] = util::duplicateString( OPC_PROPERTY_DESC_TIMESTAMP );
				(*ppvtDataTypes)[3] = VT_DATE;

				// access rights
				(*ppPropertyIDs)[4] = OPC_PROPERTY_ACCESS_RIGHTS;
				(*ppDescriptions)[4] = util::duplicateString( OPC_PROPERTY_DESC_ACCESS_RIGHTS );
				(*ppvtDataTypes)[4] = VT_I4;
				
				// scan rate
				(*ppPropertyIDs)[5] = OPC_PROPERTY_SCAN_RATE;
				(*ppDescriptions)[5] = util::duplicateString( OPC_PROPERTY_DESC_SCAN_RATE );
				(*ppvtDataTypes)[5] = VT_R4;


				return S_OK;
			}

			virtual HRESULT STDMETHODCALLTYPE GetItemProperties( 
				/* [in] */ LPWSTR szItemID,
				/* [in] */ DWORD dwCount,
				/* [size_is][in] */ DWORD *pdwPropertyIDs,
				/* [size_is][size_is][out] */ VARIANT **ppvData,
				/* [size_is][size_is][out] */ HRESULT **ppErrors)
			{
				if ( szItemID == NULL || ppvData == NULL || ppErrors  == NULL )
					return E_INVALIDARG;
				
				*ppvData  = NULL;
				*ppErrors = NULL;

				if( dwCount == 0 )
					return E_INVALIDARG;
				
				if( wcslen( szItemID ) == 0 )
					return OPC_E_INVALIDITEMID;

				if( ! opcAddressSpace.isExistLeaf( szItemID ) )
					return OPC_E_UNKNOWNITEMID;

				*ppvData = util::allocMemory< VARIANT >( dwCount );
				if( *ppvData == NULL )
					return E_OUTOFMEMORY;
				util::zeroMemory< VARIANT >( *ppvData, dwCount );
				
				*ppErrors = util::allocMemory< HRESULT >( dwCount );
				if( *ppErrors == NULL )
					return E_OUTOFMEMORY;
				util::zeroMemory< HRESULT >( *ppErrors, dwCount );

				address_space::Tag *item = opcAddressSpace.getTag( szItemID );

				HRESULT res = S_OK;
				for( DWORD i = 0; i < dwCount; i++ )
				{
					switch( pdwPropertyIDs[i] )
					{
						::VariantInit( &(*ppvData)[i] );

						case OPC_PROPERTY_DATATYPE:
						{
							ComVariant retVal = (short)item->getCanonicalDataType();
							ComVariant::variantCopy( &(*ppvData)[i], retVal.getPtr() );
						}
						break;
						
						case OPC_PROPERTY_VALUE:
						{
							ComVariant retVal = item->read();
							ComVariant::variantCopy( &(*ppvData)[i], retVal.getPtr() );
						}
						break;

						case OPC_PROPERTY_QUALITY:
						{
							ComVariant retVal = (short)item->getQuality();
							ComVariant::variantCopy( &(*ppvData)[i], retVal.getPtr() );
						}
						break;
						
						case OPC_PROPERTY_TIMESTAMP:
						{
							ComVariant retVal = item->getTimeStamp();
							ComVariant::variantCopy( &(*ppvData)[i], retVal.getPtr() );
						}
						break;

						case OPC_PROPERTY_ACCESS_RIGHTS:
						{
							ComVariant retVal = (long)item->getAccessRights();
							ComVariant::variantCopy( &(*ppvData)[i], retVal.getPtr() );
						}
						break;

						case OPC_PROPERTY_SCAN_RATE:
						{
							ComVariant retVal = (float)item->getScanRate();
							ComVariant::variantCopy( &(*ppvData)[i], retVal.getPtr() );
						}
						break;

						default:
						{
							(*ppErrors)[i] = OPC_E_INVALID_PID;
							res = S_FALSE;
							continue;
						}
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
				if ( szItemID == NULL || ppszNewItemIDs == NULL || ppErrors == NULL )
					return E_INVALIDARG;

				*ppszNewItemIDs = NULL;
				*ppErrors = NULL;
				
				if (dwCount == 0)
					return E_INVALIDARG;
				
				if( wcslen( szItemID ) == 0 )
					return OPC_E_INVALIDITEMID;

				if( ! opcAddressSpace.isExistLeaf( szItemID ) )
					return OPC_E_UNKNOWNITEMID;
				
				address_space::Tag *item = opcAddressSpace.getTag( szItemID );

				*ppszNewItemIDs = util::allocMemory< LPWSTR >( dwCount );
				if( ppszNewItemIDs == NULL )
					return E_OUTOFMEMORY;
				util::zeroMemory< LPWSTR>( *ppszNewItemIDs, dwCount );

				*ppErrors = util::allocMemory< HRESULT >( dwCount );
				if( ppErrors == NULL )
					return E_OUTOFMEMORY;
				util::zeroMemory< HRESULT >( *ppErrors, dwCount );

				HRESULT ret = S_OK;
				for( DWORD i = 0; i < dwCount; i++ )
				{
					switch( pdwPropertyIDs[i] )
					{
						case OPC_PROPERTY_DATATYPE:
						{
							(*ppszNewItemIDs)[i] = util::duplicateString( OPC_PROPERTY_DESC_DATATYPE );
						}
						break;

						case OPC_PROPERTY_VALUE:
						{
							(*ppszNewItemIDs)[i] = util::duplicateString( OPC_PROPERTY_DESC_VALUE );
						}
						break;

						case OPC_PROPERTY_TIMESTAMP:
						{
							(*ppszNewItemIDs)[i] = util::duplicateString( OPC_PROPERTY_DESC_TIMESTAMP );
						}
						break;

						case OPC_PROPERTY_ACCESS_RIGHTS:
						{
							(*ppszNewItemIDs)[i] = util::duplicateString( OPC_PROPERTY_DESC_ACCESS_RIGHTS );
						}
						break;
						default:
						{
							(*ppErrors)[i] = OPC_E_INVALID_PID;
							ret = S_FALSE;
						}
					}

					if( pdwPropertyIDs[i] <= OPC_PROPERTY_EU_INFO)
					{
						(*ppErrors)[i] = OPC_E_INVALID_PID;
						ret = S_FALSE;
					}
					else
					{
						(*ppErrors)[i] = S_OK;
					}
				}
				return ret;
			}
		}; // class ItemProperties
	} // namespace opc
} // namespace FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_item_properties_h_
