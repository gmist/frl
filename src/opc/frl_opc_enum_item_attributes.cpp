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

OPCITEMATTRIBUTES* EnumOPCItemAttributes::copy( OPCITEMATTRIBUTES *i )
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
	c->szAccessPath = util::duplicateString( i->szAccessPath );
	c->szItemID = util::duplicateString( i->szItemID );
	VariantCopy(&c->vEUInfo , &i->vEUInfo );
	c->vtCanonicalDataType = i->vtCanonicalDataType;
	c->vtRequestedDataType = i->vtRequestedDataType;
	return c;
}

void EnumOPCItemAttributes::addItem( OPCHANDLE first, const GroupItemElem &i )
{
	OPCITEMATTRIBUTES *attributes = (OPCITEMATTRIBUTES *)CoTaskMemAlloc( sizeof(OPCITEMATTRIBUTES) );
	ZeroMemory( attributes, sizeof(OPCITEMATTRIBUTES) );
	attributes->bActive = i->isActived();
	attributes->hClient = i->getClientHandle();
	attributes->hServer = first;

	if( i->getItemID().empty() )
	{
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		attributes->szItemID = util::duplicateString( FRL_STR("") );
		#else
		attributes->szItemID = util::duplicateString( string2wstring( FRL_STR("") ) );
		#endif
	}
	else
	{
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		attributes->szItemID = util::duplicateString( i->getItemID() );
		#else
		attributes->szItemID = util::duplicateString( string2wstring( i->getItemID() ) );
		#endif
	}

	if( i->getAccessPath().empty() )
	{
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		attributes->szAccessPath = util::duplicateString( FRL_STR(""));
		#else
		attributes->szAccessPath = util::duplicateString( string2wstring( FRL_STR("") ) );
		#endif
	}
	else
	{
		#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
		attributes->szItemID = util::duplicateString( i->getAccessPath() );
		#else
		attributes->szItemID = util::duplicateString( string2wstring( i->getAccessPath() ) );
		#endif
	}

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

	size_t i;
	for( i = 0; i < celt; ++i )
	{
		if( curIndex >= itemList.size() ) break;
		ppItemArray[i] = copy( itemList[curIndex] );
		if( pceltFetched != NULL )
			++(*pceltFetched);
		++curIndex;
	}

	if( i == celt )
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}

STDMETHODIMP EnumOPCItemAttributes::Skip( ULONG celt )
{
	curIndex += celt;
	if( itemList.size() <= curIndex )
		return S_FALSE;
	return S_OK;
}

STDMETHODIMP EnumOPCItemAttributes::Reset( void )
{
	curIndex = 0;
	return S_OK;
}

STDMETHODIMP EnumOPCItemAttributes::Clone( IEnumOPCItemAttributes **ppEnum )
{
	// check for invalid arguments.
	if (ppEnum == NULL)
		return E_INVALIDARG;

	// allocate enumerator.
	EnumOPCItemAttributes* pEnum = new EnumOPCItemAttributes();
	if( pEnum == NULL )
		return E_OUTOFMEMORY;

	pEnum->itemList.reserve( itemList.size() );
	for( size_t i = 0; i < itemList.size(); ++i )
	{
		OPCITEMATTRIBUTES *pItem = itemList[i];
		OPCITEMATTRIBUTES *newItem = new OPCITEMATTRIBUTES();
		if( newItem != NULL )
		{
			newItem = copy( pItem );
			pEnum->itemList.push_back( newItem );
		}
	}
	pEnum->curIndex = curIndex;
	*ppEnum = pEnum;
	return S_OK;
}
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
