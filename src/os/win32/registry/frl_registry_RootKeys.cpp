#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "os/win32/registry/frl_registry_RootKeys.h"

namespace frl
{
	namespace os
	{
		namespace win32
		{
			namespace registry
			{
				const RootKey RootKeys::classesRoot( FRL_STR("HKEY_CLASSES_ROOT") );
				const RootKey RootKeys::currentConfig( FRL_STR("HKEY_CURRENT_CONFIG") );
				const RootKey RootKeys::currentUser( FRL_STR("HKEY_CURRENT_USER") );
				const RootKey RootKeys::dynData( FRL_STR("HKEY_DYN_DATA") );
				const RootKey RootKeys::localMachine( FRL_STR("HKEY_LOCAL_MACHINE") );
				const RootKey RootKeys::performanceData( FRL_STR("HKEY_PERFORMANCE_DATA") );
				const RootKey RootKeys::performanceNlsText( FRL_STR("HKEY_PERFORMANCE_NLSTEXT") );
				const RootKey RootKeys::performanceText( FRL_STR("HKEY_PERFORMANCE_TEXT") );
				const RootKey RootKeys::users( FRL_STR("HKEY_USERS") );
			} // namespace registry
		} // namespace win32
	} // namespace os
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
