#ifndef frl_os_win32_com_uuidof_h_
#define frl_os_win32_com_uuidof_h_

#if ! ( defined __uuidof )
#define __uuidof( interface_ ) IID_##interface_
#endif

#endif // frl_os_win32_com_uuidof_h_
