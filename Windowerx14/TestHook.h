/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Test hooks
**************************************************************************/
#ifndef __TEST_HOOK_H__
#define __TEST_HOOK_H__

// int __thiscall sub_4357A0(int this, int a2, int a3, int a4, int a5) -- ObjectInterface::Invoke?
typedef int (WINAPI *fnSub4357A0)(LPVOID pThis, DWORD a2, char *a3, int a4, int a5);
int WINAPI Sub4357A0Hook(LPVOID pThis, DWORD a2, char *a3, int a4, int a5);

#endif//__TEST_HOOK_H__