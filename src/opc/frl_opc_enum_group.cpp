#include "opc/frl_opc_enum_group.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_group.h"

namespace frl
{
namespace opc
{
EnumGroup::EnumGroup()
	: refCount( 0 ), currentIndex( 0 )
{
}

EnumGroup::EnumGroup( const std::vector< GroupElem > &groups )
	: refCount( 0 ), currentIndex( 0 ), groupList( groups )
{
}

EnumGroup::~EnumGroup()
{
}

STDMETHODIMP EnumGroup::QueryInterface( REFIID iid, LPVOID* ppInterface )
{
	if ( ppInterface == NULL )
	{
		return E_INVALIDARG;
	}

	if ( iid == IID_IUnknown )
	{
		*ppInterface = (IUnknown*) this;
	}
	else if ( iid == IID_IEnumUnknown )
	{
		*ppInterface = this;
	}
	else
	{
		*ppInterface = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG EnumGroup::AddRef( void )
{
	return ::InterlockedIncrement( &refCount );
}

ULONG EnumGroup::Release( void )
{
	LONG tmp = ::InterlockedDecrement( &refCount );
	if( tmp == 0 )
		delete this;
	return tmp;
}

STDMETHODIMP EnumGroup::Next( ULONG celt, IUnknown** rgelt, ULONG *pceltFetched )
{
	if( pceltFetched )
	{
		*pceltFetched = 0;
	}

	if( rgelt == NULL || ( celt != 1 && pceltFetched == NULL) )
	{
		return E_POINTER;
	}

	if( currentIndex >= groupList.size() )
		return S_FALSE;

	Group *pGroup;
	size_t i = currentIndex;
	size_t j = 0;
	for( ; i < groupList.size() && j < celt; ++i, ++j )
	{
		pGroup = groupList[i].get();
		rgelt[j] = reinterpret_cast< IUnknown*>( pGroup );
		if( pGroup != NULL  )
			pGroup->AddRef();
		if( pceltFetched )
			++(*pceltFetched);
	}

	if( j < celt )
	{
		currentIndex = groupList.size();
		return S_FALSE;
	}
	currentIndex = i;
	return S_OK;
}

STDMETHODIMP EnumGroup::Skip( ULONG celt )
{
	if ( currentIndex + celt > groupList.size())
	{
		currentIndex = groupList.size();
		return S_FALSE;
	}

	currentIndex += celt;
	return S_OK;
}

STDMETHODIMP EnumGroup::Reset( void )
{
	currentIndex = 0;
	return S_OK;
}

STDMETHODIMP EnumGroup::Clone( IEnumUnknown **ppEnum )
{
	if (ppEnum == NULL)
		return E_INVALIDARG;

	EnumGroup *pNewEnum = new EnumGroup();
	if (pNewEnum == NULL)
	{
		*ppEnum=NULL;
		return E_OUTOFMEMORY;
	}

	pNewEnum->groupList = groupList;
	pNewEnum->currentIndex = currentIndex;
	HRESULT hResult = pNewEnum->QueryInterface( IID_IEnumUnknown, (void**) ppEnum );
	return hResult;
}
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
