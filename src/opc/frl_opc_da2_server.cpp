#include "opc/frl_opc_da2_server.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "../dependency/vendors/opc_foundation/opcda.h"

namespace frl
{
	namespace opc
	{
		DA2Server::DA2Server( const ServerType& serverType )
			:	ServerKind( serverType )
		{
		}

		void DA2Server::registrerServer()
		{
			ServerKind::registrerServer( ::CATID_OPCDAServer20 );
		}

	} // namespace opc
} // namespace FatRat Library

#endif /* FRL_PLATFORM_WIN32 */
