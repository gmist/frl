#include "frl_platform.h"

#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_server_types.h"

namespace frl
{
namespace opc
{
const ServerType ServerTypes::localSever32( FRL_STR( "LocalServer32" ) );
const ServerType ServerTypes::inprocServer32( FRL_STR( "inprocServer32" ) );
const ServerType ServerTypes::inprocHandler32( FRL_STR( "InprocHandler32" ) );
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM
