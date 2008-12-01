#ifndef main_func_h_
#define main_func_h_
#include <Windows.h>

// Saves instance handle and creates main window
BOOL initInstance(HINSTANCE hInstance, int nCmdShow);

// Registers the window class.
BOOL regClass( HINSTANCE hInstance );

// Processes messages for the main window.
LRESULT CALLBACK mainFunc( HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param );

#endif // main_func_h_
