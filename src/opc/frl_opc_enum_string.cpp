#include "opc/frl_opc_enum_string.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )

namespace frl
{
	namespace opc
	{
		EnumString::EnumString()
			: refCount( 0 ), curIndex( 0 )
		{}

		EnumString::~EnumString()
		{
			if ( ! strings.empty() )
				strings.erase( strings.begin(), strings.end() );
		}

		STDMETHODIMP EnumString::QueryInterface( REFIID iid, LPVOID* ppInterface )
		{
			if ( ppInterface == NULL)
				return E_INVALIDARG;

			if ( iid == IID_IUnknown )
			{
				*ppInterface = (IUnknown*) this;
			}
			else if (iid == IID_IEnumString)
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
			if (rgelt == NULL || pceltFetched == NULL)
				return E_INVALIDARG;

			if( curIndex >= strings.size() )
				return S_FALSE;

			size_t ii = curIndex;
			for ( ; ii < strings.size() && *pceltFetched < celt; ii++)
			{
				size_t size = ( ( strings[ii].length() +1 ) * sizeof(WCHAR) );
				rgelt[*pceltFetched] = (LPWSTR)CoTaskMemAlloc( size );
				if( rgelt[*pceltFetched] == NULL )
					return E_OUTOFMEMORY;

				if ( strings[ii].empty() )
				{
					rgelt[*pceltFetched] = NULL;
				}
				else
				{
					wcscpy_s( rgelt[*pceltFetched], size/sizeof(WCHAR), strings[ii].c_str() );
				}
				(*pceltFetched)++;
			}

			if (*pceltFetched < celt)
			{
				curIndex = strings.size();
				return S_FALSE;
			}

			curIndex = ii;
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
			if (ppEnum == NULL)
				return E_INVALIDARG;

			EnumString* pEnum = new EnumString();
			if ( pEnum == NULL )
			{
				*ppEnum = NULL;
				return E_OUTOFMEMORY;
			}

			pEnum->AddRef();

			for (UINT ii = 0; ii < strings.size(); ii++)
			{
				pEnum->strings.push_back( strings[ii] );
			}

			pEnum->curIndex = curIndex;

			HRESULT hResult = pEnum->QueryInterface(IID_IEnumString, (void**)ppEnum);
			// release local reference.
			pEnum->Release();
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
