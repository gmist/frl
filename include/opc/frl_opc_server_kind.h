#ifndef frl_opc_server_kind_h_
#define frl_opc_server_kind_h_
#include "frl_platform.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include <vector>
#include "frl_types.h"
#include "frl_lexical_cast.h"
#include "frl_type_to_type.h"
#include "opc/frl_opc_server_type.h"

namespace frl{ namespace opc{

class ServerKind
{
private:
	String progID;
	String clsid;
	String vendor;
	String description;
	String driverName;
	String version;
	ServerType type;
protected:
	void registrerServer( const std::vector<IID> &categories );
public:
	ServerKind( const ServerType& serverType );
	void setProgID( const String &newProgID );
	void setCLSID( const String &newCLSID );
	void setVendor( const String &newVendor );
	void setDescription( const String &newDescription );
	void setDriverName( const String &newDriverName );
	void setVersion( const String &newVersion );
	void setVersion( double newVersion );
	void setType( const ServerType &newType );

	const String& getProgID();
	const String& getCLSID();
	const String& getVendor();
	const String& getDescription();
	const String& getDriverName();
	const String& getVersion();
	double getVersion( TypeToType<double> )
	{
		return lexicalCast<String, double>( version );
	}
	const ServerType& getType();
	void unregisterServer();
};
} // namespace opc
} // FatRat Library

#endif // FRL_PLATFORM_WIN32
#endif /* frl_opc_server_kind_h_ */
