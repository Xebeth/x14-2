/**************************************************************************
	created		:	2011-06-03
	filename	: 	WndHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	SetWindowSubclass hook definitions
**************************************************************************/
#ifndef __CREATE_WINDOW_EX_HOOK_H__
#define __CREATE_WINDOW_EX_HOOK_H__

typedef LRESULT (CALLBACK *SUBCLASSPROC)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
typedef BOOL (WINAPI *fnSetWindowSubclass)(HWND hWnd_in, SUBCLASSPROC pfnSubclass_in, UINT_PTR uIdSubclass_in, DWORD_PTR dwRefData_in);
typedef ATOM (WINAPI *fnRegisterClassExA)(const WNDCLASSEXA *pWndClass_in);

#endif//__CREATE_WINDOW_EX_HOOK_H__