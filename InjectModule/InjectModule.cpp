/**************************************************************************
	created		:	2011-03-06
	filename	: 	InjectModule.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	DLL Injection
**************************************************************************/
#include "stdafx.h"
#include <string_t.h>
#include "InjectModule.h"

namespace InjectModule
{
	typedef LONG WINAPI fnCreateThreadEx(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, LPVOID ObjectAttributes,
										 HANDLE ProcessHandle, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter,
										 BOOL CreateSuspended, DWORD dwStackSize, LPVOID Unknown1, LPVOID Unknown2, LPVOID Unknown3); 

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
						 TCHAR *pCmdLine_in_out_opt, DWORD CreateProcessFlags_in_opt,
						 const TCHAR *pDllPathX86_in_opt, const TCHAR *pDllPathX64_in_opt)
	{
		BOOL Result;

		// initialize the PROCESS_INFORMATION structure
		RtlZeroMemory(&ProcessInfo_out, sizeof(ProcessInfo_out));

		////////////////////////////////////////////////////////////////////////////
		// Create the target process
		//

#ifdef _M_X64
		const TCHAR* pDllPath(pDllPathX64_in_opt);
#else
		const TCHAR* pDllPath(pDllPathX86_in_opt);
#endif
		if ((Result = CreateSuspendedProcess(ExePath_in, ProcessInfo_out, pCmdLine_in_out_opt, CreateProcessFlags_in_opt)) && pDllPath != NULL)
		{
			////////////////////////////////////////////////////////////////////
			// Inject the DLL by creating a thread in the target process
			// 
			SIZE_T DataSize = (_tcslen(pDllPath) + 1) * sizeof(TCHAR);
			SIZE_T DataWritten = 0;
			HANDLE hThread = NULL;
			DWORD RetCode = 0UL;
			// allocate enough space in the target process to hold the path to the DLL
			void* pReservedSpace = VirtualAllocEx(ProcessInfo_out.hProcess, NULL, DataSize,
												  MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			if (pReservedSpace != NULL)
			{
				HANDLE hThread = NULL;

				// write the path to the DLL in the memory of the target process
				if (WriteProcessMemory(ProcessInfo_out.hProcess, pReservedSpace, pDllPath, DataSize, &DataWritten))
				{
					// create a remote thread in the target process executing LoadLibrary
					// with the path of the DLL as its parameter (this will attach the DLL)
					hThread = CreateRemoteThread(ProcessInfo_out.hProcess, NULL, 0,
												 (LPTHREAD_START_ROUTINE)LoadLibraryW,
												 pReservedSpace, 0, NULL);

					if (hThread != NULL)
					{
						// wait for the thread to terminate
						RetCode = WaitForSingleObject(hThread, INFINITE);
						CloseHandle(hThread);
					}
				}

				if (RetCode == WAIT_OBJECT_0)
				{
					if (GetExitCodeThread(hThread, &RetCode) && FAILED(RetCode))
					{
						SetLastError(RetCode & 0x0FFFFFFF);
					}
				}

				// free the memory allocated in the target process
				VirtualFreeEx(ProcessInfo_out.hProcess, pReservedSpace, DataSize, MEM_COMMIT);
			}
		}

		////////////////////////////////////////////////////////////////////////////
		// Resume the target process
		// 
		if (Result)
			Result = (ResumeThread(ProcessInfo_out.hThread) != -1);

		////////////////////////////////////////////////////////////////////////////
		// Cleanup
		// 
		if(ProcessInfo_out.hProcess != NULL)
		{
			CloseHandle(ProcessInfo_out.hProcess);
			ProcessInfo_out.hProcess = NULL;
		}

		if(ProcessInfo_out.hThread != NULL)
		{
			CloseHandle(ProcessInfo_out.hThread);
			ProcessInfo_out.hThread = NULL;
		}

		return Result;
	}

	/*! \brief Creates a suspended process
		\param[in]			ExePath_in : the path to the target process
		\param[out]			ProcessInfo_out : a PROCESS_INFORMATION structure receiving the result of CreateProcess(...)
		\param[in,out,opt]	pCmdLine_in_out_opt : the command line to be executed
		\param[in,opt]		CreateProcessFlags_in_opt : user-defined to be passed to the dwCreationFlags parameter of CreateProcess(...)
		\return the result of the CreateProcess(...) call
	*/
	BOOL CreateSuspendedProcess(const string_t &ExePath_in, PROCESS_INFORMATION &ProcessInfo_out,
								TCHAR *pCmdLine_in_out_opt, DWORD CreateProcessFlags_in_opt)
	{
		STARTUPINFO StartupInfo;

		// initialize the STARTUPINFO structure
		RtlZeroMemory(&StartupInfo, sizeof(StartupInfo));

		StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
		StartupInfo.wShowWindow = SW_SHOWNORMAL;
		StartupInfo.cb = sizeof(StartupInfo);
		// try to determine the current directory from the executable path
		UINT IndexOf = ExePath_in.rfind('\\');
		string_t CurrentDirectory;

		CurrentDirectory = (IndexOf != STRING_T_NPOS) ? ExePath_in.substr(0, IndexOf + 1) : _T(".");

		return CreateProcess(ExePath_in.c_str(), pCmdLine_in_out_opt, NULL, NULL, FALSE,
							 CreateProcessFlags_in_opt | CREATE_SUSPENDED | NORMAL_PRIORITY_CLASS,
							 NULL, CurrentDirectory.c_str(), &StartupInfo, &ProcessInfo_out);
	}

	HANDLE NtCreateThreadEx(HANDLE hProcess_in, LPTHREAD_START_ROUTINE pRemoteRoutine_in,
							void* pfnCallback_in, bool CreateSuspended_in)
	{
		fnCreateThreadEx *pNtCreateThreadEx;
		HANDLE hRemoteThread = NULL;
		HMODULE hNtDll;

		if ((hNtDll = GetModuleHandle(_T("ntdll.dll"))) != NULL)
		{
			pNtCreateThreadEx = (fnCreateThreadEx*)GetProcAddress(hNtDll, "NtCreateThreadEx");

			if (pNtCreateThreadEx != NULL)
			{
				pNtCreateThreadEx(&hRemoteThread, SPECIFIC_RIGHTS_ALL, NULL, hProcess_in,
								  (LPTHREAD_START_ROUTINE)pRemoteRoutine_in, pfnCallback_in,
								  CreateSuspended_in, 0, NULL, NULL, NULL);
			}
		}

		return hRemoteThread;
	}
}