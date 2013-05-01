/**************************************************************************
	created		:	2011-03-06
	filename	: 	InjectModule.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	DLL Injection
**************************************************************************/
#include "stdafx.h"
#include <tlhelp32.h>

#include "InjectModule.h"

namespace InjectModule
{
	typedef LONG WINAPI fnCreateThreadEx(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, LPVOID ObjectAttributes,
										 HANDLE ProcessHandle, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter,
										 BOOL CreateSuspended, DWORD dwStackSize, LPVOID Unknown1, LPVOID Unknown2, LPVOID Unknown3); 

	/*! \brief Creates a suspended process, injects the DLL and resumes the execution
		\param[in]		ExePath_in : the path to the target process
		\param[out]		ProcessInfo_out : a PROCESS_INFORMATION structure receiving the result of CreateProcess(...)
		\param[in,out]	pCmdLine_in_out : the command line to be executed
		\param[in]		CreateProcessFlags_in : user-defined to be passed to the dwCreationFlags parameter of CreateProcess(...)
		\param[in]		pDllPath_in : the path to the injected DLL for an x86 target
		\return TRUE if the injection succeeded; FALSE otherwise
	*/
	BOOL CreateProcessEx(const string_t &ExePath_in, PROCESS_INFORMATION &ProcessInfo_out,
						 const TCHAR *pCmdLine_in_out, DWORD CreateProcessFlags_in,
						 const TCHAR *pDllPath_in)
	{
		BOOL Result;

		// initialize the PROCESS_INFORMATION structure
		RtlZeroMemory(&ProcessInfo_out, sizeof(ProcessInfo_out));

		////////////////////////////////////////////////////////////////////////////
		// Create the target process
		//

		if (CreateSuspendedProcess(ExePath_in, ProcessInfo_out, pCmdLine_in_out, CreateProcessFlags_in)
		 && InjectModule(ProcessInfo_out.hProcess, pDllPath_in))
		{
			// resume the process execution
			Result = (ResumeThread(ProcessInfo_out.hThread) != -1);	
		}
		else
		{
			// terminate the process
			Result = !TerminateProcess(ProcessInfo_out.hProcess, 0xFFFFFFFFU);
		}

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
		\param[in]		ExePath_in : the path to the target process
		\param[out]		ProcessInfo_out : a PROCESS_INFORMATION structure receiving the result of CreateProcess(...)
		\param[in,out]	pCmdLine_in_out : the command line to be executed
		\param[in]		CreateProcessFlags_in : user-defined to be passed to the dwCreationFlags parameter of CreateProcess(...)
		\return the result of the CreateProcess(...) call
	*/
	BOOL CreateSuspendedProcess(const string_t &ExePath_in, PROCESS_INFORMATION &ProcessInfo_out,
								const TCHAR *pCmdLine_in_out, DWORD CreateProcessFlags_in)
	{
		TCHAR *pCmdLine = _tcsdup(pCmdLine_in_out);
		STARTUPINFO StartupInfo;
		BOOL bResult;

		// initialize the STARTUPINFO structure
		RtlZeroMemory(&StartupInfo, sizeof(StartupInfo));

		StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
		StartupInfo.wShowWindow = SW_SHOWNORMAL;
		StartupInfo.cb = sizeof(StartupInfo);
		// try to determine the current directory from the executable path
		UINT IndexOf = ExePath_in.rfind('\\');
		string_t CurrentDirectory;

		CurrentDirectory = (IndexOf != STRING_T_NPOS) ? ExePath_in.substr(0, IndexOf + 1) : _T(".");

		bResult = CreateProcess(ExePath_in.c_str(), pCmdLine, NULL, NULL, FALSE,
								CreateProcessFlags_in | CREATE_SUSPENDED | NORMAL_PRIORITY_CLASS,
								NULL, CurrentDirectory.c_str(), &StartupInfo, &ProcessInfo_out);
		// cleanup
		free(pCmdLine);

		return bResult;
	}

	/*! \brief Imports 'NtCreateThreadEx' from 'ntdll.dll' and calls it to create a thread
		\param[in] hProcess_in : handle to the process in which the thread is to be created
		\param[in] pRemoteRoutine_in : function pointer to be executed by the thread
		\param[in] lpThreadData_in : pointer to a variable to be passed to the thread function
		\param[in] CreateSuspended_in : flag specifying if the the thread is created in a suspended state
		\return the handle to the new thread
	*/
	HANDLE NtCreateThreadEx(HANDLE hProcess_in, LPTHREAD_START_ROUTINE pRemoteRoutine_in,
							LPVOID lpThreadData_in, bool CreateSuspended_in)
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
								  (LPTHREAD_START_ROUTINE)pRemoteRoutine_in, lpThreadData_in,
								  CreateSuspended_in, 0, NULL, NULL, NULL);
			}
		}

		return hRemoteThread;
	}

	/*! \brief Injects a DLL by creating a thread in the target process
		\param[in] hProcess_in : the handle of the process to inject into
		\param[in] pDllPath_in : the path of the DLL to inject
	*/
	BOOL InjectModule(HANDLE hProcess_in, const TCHAR *pDllPath_in)
	{
		if (hProcess_in != NULL && pDllPath_in != NULL)
		{
			////////////////////////////////////////////////////////////////////
			// Inject the DLL by creating a thread in the target process
			// 
			SIZE_T DataSize = (_tcslen(pDllPath_in) + 1) * sizeof(TCHAR);
			SIZE_T DataWritten = 0;
			HANDLE hThread = NULL;
			DWORD RetCode = 0UL;
			// allocate enough space in the target process to hold the path to the DLL
			LPVOID pReservedSpace = VirtualAllocEx(hProcess_in, NULL, DataSize,
												   MEM_COMMIT, PAGE_EXECUTE_READWRITE);

			if (pReservedSpace != NULL)
			{
				// write the path to the DLL in the memory of the target process
				if (WriteProcessMemory(hProcess_in, pReservedSpace, pDllPath_in, DataSize, &DataWritten))
				{
					// create a remote thread in the target process executing LoadLibrary
					// with the path of the DLL as its parameter (this will attach the DLL)
					hThread = CreateRemoteThread(hProcess_in, NULL, 0,
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

						return FALSE;
					}
				}

				// free the memory allocated in the target process
				VirtualFreeEx(hProcess_in, pReservedSpace, DataSize, MEM_COMMIT);

				return TRUE;
			}
		}

		return FALSE;
	}

	/*! \brief Finds the handle of a process given an executable name
		\param[in] pProcessExe_in : the name of the executable
		\return the process handle if found; NULL otherwise
	*/
	HANDLE FindProcess(const TCHAR *pProcessExe_in)
	{
		HANDLE Result = NULL;

		if (pProcessExe_in != NULL)
		{
			PROCESSENTRY32 uProcess;

			ZeroMemory(&uProcess, sizeof(uProcess));
			uProcess.dwSize = sizeof(uProcess); 

			// create a snapshot of modules and iterate the list
			HANDLE hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
			BOOL bContinue = Process32First(hModuleSnapshot, &uProcess);

			// walk through the process list
			while(bContinue)
			{
				if(_tcsicmp(uProcess.szExeFile, pProcessExe_in) == 0)
				{
					Result = OpenProcess(PROCESS_ALL_ACCESS, FALSE, uProcess.th32ProcessID);
					break;
				}

				bContinue = Process32Next(hModuleSnapshot, &uProcess);
			}

			CloseHandle(hModuleSnapshot);
		}

		return Result;
	}
}