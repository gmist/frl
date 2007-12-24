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
		{}

		EnumString::~EnumString()
		{
			if( ! strings.empty() )
				strings.erase( strings.begin(), strings.end() );
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
			LONG ret = ::InterlockedDecrement( &refCount );
			if( ret <= 0 )
				delete this;
			return ret;
		}

		STDMETHODIMP EnumString::Next( ULONG celt, LPOLESTR* rgelt, ULONG* pceltFetched )
		{
			if( rgelt == NULL || pceltFetched == NULL )
				return E_INVALIDARG;

			*pceltFetched = 0;

			if( curIndex >= strings.size() )
				return S_FALSE;

			size_t i = curIndex;
			for( ; i < strings.size() && *pceltFetched < celt; i++ )
			{
				if ( strings[i].empty() )
				{
					rgelt[*pceltFetched] = NULL;
				}
				else
				{
					rgelt[*pceltFetched] = util::duplicateString( strings[i].c_str() );
				}
				(*pceltFetched)++;
			}

			if (*pceltFetched < celt)
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

			for( size_t i = 0; i < strings.size(); i++ )
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
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
