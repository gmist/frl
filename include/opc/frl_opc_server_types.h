#ifndef frl_opc_server_types_h_
#define frl_opc_server_types_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_server_type.h"

namespace frl{ namespace opc{

struct ServerTypes
{
	static const ServerType localSever32;
	static const ServerType inprocServer32;
	static const ServerType inprocHandler32;
};

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_server_types_h_
