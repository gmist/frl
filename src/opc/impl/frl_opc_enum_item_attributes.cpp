#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/impl/frl_opc_enum_item_attributes.h"

namespace frl{ namespace opc{ namespace impl{

EnumOPCItemAttributes::EnumOPCItemAttributes()
	:	refCount( 0 ),
		curIndex( 0 )
{
}

EnumOPCItemAttributes::EnumOPCItemAttributes( const EnumOPCItemAttributes& other )
	:	refCount( 0 ),
		itemList( other.itemList ),
		curIndex( other.curIndex )
{
}

EnumOPCItemAttributes::~EnumOPCItemAttributes()
{
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

void EnumOPCItemAttributes::addItem( const std::pair< OPCHANDLE, GroupItemElem >& newItem )
{
	ItemAttributes attrib;
	attrib = newItem;
	itemList.push_back( attrib );
}

/*!
	Fetch the next "celt" items from the group.

	\param[in] celt
		number of items to be fetched.

	\param[out] ppItemArray
		Array of OPCITEMATTRIBUTES. Returned by the server.

	\param[out] pceltFetched
		Number of items actually returned.

	\section EnumOPCItemAttributes_Next_Comments Comments
		The client must free the returned OPCITEMATTRIBUTES 
		structure including the contained items; szItemID, szAccessPath, pBlob, vEUInfo.
*/
STDMETHODIMP EnumOPCItemAttributes::Next( ULONG celt, OPCITEMATTRIBUTES** ppItemArray, ULONG* pceltFetched )
{
	if( pceltFetched )
	{
		*pceltFetched = 0;
	}

	if( ppItemArray == NULL || ( celt != 1 && pceltFetched == NULL) )
	{
		return E_POINTER;
	}

	if( curIndex >= itemList.size() )
		return S_FALSE;

	*ppItemArray = os::win32::com::allocMemory< OPCITEMATTRIBUTES >( celt );
	size_t i = curIndex;
	size_t j = 0;
	for( ; i < itemList.size() && j < celt; ++i, ++j )
	{
		itemList[i].copyTo( (*ppItemArray)[j] );
		if( pceltFetched )
			++(*pceltFetched);
	}

	if( j < celt )
	{
		curIndex = itemList.size();
		return S_FALSE;
	}

	curIndex = i;
	return S_OK;
}

/*!
	Skip over the next "celt" attributes.

	\param[in] celt
		Number of items to skip

	\section EnumOPCItemAttributes_Skip_Comments Comments
		Skip is probably not useful in the context of OPC.
*/
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

/*!
	Reset the enumerator back to the first item.
*/
STDMETHODIMP EnumOPCItemAttributes::Reset( void )
{
	curIndex = 0;
	return S_OK;
}

/*!
	Create a 2nd copy of the enumerator. 
	The new enumerator will initially be in the same "state" as the current enumerator.

	\param[out] ppEnumItemAttributes
		Place to return the new interface

	\section EnumOPCItemAttributes_Clone_Comments Comments
		The client must release the returned interface pointer when it is done with it.
*/
STDMETHODIMP EnumOPCItemAttributes::Clone( IEnumOPCItemAttributes** ppEnumItemAttributes )
{
	if (ppEnumItemAttributes == NULL)
		return E_INVALIDARG;

	EnumOPCItemAttributes* pEnum = new EnumOPCItemAttributes( *this );
	if( pEnum == NULL )
	{
		*ppEnumItemAttributes = NULL;
		return E_OUTOFMEMORY;
	}

	HRESULT res = pEnum->QueryInterface( IID_IEnumOPCItemAttributes, (void**)ppEnumItemAttributes );
	if( FAILED( res ) )
		delete pEnum;
	return res;
}

} // namespace impl
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
