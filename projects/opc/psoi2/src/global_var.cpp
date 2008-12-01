#include <map>
#include "global_var.h"
#include "channel_desc.h"

namespace global_var
{
	namespace main_wnd
	{
		const TCHAR class_name[] = FRL_STR("psoi2_main_window");
		const TCHAR title[] = FRL_STR("PSOI2 OPC");
		HINSTANCE h_instance = NULL;
		HWND handle = NULL;
		HWND tree_handle = NULL;

		const int height = 425;
		const int width = 225;
	}
	std::map< HTREEITEM, ChannelDescr > channels_map;
}
