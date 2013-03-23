/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Test hooks
**************************************************************************/
#ifndef __TEST_HOOK_H__
#define __TEST_HOOK_H__

// int __thiscall sub_5E3810(int this, const char *pText, unsigned int TextLen)
typedef int(WINAPI *fn5E3810)(LPVOID pThis, char *pText, int TextLen);
int WINAPI sub5E3810Hook(LPVOID pThis, char *pText, int TextLen);

// int __thiscall sub_48F5B0(void *this, char *pText)
typedef int (WINAPI *fn48F5B0)(LPVOID pThis, char *pText);
int WINAPI sub48F5B0Hook(LPVOID pThis, char *pText);

// int __thiscall sub_568B90(void *this, char *pText, int TextLen, signed int a4)
typedef int (WINAPI *fn568B90)(LPVOID pThis, char *pText, int TextLen, signed int a4);
int WINAPI sub568B90Hook(LPVOID pThis, char *pText, int TextLen, signed int a4);

// bool __thiscall sub_A73BE0(void *this, int a2)
typedef bool (WINAPI *fnA73BE0)(LPVOID pThis, char *pText);
bool WINAPI subA73BE0Hook(LPVOID pThis, char *pText);

// int __thiscall sub_8FFDA0(int this, int a2, const char *a3, int a4)
typedef int (WINAPI *fn8FFDA0)(LPVOID pThis, int a2, const char *a3, int a4);
int WINAPI sub8FFDA0Hook(LPVOID pThis, int a2, const char *a3, int a4);

// int __thiscall sub_48F4B0(void *this, int a2, int a3)
typedef int (WINAPI *fn48F4B0)(LPVOID pThis, char *pText, int TextLen);
int WINAPI sub48F4B0Hook(LPVOID pThis, char *pText, int TextLen);

#endif//__TEST_HOOK_H__