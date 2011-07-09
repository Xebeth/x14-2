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
						    void* pfnCallback_in, bool CreateSuspended_in);

	/*! \brief Creates a suspended process
		\param[in]			ExePath_in : the path to the target process
		\param[out]			ProcessInfo_out : a PROCESS_INFORMATION structure receiving the result of CreateProcess(...)
		\param[in,out,opt]	pCmdLine_in_out_opt : the command line to be executed
		\param[in,opt]		CreateProcessFlags_in_opt : user-defined to be passed to the dwCreationFlags parameter of CreateProcess(...)
		\return the result of the CreateProcess(...) call
	*/
	BOOL CreateSuspendedProcess(const string_t &ExePath_in, PROCESS_INFORMATION &ProcessInfo_out,
								TCHAR *pCmdLine_in_out_opt = NULL, DWORD CreateProcessFlags_in_opt = NORMAL_PRIORITY_CLASS);
	/*! \brief Creates a suspended process, injects the DLL and resumes the execution
		\param[in]			ExePath_in : the path to the target process
		\param[out]			ProcessInfo_out : a PROCESS_INFORMATION structure receiving the result of CreateProcess(...)
		\param[in,out,opt]	pCmdLine_in_out_opt : the command line to be executed
		\param[in,opt]		CreateProcessFlags_in_opt : user-defined to be passed to the dwCreationFlags parameter of CreateProcess(...)
		\param[in,opt]		pDllPathX86_in_opt : the path to the injected DLL for an x86 target
		\param[in,opt]		pDllPathX64_in_opt : the path to the injected DLL for an x64 target
		\return TRUE if the injection succeeded; FALSE otherwise
	*/
	BOOL CreateProcessEx(const string_t &ExePath_in, PROCESS_INFORMATION &ProcessInfo_out,
						 TCHAR *pCmdLine_in_out_opt = NULL, DWORD CreateProcessFlags_in_opt = NORMAL_PRIORITY_CLASS,
						 const TCHAR *pDllPathX86_in_opt = NULL, const TCHAR *pDllPathX64_in_opt = NULL);
	BOOL UnloadDLL(HMODULE hDLL_in);
}

#endif//__INJECT_MODULE_H__