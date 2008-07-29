#ifndef frl_opc_da2_server_h_
#define frl_opc_da2_server_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "frl_types.h"
#include "opc/frl_opc_server_kind.h"

namespace frl{ namespace opc{

class DAServer
	:	public ServerKind
{
private:
	Bool isInit;
	DWORD objectId;
public:
	DAServer( const ServerType& serverType );
	~DAServer();
	void registrerServer();
	void registrerServer3();
	void unregisterServer();
	Bool init();
};

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_da2_server_h_
