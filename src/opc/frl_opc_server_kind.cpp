#include "opc/frl_opc_server_kind.h"
#if( FRL_PLATFORM == FRL_PLATFORM_WIN32 )
#include "frl_lexical_cast.h"
#include "os/win32/registry/frl_registry_Key.h"

namespace frl
{
namespace opc
{
ServerKind::ServerKind( const ServerType& serverType )
	:	type( serverType )
{

}

void ServerKind::setProgID( const String &newProgID )
{
	progID = newProgID;
}

void ServerKind::setCLSID( const String &newCLSID )
{
	clsid = newCLSID;
}

void ServerKind::setVendor( const String &newVendor )
{
	vendor = newVendor;
}

void ServerKind::setDescription( const String &newDescription )
{
	description = newDescription;
}

void ServerKind::setDriverName( const String &newDriverName )
{
	driverName = newDriverName;
}

void ServerKind::setVersion( const String &newVersion )
{
	version = newVersion;
}

void ServerKind::setVersion( double newVersion )
{
	version = lexicalCast<double, String >(newVersion);
}

void ServerKind::setType( const ServerType &newType )
{
	type = newType;
}

const String& ServerKind::getProgID()
{
	return progID;
}

const String& ServerKind::getCLSID()
{
	return clsid;
}

const String& ServerKind::getVendor()
{
	return vendor;
}

const String& ServerKind::getDescription()
{
	return description;
}

const String& ServerKind::getDriverName()
{
	return driverName;
}

const String& ServerKind::getVersion()
{
	return version;
}

const ServerType& ServerKind::getType()
{
	return type;
}

void ServerKind::registrerServer( const std::vector<IID> &categories )
{
	if( clsid.empty() || driverName.empty() || vendor.empty() || version.empty() || categories.empty() )
		FRL_THROW( FRL_STR( "Invalid server attributes" ) );
	const String vendDrvVers = vendor+FRL_STR(".") + driverName + FRL_STR(".") + version;
	frl::os::win32::registry::Key key( vendDrvVers, frl::os::win32::registry::RootKeys::classesRoot );
	key.create();
	key.setStringValue( description );
	key.reinit( vendDrvVers + FRL_STR("\\CLSID"), key.getRootKey() );
	key.create();
	key.setStringValue( clsid );
	key.reinit( vendDrvVers + FRL_STR("\\OPC\\Vendor"), key.getRootKey() );
	key.setStringValue( vendor );
	key.reinit( FRL_STR("CLSID\\") + clsid, key.getRootKey() );
	key.create();
	key.setStringValue( description );
	key.setStringValue( FRL_STR("AppID"), clsid );
	key.reinit( FRL_STR("CLSID\\") + clsid + FRL_STR("\\ProgID"), key.getRootKey() );
	key.setStringValue( vendDrvVers );
	key.reinit( FRL_STR("CLSID\\") + clsid + FRL_STR("\\") + type.getType(), key.getRootKey() );	
	key.create();
	TCHAR path[MAX_PATH] = {0};
	GetModuleFileName(NULL, path ,MAX_PATH);
	key.setStringValue( path );
	for( std::vector< IID >::const_iterator it = categories.begin(); it != categories.end(); ++it )
	{
		key.reinit(	FRL_STR("CLSID\\") +
			clsid +
			FRL_STR("\\" ) +
			FRL_STR("Implemented Categories\\") +
			lexicalCast<GUID, String >( (*it) ), key.getRootKey() );
	}
	key.create();
}

void ServerKind::unregisterServer()
{
	if( clsid.empty() || driverName.empty() || vendor.empty() || version.empty() )
		FRL_THROW( FRL_STR( "Invalid server attributes" ) );
	const String vendDrvVers = vendor+FRL_STR(".") + driverName + FRL_STR(".") + version;
	frl::os::win32::registry::Key key( vendDrvVers, frl::os::win32::registry::RootKeys::classesRoot );
	key.deleteKey( True );
	key.reinit( FRL_STR("CLSID\\") + clsid, key.getRootKey() );
	key.deleteKey( True );
}

} // namespace opc
} // FatRat Library

#endif //  FRL_PLATFORM_WIN32
