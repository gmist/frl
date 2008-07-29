#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_enum_item_attributes.h"

namespace frl
{
namespace opc
{

EnumOPCItemAttributes::EnumOPCItemAttributes()
	:	refCount( 0 ),
		curIndex( 0 )
{
}

EnumOPCItemAttributes::~EnumOPCItemAttributes()
{
	for( size_t i=0; i< itemList.size(); ++i )
	{
		if( itemList[i] != NULL )
		{
			if( itemList[i]->szItemID != NULL )
				os::win32::com::freeMemory( itemList[i]->szItemID );

			if( itemList[i]->szAccessPath != NULL )
				os::win32::com::freeMemory( itemList[i]->szAccessPath );

			os::win32::com::freeMemory( itemList[i] );
		}
	}
}

STDMETHODIMP EnumOPCItemAttributes::QueryInterface( REFIID iid, LPVOID* ppInterface )
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

STDMETHODIMP_(ULONG) EnumOPCItemAttributes::AddRef( void )
{
	return ::InterlockedIncrement( &refCount );
}

STDMETHODIMP_(ULONG) EnumOPCItemAttributes::Release( void )
{
	LONG ret = ::InterlockedDecrement( &refCount );
	if( ret == 0 )
		delete this;
	return ret;
}

void EnumOPCItemAttributes::copy( OPCITEMATTRIBUTES &dst, OPCITEMATTRIBUTES &src )
{
	os::win32::com::zeroMemory( &dst );
	dst.bActive = src.bActive;
	dst.dwAccessRights = src.dwAccessRights;
	dst.dwBlobSize = src.dwBlobSize;
	dst.dwEUType = src.dwEUType;
	dst.hClient = src.hClient;
	dst.hServer = src.hServer;
	dst.pBlob = src.pBlob;
	dst.szAccessPath = util::duplicateString( src.szAccessPath );
	dst.szItemID = util::duplicateString( src.szItemID );
	VariantCopy(&dst.vEUInfo , &src.vEUInfo );
	dst.vtCanonicalDataType = src.vtCanonicalDataType;
	dst.vtRequestedDataType = src.vtRequestedDataType;
}

void EnumOPCItemAttributes::addItem( OPCHANDLE first, const GroupItemElem &i )
{
	OPCITEMATTRIBUTES *attributes = os::win32::com::allocMemory<OPCITEMATTRIBUTES>();
	os::win32::com::zeroMemory( attributes );
	attributes->bActive = i->isActived();
	attributes->hClient = i->getClientHandle();
	attributes->hServer = first;

	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		attributes->szItemID = util::duplicateString( i->getItemID() );
	#else
		attributes->szItemID = util::duplicateString( string2wstring( i->getItemID() ) );
	#endif
	
	#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		attributes->szItemID = util::duplicateString( i->getAccessPath() );
	#else
			attributes->szItemID = util::duplicateString( string2wstring( i->getAccessPath() ) );
	#endif

	address_space::Tag *item = opcAddressSpace::getInstance().getTag( i->getItemID() );
	attributes->dwAccessRights = item->getAccessRights();
	attributes->dwBlobSize = 0;
	attributes->pBlob = NULL;
	attributes->vtCanonicalDataType = item->getCanonicalDataType();
	attributes->vtRequestedDataType = i->getReguestDataType();
	itemList.push_back( attributes );
}

STDMETHODIMP EnumOPCItemAttributes::Next( ULONG celt, OPCITEMATTRIBUTES **ppItemArray, ULONG *pceltFetched )
{
	if( pceltFetched )
	{
		*pceltFetched = 0;
	}

	if( ppItemArray == NULL )
	{
		return E_INVALIDARG;
	}

	if( ppItemArray == NULL || ( celt != 1 && pceltFetched == NULL) )
	{
		return E_POINTER;
	}
	
	if( curIndex >= itemList.size() )
		return S_FALSE;

	*ppItemArray = os::win32::com::allocMemory< OPCITEMATTRIBUTES >( celt );
	size_t i = curIndex;
	for( ; i < itemList.size() && *pceltFetched < celt; ++i )
	{
		 copy( (*ppItemArray)[*pceltFetched], *itemList[i] );
		++(*pceltFetched);
	}

	if (*pceltFetched < celt)
	{
		curIndex = itemList.size();
		return S_FALSE;
	}

	curIndex = i;
	return S_OK;
}

STDMETHODIMP EnumOPCItemAttributes::Skip( ULONG celt )
{
	if (curIndex + celt > itemList.size())
	{
		curIndex = itemList.size();
		return S_FALSE;
	}
	curIndex += celt;
	return S_OK;
}

STDMETHODIMP EnumOPCItemAttributes::Reset( void )
{
	curIndex = 0;
	return S_OK;
}

STDMETHODIMP EnumOPCItemAttributes::Clone( IEnumOPCItemAttributes **ppEnum )
{
	if (ppEnum == NULL)
		return E_INVALIDARG;

	EnumOPCItemAttributes* pEnum = new EnumOPCItemAttributes();
	if( pEnum == NULL )
		return E_OUTOFMEMORY;

	pEnum->itemList.reserve( itemList.size() );
	OPCITEMATTRIBUTES *pItem;
	for( size_t i = 0; i < itemList.size(); ++i )
	{
		pItem = itemList[i];
		OPCITEMATTRIBUTES *newItem = os::win32::com::allocMemory< OPCITEMATTRIBUTES >();
		copy( *newItem, *pItem );
		pEnum->itemList.push_back( newItem );
	}
	pEnum->curIndex = curIndex;
	HRESULT hResult = pEnum->QueryInterface( IID_IEnumOPCItemAttributes, (void**)ppEnum );
	if( FAILED( hResult ) )
		delete pEnum;

	return hResult;
}

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
