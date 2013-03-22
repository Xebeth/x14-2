/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Test hooks
**************************************************************************/
#ifndef __TEST_HOOK_H__
#define __TEST_HOOK_H__

// int __thiscall sub_5E0DF0(int this, const char *pText, unsigned int TextLen)
typedef int(WINAPI *fn5E0DF0)(LPVOID pThis, char *pText, int TextLen);
int WINAPI sub5E0DF0Hook(LPVOID pThis, char *pText, int TextLen);

// int __thiscall sub_490020(void *this, char *pText)
typedef int (WINAPI *fn490020)(LPVOID pThis, char *pText);
int WINAPI sub490020Hook(LPVOID pThis, char *pText);

// int __thiscall sub_5668C0(void *this, char *pText, int TextLen, signed int a4)
typedef int (WINAPI *fn5668C0)(LPVOID pThis, char *pText, int TextLen, signed int a4);
int WINAPI sub5668C0Hook(LPVOID pThis, char *pText, int TextLen, signed int a4);

// bool __thiscall sub_A73810(void *this, int a2)
typedef bool (WINAPI *fnA73810)(LPVOID pThis, char *pText);
bool WINAPI subA73810Hook(LPVOID pThis, char *pText);

// int __thiscall sub_8FDF60(int this, int a2, const char *a3, int a4)
typedef int (WINAPI *fn8FDF60)(LPVOID pThis, int a2, const char *a3, int a4);
int WINAPI sub8FDF60Hook(LPVOID pThis, int a2, const char *a3, int a4);

#endif//__TEST_HOOK_H__