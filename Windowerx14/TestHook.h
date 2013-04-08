/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Test hooks
**************************************************************************/
#ifndef __TEST_HOOK_H__
#define __TEST_HOOK_H__

// int __thiscall sub_69C130(int this)
typedef int (WINAPI *fnSub69C130)(LPVOID pThis);
int WINAPI Sub69C130Hook(LPVOID pThis);

#endif//__TEST_HOOK_H__