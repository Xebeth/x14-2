/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Test hooks
**************************************************************************/
#ifndef __TEST_HOOK_H__
#define __TEST_HOOK_H__

// char __thiscall sub_98B710(int this, int a2, int a3, int a4, int a5)
typedef char(WINAPI *fn98B710)(LPVOID pThis, int a2, int a3, int a4, int a5);
char WINAPI sub98B710Hook(LPVOID pThis, int a2, int a3, int a4, int a5);

// void __usercall sub_5AEB30(int a1<ecx>, int a2<ebp>)
void WINAPI sub5AEB30NakedHook();

#endif//__TEST_HOOK_H__