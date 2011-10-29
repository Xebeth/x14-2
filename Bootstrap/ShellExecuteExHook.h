/**************************************************************************
	created		:	2011-05-09
	filename	: 	ShellExecuteExHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	ShellExecuteExW hook definitions
**************************************************************************/
#ifndef _SHELL_EXECUTE_EX_HOOK_H__
#define _SHELL_EXECUTE_EX_HOOK_H__

typedef BOOL (WINAPI *fnShellExecuteEx)(LPSHELLEXECUTEINFO lpExecInfo_in);
BOOL WINAPI ShellExecuteExHook(LPSHELLEXECUTEINFO lpExecInfo_in);

#endif//_SHELL_EXECUTE_EX_HOOK_H__