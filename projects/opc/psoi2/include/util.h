#ifndef util_h_
#define util_h_
#include "frl_types.h"
#include "opc/frl_opc_da_server.h"

namespace util
{
	void setServerInfo( frl::opc::DAServer &toServer );
	void regServer();
	void unregServer();
	bool lineHelper( const frl::String &line );
	bool isAlreadyRunning();
}

#endif // util_h_
