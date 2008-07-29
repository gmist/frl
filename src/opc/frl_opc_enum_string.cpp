#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_enum_string.h"
#include "opc/frl_opc_util.h"

namespace frl
{
namespace opc
{
EnumString::EnumString()
	: refCount( 0 ), curIndex( 0 )
{
}

EnumString::~EnumString()
{
}

STDMETHODIMP EnumString::QueryInterface( REFIID iid, LPVOID* ppInterface )
{
	if( ppInterface == NULL )
		return E_INVALIDARG;

	if( iid == IID_IUnknown )
	{
		*ppInterface = (IUnknown*) this;
	}
	else if(iid == IID_IEnumString)
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

ULONG EnumString::AddRef( void )
{
	return ::InterlockedIncrement( &refCount );
}

ULONG EnumString::Release( void )
{
	LONG tmp = ::InterlockedDecrement( &refCount );
	if( tmp == 0 )
		delete this;
	return tmp;
}

STDMETHODIMP EnumString::Next( ULONG celt, LPOLESTR* rgelt, ULONG* pceltFetched )
{
	if( pceltFetched )
	{
		*pceltFetched = 0;
	}

	if( rgelt == NULL || ( celt != 1 && pceltFetched == NULL) )
	{
		return E_POINTER;
	}

	if( curIndex >= strings.size() )
		return S_FALSE;

	size_t i = curIndex;
	size_t j = 0;
	for( ; i < strings.size() && j < celt; ++i, ++j )
	{
		if ( strings[i].empty() )
		{
			rgelt[j] = NULL;
		}
		else
		{
			#if( FRL_CHARACTER == FRL_CHARACTER_UNICODE )
				rgelt[j] = util::duplicateString( strings[i] );
			#else
				rgelt[j] = util::duplicateString( string2wstring( strings[i] ) );
			#endif
		}
		if( pceltFetched )
			++(*pceltFetched);
	}

	if( j < celt )
	{
		curIndex = strings.size();
		return S_FALSE;
	}

	curIndex = i;
	return S_OK;
}

STDMETHODIMP EnumString::Skip( ULONG celt )
{
	if (curIndex + celt > strings.size())
	{
		curIndex = strings.size();
		return S_FALSE;
	}

	curIndex += celt;
	return S_OK;
}

STDMETHODIMP EnumString::Reset( void )
{
	curIndex = 0;
	return S_OK;
}

STDMETHODIMP EnumString::Clone( IEnumString **ppEnum )
{
	if( ppEnum == NULL )
		return E_INVALIDARG;

	EnumString* pEnum = new EnumString();
	if( pEnum == NULL )
	{
		*ppEnum = NULL;
		return E_OUTOFMEMORY;
	}

	for( size_t i = 0; i < strings.size(); ++i )
	{
		pEnum->strings.push_back( strings[i] );
	}

	pEnum->curIndex = curIndex;

	HRESULT hResult = pEnum->QueryInterface(IID_IEnumString, (void**)ppEnum);
	return hResult;
}

void EnumString::init( const std::vector< String > &items )
{
	strings = items;
	Reset();
}
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
