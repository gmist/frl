#ifndef FRL_OS_WINDOWS_REGISTRY_ROOTKEY_H_
#define FRL_OS_WINDOWS_REGISTRY_ROOTKEY_H_

#include "frl_platform.h"

#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "frl_platform.h"
#include "frl_types.h"
#include "frl_string.h"

namespace frl
{
	namespace os
	{
		namespace win32
		{
			namespace registry
			{
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
} // frl

#endif // #if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )

#endif /* FRL_OS_WINDOWS_REGISTRY_ROOTKEY_H_ */
