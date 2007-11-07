#include "opc/frl_opc_util.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )

namespace frl
{
	namespace opc
	{
		namespace util
		{
			OPCHANDLE getUniqueServerHandle()
			{
				static OPCHANDLE handle = 0;
				return ++handle;
			}
		}	// namespace util
	} // namespace opc
} // // namespace FatRat Library


#endif /* FRL_PLATFORM_WIN32 */
