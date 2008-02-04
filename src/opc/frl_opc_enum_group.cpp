#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_enum_group.h"

namespace frl
{
	namespace opc
	{
		EnumGroup::EnumGroup()
			: refCount( 0 ), currentIndex( 0 )
		{
		}

		EnumGroup::EnumGroup( const std::vector< Group* > &groups )
			: refCount( 0 ), currentIndex( 0 )
		{
			for ( size_t i = 0; i < groups.size(); i++)
				groupList.push_back( groups[i] );
		}

		EnumGroup::~EnumGroup()
		{
			groupList.clear();
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

		STDMETHODIMP EnumGroup::Next( ULONG celt, IUnknown **rgelt, ULONG *pceltFetched )
		{
			if ( rgelt == NULL || pceltFetched == NULL || celt == 0 )
				return E_INVALIDARG;

			if( pceltFetched == NULL && celt != 1 )
				return E_POINTER;

			*pceltFetched = 0;

			if( currentIndex >= groupList.size() )
				return S_FALSE;

			Group *pGroup;
			size_t i = currentIndex;
			for ( ; ( i < groupList.size() ) && ( *pceltFetched < celt ); i++)
			{
				pGroup = groupList[i];
				rgelt[*pceltFetched] = reinterpret_cast< IUnknown*>( pGroup );
				if( pGroup != NULL  )
					pGroup->AddRef();

				(*pceltFetched)++;
			}

			if( *pceltFetched < celt )
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

			std::vector< Group* >::iterator it;
			for( it = groupList.begin(); it != groupList.end();  it++ )
			{
				pNewEnum->groupList.push_back( *it );
			}
			pNewEnum->currentIndex = currentIndex;
			HRESULT hResult = pNewEnum->QueryInterface( IID_IEnumUnknown, (void**) ppEnum );
			return S_OK;
		}
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
