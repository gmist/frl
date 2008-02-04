#ifndef frl_opc_da2_server_h_
#define frl_opc_da2_server_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "frl_types.h"
#include "opc/frl_opc_server_kind.h"

namespace frl
{
	namespace opc
	{
		class DA2Server
			:	public ServerKind
		{
		private:
		public:
			DA2Server( const ServerType& serverType );
			void registrerServer();
			Bool init();
		};
	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
#endif /* frl_opc_da2_server_h_ */
