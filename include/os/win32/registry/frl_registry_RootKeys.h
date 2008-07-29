#ifndef frl_os_win32_registry_rootkeys_h_
#define frl_os_win32_registry_rootkeys_h_
#include "os/win32/registry/frl_registry_RootKey.h"

namespace frl{ namespace os{ namespace win32{ namespace registry{

struct  RootKeys
{
	static const RootKey classesRoot;
	static const RootKey currentConfig;
	static const RootKey currentUser;
	static const RootKey dynData;
	static const RootKey localMachine;
	static const RootKey performanceData;
	static const RootKey performanceNlsText;
	static const RootKey performanceText;
	static const RootKey users;
};

} // namespace registry
} // namespace win32
} // namespace os
} // FatRat Library

#endif // frl_os_win32_registry_rootkeys_h_
