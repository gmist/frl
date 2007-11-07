#ifndef FRL_IO_H_HEADER_
#define FRL_IO_H_HEADER_
#include "frl_platform.h"

#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "io/win32/registry/frl_registry_RootKey.h"
#include "io/win32/registry/frl_registry_RootKeys.h"
#include "io/win32/registry/frl_registry_Key.h"
#endif /* FRL_PLATFORM_WIN32 */

#endif /* FRL_IO_H_HEADER_ */
