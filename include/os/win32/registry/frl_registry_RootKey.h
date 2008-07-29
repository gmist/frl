#ifndef frl_os_win32_registry_rootkey_h_
#define frl_os_win32_registry_rootkey_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "frl_platform.h"
#include "frl_types.h"
#include "frl_string.h"

namespace frl{ namespace os{ namespace win32{ namespace registry{

class RootKey
{
private:
	frl::String name;
	HKEY value;
	
public:
	// Constructors for RootKeys
	RootKey();
	RootKey( frl::String newName );
	RootKey( const RootKey &rval );
	RootKey& operator = ( const RootKey &rval );

	// Return root key HKEY
	HKEY getValue();
	// return root key name
	frl::String toString();
	frl::Bool operator == ( const RootKey &rhs );
}; // RootKey

} // namespace registry
} // namespace win32
} // namespace os
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_os_win32_registry_rootkey_h_
