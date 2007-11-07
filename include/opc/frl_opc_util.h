#ifndef frl_opc_util_h_
#define frl_opc_util_h_
#include "frl_platform.h"
#if ( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"
#include "frl_types.h"

namespace frl
{
	namespace opc
	{
		namespace util
		{
			OPCHANDLE getUniqueServerHandle();	
		} // namespace util
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_util_h_ */
