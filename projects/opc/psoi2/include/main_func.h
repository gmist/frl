#ifndef main_func_h_
#define main_func_h_
#include <Windows.h>

// Saves instance handle and creates main window
BOOL initInstance(HINSTANCE hInstance, int nCmdShow);

// Helper for registration main window class
BOOL regClassHelper( HINSTANCE hInstance );

// Processes messages for the main window
LRESULT CALLBACK mainFunc( HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param );

// Function of creating a timer to checking connection of clients
void createCheckConnectionTimer();

#endif // main_func_h_
