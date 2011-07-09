/**************************************************************************
	created		:	2011-06-03
	filename	: 	CreateWindowExHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	CreateWindowEx hook definitions
**************************************************************************/
#ifndef __CREATE_WINDOW_EX_HOOK_H__
#define __CREATE_WINDOW_EX_HOOK_H__

typedef HWND (WINAPI *fnCreateWindowExA)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, 
										 int nWidth, int nHeight,HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
HWND WINAPI CreateWindowExAHook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, 
								int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

#endif//__CREATE_WINDOW_EX_HOOK_H__