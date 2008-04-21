#ifndef frl_opc_serv_handle_counter_h_
#define frl_opc_serv_handle_counter_h_
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "opc/frl_opc_util.h"

namespace frl
{
namespace opc
{
class ServerHandleCounter
{
private:
	OPCHANDLE serverHandle;
public:
	ServerHandleCounter()
		: serverHandle( util::getUniqueServerHandle() )
	{
	
	}

	OPCHANDLE getServerHandle() const
	{
		return serverHandle;
	}
};
} // namespace
} // FatRat Library

#endif // frl_opc_serv_handle_counter_h_
