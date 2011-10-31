/**************************************************************************
	created		:	2011-07-03
	filename	: 	CreateWindowExHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	CreateWindowEx hook definitions
**************************************************************************/
#ifndef __CREATE_WINDOW_EX_HOOK_H__
#define __CREATE_WINDOW_EX_HOOK_H__

typedef HWND (WINAPI *fnCreateWindowExW)(DWORD dwExStyle_in, LPCTSTR lpClassName_in, LPCTSTR lpWindowName_in, DWORD dwStyle_in, int X_in, int Y_in, 
										 int nWidth_in, int nHeight_in, HWND hWndParent_in, HMENU hMenu_in, HINSTANCE hInstance_in, LPVOID lpParam_in);
HWND WINAPI CreateWindowExWHook(DWORD dwExStyle_in, LPCTSTR lpClassName_in, LPCTSTR lpWindowName_in, DWORD dwStyle_in, int X_in, int Y_in, 
							    int nWidth_in, int nHeight_in, HWND hWndParent_in, HMENU hMenu_in, HINSTANCE hInstance_in, LPVOID lpParam_in);

#endif//__CREATE_WINDOW_EX_HOOK_H__