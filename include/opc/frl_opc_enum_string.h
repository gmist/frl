#ifndef frl_opc_enum_string_h_
#define frl_opc_enum_string_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "frl_types.h"
#include <vector>
#include "os/win32/com/frl_os_win32_com_allocator.h"

namespace frl
{
	namespace opc
	{
		class EnumString : public IEnumString, public os::win32::com::Allocator
		{
		public:
			EnumString(); // Constructor
			~EnumString(); // Destructor

			void init( const std::vector< String > &items );

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
			volatile LONG refCount;			// reference counter
			size_t curIndex;						// current element
			std::vector<String> strings;	// enum strings
		}; // class EnumString
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_enum_string_h_ */
