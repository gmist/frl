#include "splash_func.h"
#include "resource.h"
#include "global_var.h"

BOOL CALLBACK splashFunc( HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param )
{
	const static DWORD ID_SPLASH_TIMER = 10000;
	static HBITMAP bit_map_handle = NULL;
	switch( msg )
	{ 	 
		case WM_TIMER:
		{
			if( w_param == ID_SPLASH_TIMER )
			{
				::KillTimer( hwnd, ID_SPLASH_TIMER );
				if( bit_map_handle != NULL )
					::DeleteObject( bit_map_handle );
				::EndDialog( hwnd, 0 );
			}
		}
		break;

		case WM_INITDIALOG: 
		{
			::SetTimer( hwnd, ID_SPLASH_TIMER, 2000, (TIMERPROC)NULL );
		}
		break;
	}
	return 0;
}

void showSplashScreen( HWND hwnd )
{
	DialogBoxParam( global_var::main_wnd::h_instance, MAKEINTRESOURCE( IDD_SPLASH_DIALOG ), hwnd, (DLGPROC)splashFunc, 0 );
}
