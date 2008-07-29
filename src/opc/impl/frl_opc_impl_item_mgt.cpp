#include "opc/impl/frl_opc_impl_item_mgt.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcerror.h"
#include "opc/impl/frl_opc_enum_item_attributes.h"
#include "opc/address_space/frl_opc_address_space.h"
#include "opc/frl_opc_async_request.h"
#include "opc/frl_opc_group_base.h"
#include "opc/frl_opc_server.h"

namespace frl { namespace opc { namespace impl {

ItemMgt::~ItemMgt()
{
}

STDMETHODIMP ItemMgt::AddItems(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCITEMDEF *pItemArray,
	/* [size_is][size_is][out] */ OPCITEMRESULT **ppAddResults,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if( pItemArray == NULL || ppAddResults == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*ppAddResults = NULL;
	*ppErrors = NULL;

	if( dwCount == 0 ) 
		return E_INVALIDARG;

	*ppAddResults = os::win32::com::allocMemory<OPCITEMRESULT>( dwCount );
	if( *ppAddResults == NULL )
		return E_OUTOFMEMORY;
	os::win32::com::zeroMemory<OPCITEMRESULT>( *ppAddResults, dwCount );

	*ppErrors = os::win32::com::allocMemory<HRESULT>( dwCount );
	if( *ppErrors == NULL ) 
	{ 
		os::win32::com::freeMemory(*ppAddResults);
		return E_OUTOFMEMORY;
	}

	HRESULT res = S_OK;
	boost::mutex::scoped_lock guard( groupGuard );
	for( DWORD i = 0; i < dwCount; ++i )
	{
		if( pItemArray[i].szItemID == NULL || wcslen( pItemArray[i].szItemID ) == 0 )
		{
			(*ppErrors)[i] = OPC_E_INVALIDITEMID;
			res = S_FALSE;
			continue;
		}

		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			String itemID = pItemArray[i].szItemID;
		#else
			String itemID = wstring2string( pItemArray[i].szItemID );
		#endif

		if( ! opcAddressSpace::getInstance().isExistLeaf( itemID ) )
		{
			(*ppErrors)[i] = OPC_E_UNKNOWNITEMID;
			res = S_FALSE;
			continue;
		}

		if( ! os::win32::com::isValidType( pItemArray[i].vtRequestedDataType ) )
		{
			(*ppErrors)[i] = OPC_E_BADTYPE;
			res = S_FALSE;
			continue;
		}

		GroupItemElem item( new GroupItem() );
		item->Init( pItemArray[i] );
		(*ppAddResults)[i].hServer = item->getServerHandle();

		address_space::Tag *tag = opcAddressSpace::getInstance().getTag( itemID );
		(*ppAddResults)[i].vtCanonicalDataType = tag->getCanonicalDataType();
		(*ppAddResults)[i].dwAccessRights = tag->getAccessRights();
		(*ppAddResults)[i].dwBlobSize = 0;
		(*ppAddResults)[i].pBlob = NULL;
		itemList.insert( std::pair< OPCHANDLE, GroupItemElem > ( item->getServerHandle(), item ));
		(*ppErrors)[i] = S_OK;
	}
	return res;
}

STDMETHODIMP ItemMgt::ValidateItems(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCITEMDEF *pItemArray,
	/* [in] */ BOOL bBlobUpdate,
	/* [size_is][size_is][out] */ OPCITEMRESULT **ppValidationResults,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;
	if (pItemArray == NULL || ppValidationResults == NULL || ppErrors == NULL )
		return E_INVALIDARG;
	*ppValidationResults = NULL;
	*ppErrors = NULL;

	if( dwCount == 0 )
		return E_INVALIDARG;

	*ppValidationResults = os::win32::com::allocMemory< OPCITEMRESULT >( dwCount );
	if( *ppValidationResults == NULL )
		return E_OUTOFMEMORY;
	os::win32::com::zeroMemory< OPCITEMRESULT >( *ppValidationResults, dwCount );

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	if( *ppErrors == NULL )
	{
		os::win32::com::freeMemory( *ppValidationResults );
		return E_OUTOFMEMORY;
	}
	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT res = S_OK;
	boost::mutex::scoped_lock guard( groupGuard );
	for( DWORD i = 0; i < dwCount; ++i )
	{
		if( pItemArray[i].szItemID == NULL || wcslen(pItemArray[i].szItemID) == 0 )
		{
			(*ppErrors)[i] = OPC_E_INVALIDITEMID;
			res = S_FALSE;
			continue;
		}

		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
			String itemID = pItemArray[i].szItemID;
		#else
			String itemID = wstring2string( pItemArray[i].szItemID );
		#endif

		if( ! opcAddressSpace::getInstance().isExistLeaf( itemID ) )
		{
			(*ppErrors)[i] = OPC_E_UNKNOWNITEMID;
			res = S_FALSE;
			continue;
		}

		if (! os::win32::com::isValidType(pItemArray[i].vtRequestedDataType ) )
		{
			(*ppErrors)[i] = OPC_E_BADTYPE;
			res = S_FALSE;
			continue;
		}

		address_space::Tag *item = opcAddressSpace::getInstance().getTag(itemID );
		ppValidationResults[0][i].vtCanonicalDataType = item->getCanonicalDataType();
		ppValidationResults[0][i].dwAccessRights = item->getAccessRights();
		ppValidationResults[0][i].dwBlobSize = 0;
		ppValidationResults[0][i].pBlob = NULL;
		ppValidationResults[0][i].hServer = 	util::getUniqueServerHandle();
	}
	return res;
}

STDMETHODIMP ItemMgt::RemoveItems(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if( phServer == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*ppErrors = NULL;
	if( dwCount == 0 )
		return E_INVALIDARG;

	*ppErrors = os::win32::com::allocMemory<HRESULT>( dwCount );
	if( *ppErrors == NULL )
		return E_OUTOFMEMORY;
	os::win32::com::zeroMemory<HRESULT>( *ppErrors, dwCount );

	HRESULT res = S_OK;
	GroupItemElemList::iterator it;
	boost::mutex::scoped_lock guard( groupGuard );
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		it = itemList.find( phServer[i] );
		if( it == end ) 
		{
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			res = S_FALSE;
			continue;
		}
		// and disconnected from all async requests
		server->removeItemFromRequestList( phServer[i] );
		itemList.erase( it );
	}
	return res;
}

STDMETHODIMP ItemMgt::SetActiveState(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [in] */ BOOL bActive,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;
	if (phServer == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*ppErrors = NULL;
	if( dwCount == 0 )
		return E_INVALIDARG;

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	if( *ppErrors == NULL ) 
		return E_OUTOFMEMORY;
	os::win32::com::zeroMemory<HRESULT>( *ppErrors, dwCount );

	HRESULT res = S_OK;
	GroupItemElemList::iterator it;
	boost::mutex::scoped_lock guard( groupGuard );
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		it = itemList.find( phServer[i] );
		if ( it == end )
		{
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			res = S_FALSE;
			continue;
		}
		(*it).second->isActived( ( bActive == VARIANT_TRUE ) || ( bActive == TRUE ) );
		(*ppErrors)[i] = S_OK;
	}
	return res;
}

STDMETHODIMP ItemMgt::SetClientHandles(
	/* [in] */ DWORD dwCount,
	/* [size_is][in] */ OPCHANDLE *phServer,
	/* [size_is][in] */ OPCHANDLE *phClient,
	/* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if ( phServer == NULL || phClient == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*ppErrors = NULL;

	if( dwCount == 0 )
		return E_INVALIDARG;

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	if( *ppErrors == NULL )
		return E_OUTOFMEMORY;

	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT res = S_OK;
	GroupItemElemList::iterator it;
	boost::mutex::scoped_lock guard( groupGuard );
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		it = itemList.find( phServer[i] );
		if( it == end )
		{
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			res = S_FALSE;
		}
		else
			(*it).second->setClientHandle( phClient[i] );
	}
	return res;
}

STDMETHODIMP ItemMgt::SetDatatypes( /* [in] */ DWORD dwCount, /* [size_is][in] */ OPCHANDLE *phServer, /* [size_is][in] */ VARTYPE *pRequestedDatatypes, /* [size_is][size_is][out] */ HRESULT **ppErrors )
{
	if( deleted )
		return E_FAIL;

	if( phServer == NULL || pRequestedDatatypes == NULL || ppErrors == NULL )
		return E_INVALIDARG;

	*ppErrors = NULL;
	if( dwCount == 0 )
		return E_INVALIDARG;

	*ppErrors = os::win32::com::allocMemory< HRESULT >( dwCount );
	if( *ppErrors == NULL )
		return E_OUTOFMEMORY;

	os::win32::com::zeroMemory< HRESULT >( *ppErrors, dwCount );

	HRESULT res = S_OK;
	GroupItemElemList::iterator it;
	boost::mutex::scoped_lock guard( groupGuard );
	GroupItemElemList::iterator end = itemList.end();
	for( DWORD i = 0; i < dwCount; ++i )
	{
		it = itemList.find( phServer[i] );
		if( it == end )
		{
			(*ppErrors)[i] = OPC_E_INVALIDHANDLE;
			res = S_FALSE;
			continue;
		}
		if( ! os::win32::com::isValidType( pRequestedDatatypes[i] ) )
		{
			(*ppErrors)[i] = OPC_E_BADTYPE;
			res = S_FALSE;
			continue;
		}
		(*it).second->setRequestDataType( pRequestedDatatypes[i] );
	}
	return res;
}

STDMETHODIMP ItemMgt::CreateEnumerator( /* [in] */ REFIID riid, /* [iid_is][out] */ LPUNKNOWN *ppUnk )
{
	if( deleted )
		return E_FAIL;

	boost::mutex::scoped_lock guard( groupGuard );
	if (ppUnk == NULL)
		return E_INVALIDARG;

	if ( riid == IID_IEnumOPCItemAttributes)
	{
		if( itemList.empty() )
			return S_FALSE;

		EnumOPCItemAttributes *temp = new EnumOPCItemAttributes();
		if (temp == NULL)
			return E_OUTOFMEMORY;

		std::for_each( itemList.begin(), itemList.end(), boost::bind( &EnumOPCItemAttributes::addItem, temp, _1 ) );
		return temp->QueryInterface(riid,(void**)ppUnk);
	}
	return E_INVALIDARG;
}

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
