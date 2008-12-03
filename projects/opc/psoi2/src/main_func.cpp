#include <Windows.h>
#include <commctrl.h>
#include <boost/thread.hpp>
#include "main_func.h"
#include "global_var.h"
#include "resource.h"
#include "psoi2_device.h"
#include "prop_func.h"
#include "splash_func.h"

#define MSG_NOTIFYICON (WM_APP) + 100

void systrayIcon( HINSTANCE hinstance, HWND hwnd, bool add )
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


BOOL regClass( HINSTANCE hInstance )
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
		WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU | WS_CAPTION,
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
		systrayIcon( global_var::main_wnd::h_instance, global_var::main_wnd::handle, true );
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
				systrayIcon( global_var::main_wnd::h_instance, hwnd, true );
				::ShowWindow( hwnd, SW_HIDE );
			}
		}
		break;

		case MSG_NOTIFYICON:
		{
			if( l_param ==  WM_LBUTTONDBLCLK )
			{
				systrayIcon( global_var::main_wnd::h_instance, hwnd, false );
				::ShowWindow( hwnd, SW_RESTORE );
				::SetForegroundWindow( hwnd );
			}
		}
		break;

		case WM_CREATE:
		{
			// show splash screen
			boost::function < void ( HWND ) > splash_func = boost::bind( &showSplashScreen, _1 );
			boost::thread processThread = boost::thread( boost::bind( splash_func, hwnd ) );

			InitCommonControls();

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
				global_var::main_wnd::h_instance, NULL); // Создание tree view

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

			return 0;
		}
		break;

		case WM_CLOSE:
		{
			int ret_val = ::MessageBox( hwnd, FRL_STR("Do you really want to exit?"), FRL_STR("PSOI2 OPC server: Confirmation message"), MB_ICONQUESTION | MB_YESNO );
			if( ret_val == IDNO )
			{
				return 0;
			}
		}
		break;

		case WM_DESTROY:
		{
			::PostQuitMessage( 0 );
			return 0;
		}
		break;

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
		break;

		case TVN_ITEMEXPANDED:
		{
			return 0;
		}
		break;
	}
	return ::DefWindowProc( hwnd, msg, w_param, l_param );
}
