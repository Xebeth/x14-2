/**************************************************************************
	created		:	2011-06-03
	filename	: 	RegisterClassExHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	RegisterClassEx hook definitions
**************************************************************************/
#ifndef __REGISTER_CLASS_EX_HOOK_H__
#define __REGISTER_CLASS_EX_HOOK_H__

typedef ATOM (WINAPI *fnRegisterClassExW)(const WNDCLASSEXW *pWndClass_in);
ATOM WINAPI RegisterClassExWHook(const WNDCLASSEXW *pWndClass_in);

#endif//__REGISTER_CLASS_EX_HOOK_H__