#ifndef FRL_OS_WINDOWS_REGISTRY_ROOTKEYS_H_
#define FRL_OS_WINDOWS_REGISTRY_ROOTKEYS_H_
#include "io/win32/registry/frl_registry_RootKey.h"

namespace frl
{
	namespace io
	{
		namespace win32
		{
			namespace registry
			{
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
	} // namespace io
} // namespace FatRat Library

#endif /* FRL_OS_WINDOWS_REGISTRY_ROOTKEYS_H_ */