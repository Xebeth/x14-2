/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Test hooks
**************************************************************************/
#ifndef __TEST_HOOK_H__
#define __TEST_HOOK_H__

// int __thiscall sub_570F30(void *this, unsigned __int16 a2, int a3, int a4, __time64_t *ArgList)
typedef int (WINAPI *fn570F30)(LPVOID pThis_in_out, unsigned __int16 a2, int a3, int a4, __time64_t *ArgList);
int WINAPI sub570F30Hook(LPVOID pThis_in_out, unsigned __int16 a2, int a3, int a4, __time64_t *ArgList);

#endif//__TEST_HOOK_H__