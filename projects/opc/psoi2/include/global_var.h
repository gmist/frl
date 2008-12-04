#ifndef global_var_h_
#define global_var_h_
#include <Windows.h>
#include <CommCtrl.h>
#include <map>
#include "channel_desc.h"
#include "psoi2_device_manager.h"
#include "frl_singleton.h"

namespace global_var
{
	namespace main_wnd
	{
		extern const TCHAR class_name[];
		extern const TCHAR title[];
		extern HINSTANCE h_instance;
		extern HWND handle;
		extern HWND tree_handle;

		extern const int height;
		extern const int width;
	}
	
	extern Bool exit_if_all_client_disconnected;

	extern std::map< HTREEITEM, ChannelDescr > channels_map;
	typedef frl::SingletonMeyers< DeviceManager > devManager;
}

#endif // global_var_h_
