/**************************************************************************
	created		:	2011-03-06
	filename	: 	InjectModule.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	DLL injection
**************************************************************************/
#ifndef __INJECT_MODULE_H__
#define __INJECT_MODULE_H__

#include <string_t.h>

namespace InjectModule
{
	HANDLE NtCreateThreadEx(HANDLE hProcess_in, LPTHREAD_START_ROUTINE pRemoteRoutine_in,
						    LPVOID pfnCallback_in, bool CreateSuspended_in);

	BOOL CreateSuspendedProcess(const string_t &ExePath_in, PROCESS_INFORMATION &ProcessInfo_out,
								const TCHAR *pCmdLine_in_out_opt = NULL, DWORD CreateProcessFlags_in_opt = NORMAL_PRIORITY_CLASS);
	BOOL CreateProcessEx(const string_t &ExePath_in, PROCESS_INFORMATION &ProcessInfo_out, const TCHAR *pCmdLine_in_out_opt = NULL,
						 DWORD CreateProcessFlags_in_opt = NORMAL_PRIORITY_CLASS, const TCHAR *pDllPathX86_in_opt = NULL);
	BOOL UnloadDLL(HMODULE hDLL_in);
}

#endif//__INJECT_MODULE_H__