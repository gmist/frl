#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "opc/frl_opc_server_type.h"

namespace frl{ namespace opc{

ServerType::ServerType( const String &type_name ) : name( type_name )
{
}

ServerType::ServerType( const ServerType &rhv ) : name( rhv.name )
{
}

const String& ServerType::getType()
{
	return name;
}

ServerType& ServerType::operator=( const ServerType &rhv )
{
	if( this != &rhv )
		name = rhv.name;
	return *this;
}

frl::Bool ServerType::operator==( const ServerType &rhv )
{
	return ( this == &rhv ) || ( name == rhv.name );
}

} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM
