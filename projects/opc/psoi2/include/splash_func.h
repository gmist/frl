#ifndef splash_func_h_
#define splash_func_h_
#include <Windows.h>

// Processes messages for the splash screen window
BOOL CALLBACK splashFunc( HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param );

// Call adapter for the splash screen window
void showSplashScreen( HWND hwnd );

#endif // splash_func_h_
