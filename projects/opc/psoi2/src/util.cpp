#include <Windows.h>
#include "opc/frl_opc_server_types.h"
#include "global_var.h"
#include "util.h"

namespace util
{

void regServer()
{
	frl::opc::DAServer tmp( frl::opc::ServerTypes::localSever32 );
	setServerInfo( tmp );
	tmp.registrerServer3();
}

void unregServer()
{
	frl::opc::DAServer tmp( frl::opc::ServerTypes::localSever32 );
	setServerInfo( tmp );
	tmp.unregisterServer();
}

void setServerInfo( frl::opc::DAServer &toServer )
{
	toServer.setCLSID( FRL_STR("{77841F96-2135-4293-8BEF-A288AAD2DBBC}") );
	toServer.setVendor( FRL_STR("Serg Baburin") );
	toServer.setDriverName( FRL_STR("PSOI2 device") );
	toServer.setDescription( FRL_STR("OPC server for PSOI2 device from Serg Baburin"));
	toServer.setVersion( 0.1 );
}

bool lineHelper( const frl::String &line )
{
	if( line.length() )
	{
		if( frl::String( FRL_STR("-r") ) == line )
		{
			try
			{
				regServer();
				MessageBox( NULL, FRL_STR("Registration success!"), FRL_STR("PSOI2 OPC server: Registration message"), MB_OK | MB_ICONINFORMATION );
			}
			catch( frl::Exception &ex )
			{
				frl::String msg = FRL_STR("Registration fail!\n");
				msg += ex.getFullDescription();
				MessageBox( NULL, msg.c_str(), FRL_STR("PSOI2 OPC server: Registration message"), MB_OK | MB_ICONSTOP );
			}
			catch( ... )
			{
				MessageBox( NULL, FRL_STR("Registration fail!\nUnknown error"), FRL_STR("PSOI2 OPC server: Registration message"), MB_OK | MB_ICONSTOP );
			}
			return true;
		}

		if( frl::String( FRL_STR("-u") ) == line )
		{
			try
			{
				unregServer();
				MessageBox( NULL, FRL_STR("Removing registration success!"), FRL_STR("PSOI2 OPC server: Registration message"), MB_OK | MB_ICONINFORMATION );
			}
			catch( frl::Exception &ex )
			{
				frl::String msg = FRL_STR("Unregistration fail!\n");
				msg += ex.getFullDescription();
				MessageBox( NULL, msg.c_str(), FRL_STR("Registration message"), MB_OK | MB_ICONSTOP );
			}
			catch( ... )
			{
				MessageBox( NULL, FRL_STR("Unregistration fail!\nUnknown error"), FRL_STR("PSOI2 OPC server: Registration message"), MB_OK | MB_ICONSTOP );
			}
			return true;
		}

		if( frl::String( FRL_STR("-Embedding") ) != line )
		{
			MessageBox( NULL,
				FRL_STR("Available parameters:\n\"-r\" - Registration server.\n\"-u\"- Unregistration server."),
				FRL_STR("PSOI2 OPC server: Registration message"),
				MB_OK | MB_ICONHAND );
			return true;
		}
	}
	return false;
}

bool isAlreadyRunning()
{
	HWND tmp_handle = ::FindWindow( global_var::main_wnd::class_name, global_var::main_wnd::title );
	if( tmp_handle )
	{
		if( ::IsIconic( tmp_handle ) )
			ShowWindow( tmp_handle, SW_RESTORE );
		SetForegroundWindow( tmp_handle );
		return true;
	}
	return false;
}

} // namespace util
