/**************************************************************************
	created		:	2011-06-03
	filename	: 	SetCursorPosHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	SetCursorPos hook definitions
**************************************************************************/
#ifndef __SET_CURSOR_POS_HOOK_H__
#define __SET_CURSOR_POS_HOOK_H__

typedef BOOL (WINAPI *fnSetCursorPos)(INT X_in, INT Y_in);
BOOL WINAPI SetCursorPosAHook(INT X_in, INT Y_in);

#endif//__SET_CURSOR_POS_HOOK_H__