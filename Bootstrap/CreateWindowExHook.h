/**************************************************************************
	created		:	2011-07-03
	filename	: 	CreateWindowExHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	CreateWindowEx hook definitions
**************************************************************************/
#ifndef __CREATE_WINDOW_EX_HOOK_H__
#define __CREATE_WINDOW_EX_HOOK_H__

typedef HWND (WINAPI *fnCreateWindowExW)(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int X, int Y, 
										int nWidth, int nHeight,HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
HWND WINAPI CreateWindowExWHook(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int X, int Y, 
							   int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

#endif//__CREATE_WINDOW_EX_HOOK_H__