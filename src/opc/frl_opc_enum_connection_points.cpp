#include "opc/frl_opc_enum_connection_points.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_connection_point.h"

namespace frl{ namespace opc{

EnumConnectionPoints::EnumConnectionPoints()
	:	refCount( 0 ),
		currentIndex( 0 )
{
}

EnumConnectionPoints::EnumConnectionPoints( const ConnectionPointList &pointsList )
	:	refCount( 0 ),
		currentIndex( 0 ),
		points( pointsList.begin(), pointsList.end() )
{
}

EnumConnectionPoints::EnumConnectionPoints( const EnumConnectionPoints& other )
	:	refCount( 0 ),
		currentIndex( other.currentIndex ),
		points( other.points )
{
}

EnumConnectionPoints::~EnumConnectionPoints()
{
}

STDMETHODIMP EnumConnectionPoints::QueryInterface( REFIID iid, LPVOID* ppInterface )
{
	if( ppInterface == NULL )
		return E_INVALIDARG;

	if ( iid == IID_IUnknown )
	{
		*ppInterface = (IUnknown*) this;
	}
	else if ( iid == IID_IEnumConnectionPoints )
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

ULONG EnumConnectionPoints::AddRef( void )
{
	return ::InterlockedIncrement( &refCount );
}

ULONG EnumConnectionPoints::Release( void )
{
	LONG tmp = ::InterlockedDecrement( &refCount );
	if( tmp == 0 )
		delete this;
	return tmp;
}

STDMETHODIMP EnumConnectionPoints::Next( ULONG celt, LPCONNECTIONPOINT* rgelt, ULONG* pceltFetched )
{
	if( pceltFetched )
	{
		*pceltFetched = 0;
	}

	if( rgelt == NULL || ( celt != 1 && pceltFetched == NULL) )
	{
		return E_POINTER;
	}

	if( currentIndex >= points.size() )
		return S_FALSE;

	size_t i = currentIndex;
	size_t j = 0;
	for( ; i < points.size() && j < celt; ++i, ++j )
	{
		rgelt[j] = points[i].get();
		rgelt[j]->AddRef();
		if( pceltFetched )
			++(*pceltFetched);
	}

	if( j < celt )
	{
		currentIndex = points.size();
		return S_FALSE;
	}
	currentIndex = i;
	return S_OK;
}

STDMETHODIMP EnumConnectionPoints::Skip( ULONG celt )
{
	if ( currentIndex + celt > points.size())
	{
		currentIndex = points.size();
		return S_FALSE;
	}

	currentIndex += celt;
	return S_OK;
}

STDMETHODIMP EnumConnectionPoints::Reset()
{
	currentIndex = 0;
	return S_OK;
}

STDMETHODIMP EnumConnectionPoints::Clone( IEnumConnectionPoints** ppEnum )
{
	if( ppEnum == NULL )
		return E_INVALIDARG;

	EnumConnectionPoints *pNewEnum = new EnumConnectionPoints( *this );
	if (pNewEnum == NULL)
	{
		*ppEnum=NULL;
		return E_OUTOFMEMORY;
	}

	HRESULT res = pNewEnum->QueryInterface( IID_IEnumConnectionPoints, (void**) ppEnum );
	if( FAILED( res ) )
		delete pNewEnum;
	return res;
}

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
