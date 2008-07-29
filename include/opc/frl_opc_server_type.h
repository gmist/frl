#ifndef frl_opc_server_type_h_
#define frl_opc_server_type_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "frl_types.h"

namespace frl{ namespace opc{

class ServerType
{
private:
	String name;
public:
	ServerType( const String& type_name );
	ServerType( const ServerType &rhv );
	const String& getType();
	ServerType& operator = ( const ServerType &rhv );
	Bool operator == ( const ServerType &rhv );
};

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif // frl_opc_server_type_h_
