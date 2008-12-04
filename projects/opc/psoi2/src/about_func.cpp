#include "about_func.h"
#include "frl_types.h"
#include "resource.h"
#include "os/win32/registry/frl_registry_Key.h"

BOOL CALLBACK aboutFunc( HWND hWnd, UINT msg, WPARAM w_param, LPARAM l_param )
{
	switch( msg )
	{ 	 
		case WM_INITDIALOG: 
		{
			return 0;
		}

		case WM_COMMAND:
		{
			switch( LOWORD(w_param) )
			{ 	 
				case IDC_BUTTON_TO_WEB:
				{
					static frl::Char url[] = FRL_STR("http://code.google.com/p/frl/");
					HINSTANCE result = ShellExecute(NULL, FRL_STR("open"), url, NULL,NULL, SW_SHOWNORMAL);

					if ((UINT)result <= HINSTANCE_ERROR) // oopps.... executing failed
					{
						// finding default browser for .htm files
						frl::os::win32::registry::Key htm_key( FRL_STR(".htm"), frl::os::win32::registry::RootKeys::classesRoot );
						frl::String app = htm_key.getStringValue();

						// find open command and preparation command to execute
						app += FRL_STR("\\shell\\open\\command");
						htm_key.reinit( app, frl::os::win32::registry::RootKeys::classesRoot );
						app = htm_key.getStringValue();
						size_t pos = app.find( '"', 1 );

						if( pos == frl::String::npos )
							return 0; // unknown error (incorrect command?) - silent exit

						app = app.substr( 0, pos );
						app += FRL_STR("\" \"");
						app += url;
						app += FRL_STR('\"');

						// execute default browser
						WinExec( frl::multiByteCompatibility( app ).c_str(), SW_SHOWNORMAL );
					}
					return 0;
				}
				break; // IDC_BUTTON_TO_WEB

				case ID_ABOUT_CLOSE:
				{
					EndDialog( hWnd,0 );
					return 0;
				}
				break; // ID_ABOUT_CLOSE
			}
		}
		break;
	}
	return 0;
}
