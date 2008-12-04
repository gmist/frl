#include <Windows.h>
#include <commctrl.h>
#include <boost/thread.hpp>
#include "main_func.h"
#include "global_var.h"
#include "resource.h"
#include "psoi2_device.h"
#include "prop_func.h"
#include "splash_func.h"
#include "about_func.h"

#define MSG_NOTIFYICON (WM_APP) + 100
#define WM_MENU_ABOUT (WM_APP) + 201
#define WM_MENU_EXIT (WM_APP) + 202

// Helper for adding items to menu
BOOL createMenuHelper( HMENU menu_handle, frl::Char *data, UINT pos, UINT id, HMENU sub_menu, UINT type )
{
	MENUITEMINFO mii;
	mii.fMask = MIIM_STATE | MIIM_TYPE | MIIM_SUBMENU | MIIM_ID;
	if( type != MFT_SEPARATOR )
	{
		mii.dwTypeData = data;
		mii.cch = sizeof( data );
	}
	mii.fType = type;
	mii.cbSize = sizeof( MENUITEMINFO );
	mii.fState = MFS_ENABLED;
	mii.wID = id;
	mii.hSubMenu = sub_menu;
	return ::InsertMenuItem( menu_handle, pos, FALSE, &mii );
}

// Helper for show/hide icon in system tray
void systrayIconHelper( HINSTANCE hinstance, HWND hwnd, bool add )
{
	NOTIFYICONDATA nid;
	::ZeroMemory( &nid,sizeof(nid) );
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hwnd;
	nid.uID = 0;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = MSG_NOTIFYICON;
	nid.hIcon = LoadIcon( hinstance, MAKEINTRESOURCE(IDI_ICON_MAIN) );
	lstrcpy( nid.szTip,TEXT("Double-click to maxmize!") );

	if( add )
		::Shell_NotifyIcon( NIM_ADD, &nid );
	else
		::Shell_NotifyIcon( NIM_DELETE, &nid );
}

// Helper for create tree view with channels list
void createTreeView( HWND hwnd )
{
	global_var::main_wnd::tree_handle = CreateWindowEx( 
		WS_EX_WINDOWEDGE,
		WC_TREEVIEW,
		NULL,
		WS_CHILD+WS_VISIBLE+TVS_HASLINES+TVS_HASBUTTONS+TVS_LINESATROOT,
		5, 5,
		210,
		410, 
		hwnd,
		(HMENU)ID_MAIN_TREE,
		global_var::main_wnd::h_instance, NULL);

	HIMAGELIST image_list_handle = ::ImageList_Create( 16,16,ILC_COLOR24,3, 10 );
	HBITMAP bit_map_handle = ::LoadBitmap( global_var::main_wnd::h_instance, MAKEINTRESOURCE(IDB_TREE_LIST) );
	::ImageList_Add(image_list_handle,bit_map_handle,NULL);
	::DeleteObject( bit_map_handle );
	::SendDlgItemMessage( hwnd, ID_MAIN_TREE, TVM_SETIMAGELIST, 0, (LPARAM)image_list_handle );

	std::vector< Psoi2Device* >::const_iterator end = global_var::devManager::getInstance().getDevices().end();
	std::vector< Psoi2Device* >::const_iterator it = global_var::devManager::getInstance().getDevices().begin();

	// temporary variables
	TV_INSERTSTRUCT tv_struct;
	HTREEITEM parent = 0;
	HTREEITEM item_handle = 0;
	frl::String ch_tmp;

	for( ; it != end; ++it )
	{
		frl::String tpm = FRL_STR("COM_");
		tpm += frl::lexicalCast< frl::UInt, frl::String >( (*it)->getPortNumber() );
		tpm += FRL_STR(": ");

		tpm += frl::lexicalCast< frl::UInt, frl::String >( (*it)->getChannelsNumber() );
		tpm += FRL_STR(" channels");

		if( (*it)->isSimulation() )
			tpm += FRL_STR(" (simulation!)");

		tv_struct.hParent=NULL;
		tv_struct.hInsertAfter=TVI_ROOT;
		tv_struct.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		tv_struct.item.pszText= (frl::Char*)tpm.c_str();
		tv_struct.item.iImage=0;
		tv_struct.item.iSelectedImage = 0;
		parent = (HTREEITEM)SendDlgItemMessage( hwnd, ID_MAIN_TREE, TVM_INSERTITEM, 0, (LPARAM)&tv_struct );

		for( frl::UInt i = 0; i < (*it)->getChannelsNumber(); ++i )
		{
			tv_struct.hParent=parent;
			tv_struct.hInsertAfter=TVI_LAST;
			ch_tmp = FRL_STR( "Cannel_" );
			ch_tmp += frl::lexicalCast< frl::Int, frl::String >( i );
			tv_struct.item.pszText=(frl::Char*)ch_tmp.c_str();
			tv_struct.item.iImage=2;
			tv_struct.item.iSelectedImage=1;
			item_handle = (HTREEITEM) ::SendDlgItemMessage( hwnd, ID_MAIN_TREE, TVM_INSERTITEM,0,(LPARAM)&tv_struct);
			global_var::channels_map.insert( std::make_pair( item_handle, ChannelDescr( (*it)->getPortNumber(), i ) ) );
		}
	}
}

BOOL regClassHelper( HINSTANCE hInstance )
{
	WNDCLASS wc;
	wc.hbrBackground	= (HBRUSH)( COLOR_WINDOW );
	wc.cbClsExtra = wc.cbWndExtra = 0;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = mainFunc;
	wc.hInstance = hInstance;
	wc.lpszClassName = global_var::main_wnd::class_name;
	wc.lpszMenuName = NULL;
	wc.hIcon = ::LoadIcon( hInstance, (LPCTSTR)IDI_ICON_MAIN );
	wc.hCursor = ::LoadCursor( NULL, IDC_ARROW );
	return RegisterClass( &wc );
}

// Function for create main window
BOOL createWindow()
{
	int x_screen = ::GetSystemMetrics( SM_CXSCREEN );
	int y_screen = ::GetSystemMetrics( SM_CYSCREEN );
	y_screen = y_screen - ::GetSystemMetrics( SM_CYMENU );

	x_screen = x_screen - global_var::main_wnd::width - 10;
	y_screen = y_screen - global_var::main_wnd::height - 10;

	global_var::main_wnd::handle = CreateWindow( 
		global_var::main_wnd::class_name, 
		global_var::main_wnd::title,
		WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		x_screen,
		y_screen,
		global_var::main_wnd::width,
		global_var::main_wnd::height,
		NULL,
		NULL,
		global_var::main_wnd::h_instance,
		NULL );

	if( ! global_var::main_wnd::handle )
		return FALSE;

	return TRUE;
}

BOOL initInstance( HINSTANCE hInstance, int nCmdShow )
{
	global_var::main_wnd::h_instance = hInstance;

	if( ! createWindow() )
		return FALSE;

	::ShowWindow( global_var::main_wnd::handle, nCmdShow );

	if( nCmdShow == SW_HIDE )
		systrayIconHelper( global_var::main_wnd::h_instance, global_var::main_wnd::handle, true );
	else
		::UpdateWindow( global_var::main_wnd::handle );

	return TRUE;
}

LRESULT CALLBACK mainFunc( HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param )
{
	switch( msg )
	{
		case WM_SIZE:
		{
			if( w_param == SIZE_MINIMIZED )
			{
				systrayIconHelper( global_var::main_wnd::h_instance, hwnd, true );
				::ShowWindow( hwnd, SW_HIDE );
			}
		}
		break; // WM_SIZE

		case MSG_NOTIFYICON:
		{
			if( l_param ==  WM_LBUTTONDBLCLK )
			{
				systrayIconHelper( global_var::main_wnd::h_instance, hwnd, false );
				::ShowWindow( hwnd, SW_RESTORE );
				::SetForegroundWindow( hwnd );
			}
		}
		break; // MSG_NOTIFYICON

		case WM_CREATE:
		{
			// show splash screen
			boost::function < void ( HWND ) > splash_func = boost::bind( &showSplashScreen, _1 );
			boost::thread processThread = boost::thread( boost::bind( splash_func, hwnd ) );

			InitCommonControls();

			// Create tree view
			createTreeView(hwnd);

			// Create main window menu
			HMENU menu_handle = ::CreateMenu();
			HMENU menu_server_handle = ::CreatePopupMenu();

			// Insert menu "Server" items
			createMenuHelper( menu_server_handle, FRL_STR("&About"), 0, WM_MENU_ABOUT, NULL, MFT_STRING );
			createMenuHelper( menu_server_handle, NULL, 1, 0, NULL, MFT_SEPARATOR );
			createMenuHelper( menu_server_handle, FRL_STR("&Exit"), 2, WM_MENU_EXIT, NULL, MFT_STRING );

			// Add menu "Server" to main menu
			createMenuHelper( menu_handle, FRL_STR("&Server"), 0, 0, menu_server_handle, MFT_STRING );

			// And show main menu
			::SetMenu( hwnd, menu_handle );
			::DrawMenuBar( hwnd );

			return 0;
		}
		break; // WM_CREATE

		case WM_CLOSE:
		{
			int ret_val = ::MessageBox( hwnd, FRL_STR("Do you really want to exit?"), FRL_STR("PSOI2 OPC server: Confirmation message"), MB_ICONQUESTION | MB_YESNO );
			if( ret_val == IDNO )
			{
				return 0;
			}
		}
		break; // WM_CLOSE

		case WM_DESTROY:
		{
			::PostQuitMessage( 0 );
			return 0;
		}
		break; // WM_DESTROY

		case WM_NOTIFY:
		{
			case ID_MAIN_TREE:
			{
				if( ( (LPNMHDR)l_param )->code == (UINT)NM_DBLCLK )
				{
					using namespace global_var;
					HTREEITEM Selected = NULL;
					Selected=(HTREEITEM) ::SendDlgItemMessage( hwnd, ID_MAIN_TREE, TVM_GETNEXTITEM, TVGN_CARET, (LPARAM)Selected );
					DialogBoxParam( global_var::main_wnd::h_instance, MAKEINTRESOURCE( IDD_CHANNEL_PROP ), hwnd, (DLGPROC)propFunc, (LPARAM)Selected );
					return 0;
				}
			}
			return 0;
		}
		break; // WM_NOTIFY

		case WM_COMMAND:
		{
			switch( LOWORD( w_param ) )
			{
				case WM_MENU_ABOUT:
				{
					DialogBoxParam( global_var::main_wnd::h_instance, MAKEINTRESOURCE( IDD_ABOUT_DIALOG ), hwnd, (DLGPROC)aboutFunc, 0 );
					return 0;
				}
				break; // WM_MENU_ABOUT

				case WM_MENU_EXIT:
				{
					::SendMessage( hwnd, WM_CLOSE, 0, 0 );
					return 0;
				}
				break; // WM_MENU_EXIT
			}
			return 0;
		}
		break; // WM_COMMAND
	}
	return ::DefWindowProc( hwnd, msg, w_param, l_param );
}
