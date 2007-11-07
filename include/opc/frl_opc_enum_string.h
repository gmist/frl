#ifndef frl_opc_enum_string_h_
#define frl_opc_enum_string_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "frl_types.h"
#include <vector>

namespace frl
{
	namespace opc
	{
		class EnumString : public IEnumString
		{
		public:
			EnumString(); // Constructor
			~EnumString(); // Destructor

			void init( const std::vector< String > items );

			// the IUnknown functions implementation
			STDMETHODIMP QueryInterface( REFIID iid, LPVOID* ppInterface );
			STDMETHODIMP_(ULONG) AddRef( void );
			STDMETHODIMP_(ULONG) Release( void );

			// the IEnumString functions implementation
			STDMETHODIMP Next ( ULONG celt, LPOLESTR* rgelt, ULONG*    pceltFetched );
			STDMETHODIMP Skip ( ULONG celt );
			STDMETHODIMP Reset( void );
			STDMETHODIMP Clone( IEnumString **ppEnum );

		protected:
			volatile LONG RefCount;			// reference counter
			std::vector<String> strings;	// enum strings
			size_t curIndex;						//Current element
		};
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_enum_string_h_ */
