/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Test hooks
**************************************************************************/
#ifndef __TEST_HOOK_H__
#define __TEST_HOOK_H__

// int __thiscall sub_868970(int this) -- PlayerDestroy?
typedef int (WINAPI *fnSub868970)(LPVOID pThis);
int WINAPI Sub868970Hook(LPVOID pThis);

// int __thiscall sub_83FD00(int this) -- PlayerInit?
typedef int (WINAPI *fnSub83FD00)(LPVOID pThis);
int WINAPI Sub83FD00Hook(LPVOID pThis);

#endif//__TEST_HOOK_H__