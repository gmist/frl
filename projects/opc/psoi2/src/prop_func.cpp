#include <Windows.h>
#include "psoi2_device.h"
#include "global_var.h"
#include "prop_func.h"
#include "resource.h"

BOOL CALLBACK propFunc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
		case WM_INITDIALOG: 
		{
			HTREEITEM Selected = (HTREEITEM)lParam;
			std::map< HTREEITEM, ChannelDescr >::iterator it = global_var::channels_map.find( Selected );
			if( it == global_var::channels_map.end() )
			{
				MessageBox( hWnd, FRL_STR("Channel not found!"), FRL_STR("PSOI2 OPC server: Channel property"), MB_OK | MB_ICONSTOP );
				EndDialog( hWnd,0 );
				return 0;
			}

			const Psoi2Device* dev = global_var::devManager::getInstance().getDevice( (*it).second.portNumber );
			frl::UInt channel_number = (*it).second.channelNumber;
			
			frl::String tmp = FRL_STR("Channel #");
			tmp += frl::lexicalCast< frl::UInt, frl::String >( channel_number );
			HWND tmp_hwnd = GetDlgItem( hWnd, IDC_CHANNEL_N );
			::SetWindowText( tmp_hwnd, tmp.c_str() );

			tmp = frl::lexicalCast< float, frl::String >( dev->getValue( channel_number ) );
			tmp_hwnd = ::GetDlgItem( hWnd, IDC_EDIT_VALUE );
			::SetWindowText( tmp_hwnd, tmp.c_str() );

			frl::Bool type_tmp = dev->getTypePPC( channel_number );
			if( type_tmp )
				tmp = FRL_STR( "PPC2" );
			else
				tmp = FRL_STR( "PPC1" );
			tmp_hwnd = ::GetDlgItem( hWnd, IDC_EDIT_TYPE );
			::SetWindowText( tmp_hwnd, tmp.c_str() );

			type_tmp = dev->getGoodPPC( channel_number );
			if( type_tmp )
				tmp = FRL_STR( "Good" );
			else
				tmp = FRL_STR( "Bad" );
			tmp_hwnd = ::GetDlgItem( hWnd, IDC_EDIT_PPC );
			::SetWindowText( tmp_hwnd, tmp.c_str() );

			type_tmp = dev->getGoodMGC( channel_number );
			if( type_tmp )
				tmp = FRL_STR( "Good" );
			else
				tmp = FRL_STR( "Bad" );
			tmp_hwnd = ::GetDlgItem( hWnd, IDC_EDIT_MGC );
			::SetWindowText( tmp_hwnd, tmp.c_str() );

			type_tmp = dev->getThresholdExceeding( channel_number );
			if( type_tmp )
				tmp = FRL_STR( "Exceeding" );
			else
				tmp = FRL_STR( "Normal" );
			tmp_hwnd = ::GetDlgItem( hWnd, IDC_EDIT_CNC );
			::SetWindowText( tmp_hwnd, tmp.c_str() );
			
		}
		break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{ 	 
				case IDC_BUTTON_CLOSE:
				{
					EndDialog( hWnd,0 );
				}
				break;
			}
		}
		break;
	}
	return 0;
}
