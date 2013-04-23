/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Test hooks
**************************************************************************/
#ifndef __TEST_HOOK_H__
#define __TEST_HOOK_H__

// int __thiscall sub_5D1C70(int this, int a2, int a3, int a4, int a5) -- ObjectInterface::Invoke?

typedef struct _StringNode StringNode;

// int __thiscall sub_5E9E30(int this, int a2)
typedef int (WINAPI *fnSub5E9E30)(LPVOID pThis, StringNode * a2);
int WINAPI Sub5E9E30Hook(LPVOID pThis, StringNode * a2);

// int __thiscall sub_5D1C70(int this, int a2, int a3, int a4, int a5)
typedef int (WINAPI *fnSub5D1C70)(LPVOID pThis, DWORD a2, char *a3, int a4, int a5);
int WINAPI Sub5D1C70Hook(LPVOID pThis, DWORD a2, char *a3, int a4, int a5);

#endif//__TEST_HOOK_H__