/**************************************************************************
	created		:	2010-10-10
	filename	: 	SystemCore.cpp
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interface with the Win32 API
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <NonCopyable.h>
#include <HookEngine.h>
#include <IATPatcher.h>

#include "BaseEngine.h"
#include "PluginEngine.h"
#include "BootstrapEngine.h"

#include "CreateProcessHook.h"
#include "ShellExecuteExHook.h"
#include "CreateWindowExHook.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "SystemCore.h"

#include <AutoLoginPlugin.h>

namespace Bootstrap
{
	/*! \brief System constructor
		\param[in,out] pEngine : a pointer to the windower engine
	*/
	SystemCore::SystemCore(Windower::PluginEngine &Engine_in)
		: Windower::WindowerCore(Engine_in)
	{
		m_pCreateWindowExWTrampoline = CreateWindowExW;
		m_pShellExecuteExTrampoline = ShellExecuteExW;
		m_pCreateProcessTrampoline = CreateProcessW;
	}

	HWND SystemCore::CreateWindowExWHook(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int X, int Y,
										 int nWidth, int nHeight,HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
	{
		HWND hWndResult = m_pCreateWindowExWTrampoline(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y,
													   nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
		DWORD_PTR ClassAtom = (DWORD_PTR)lpClassName;

		if (lpClassName != NULL && ((ClassAtom & 0xFFFF0000) != NULL) && _tcscmp(TARGET_CLASSNAME, lpClassName) == 0)
		{
			static_cast<BootstrapEngine&>(m_Engine).InvokeAutoLogin(hWndResult);
		}

		return hWndResult;
	}

	BOOL SystemCore::CreateProcessHook(LPCTSTR lpApplicationName, LPTSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
									   LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags,
									   LPVOID lpEnvironment, LPCTSTR lpCurrentDirectory, LPSTARTUPINFO lpStartupInfo,
									   LPPROCESS_INFORMATION lpProcessInformation)
	{
		char DLLPath[MAX_PATH];
		BOOL Result = FALSE;
		TCHAR* pDLL32Path;

#ifndef _DEBUG
		TCHAR *pDirPath = new TCHAR[MAX_PATH];
		pDLL32Path = new TCHAR[MAX_PATH];

		GetCurrentDirectory(MAX_PATH, pDirPath);

		if (_tcsstr(lpCommandLine, TARGET_PROCESS_GAME) != NULL || _tcsstr(lpApplicationName, TARGET_PROCESS_GAME) != NULL)
			_stprintf_s(pDLL32Path, MAX_PATH, _T("%s\\windowerx14.dll"), pDirPath);
		else
			_stprintf_s(pDLL32Path, MAX_PATH, _T("%s\\bootstrap.dll"), pDirPath);

		delete[] pDirPath;
#else
		if ((lpCommandLine != NULL && _tcsstr(lpCommandLine, TARGET_PROCESS_GAME) != NULL)
		 || (lpApplicationName != NULL && _tcsstr(lpApplicationName, TARGET_PROCESS_GAME) != NULL))
			pDLL32Path = _tcsdup(_T("F:\\~dev\\cpp\\Windowerx14\\Debug\\windowerx14.dll"));
		else
			pDLL32Path = _tcsdup(_T("F:\\~dev\\cpp\\Windowerx14\\Debug\\bootstrap.dll"));
#endif // _DEBUG

		sprintf_s(DLLPath, "%S", pDLL32Path);
		// attach the DLL to the next process in the chain
		Result = DetourCreateProcessWithDllW(lpApplicationName, lpCommandLine, lpProcessAttributes,
											 lpThreadAttributes, bInheritHandles, dwCreationFlags,
											 lpEnvironment, lpCurrentDirectory, lpStartupInfo,
											 lpProcessInformation, NULL, DLLPath,
											 m_pCreateProcessTrampoline);
#ifdef _DEBUG
		delete pDLL32Path;
#else
		delete[] pDLL32Path;
#endif // _DEBUG

		return Result;
	}

	BOOL SystemCore::ShellExecuteExHook(LPSHELLEXECUTEINFO lpExecInfo)
	{
		if (lpExecInfo->lpVerb == NULL || _tcsicmp(lpExecInfo->lpVerb, _T("open")) != 0)
		{
			PROCESS_INFORMATION ProcessInfo;
			STARTUPINFO StartupInfo;
			string_t CommandLine;

			format(CommandLine, _T("\"%s\" %s"), lpExecInfo->lpFile, lpExecInfo->lpParameters);
			SecureZeroMemory(&StartupInfo, sizeof(StartupInfo));

			StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
			StartupInfo.wShowWindow = SW_SHOWNORMAL;
			StartupInfo.cb = sizeof(StartupInfo);

			return CreateProcessHook(lpExecInfo->lpFile, (LPTSTR)CommandLine.c_str(), NULL, NULL, FALSE,
									 CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
									 NULL, lpExecInfo->lpDirectory, &StartupInfo, &ProcessInfo);
		}
		else
		{
			return m_pShellExecuteExTrampoline(lpExecInfo);
		}
	}

	void SystemCore::RegisterHooks(IHookManager *pHookManager)
	{
		if (pHookManager != NULL)
		{
			pHookManager->RegisterHook("CreateWindowEx", "User32.dll", CreateWindowExW, ::CreateWindowExWHook);
			pHookManager->RegisterHook("ShellExecuteEx", "Shell32.dll", ShellExecuteExW, ::ShellExecuteExHook);
			IATPatcher::PatchIAT(GetModuleHandle(NULL), "Kernel32.dll", "CreateProcessW", (PVOID*)&m_pCreateProcessTrampoline, ::CreateProcessHook);
			// pHookManager->RegisterHook("CreateProcess", "Kernel32.dll", CreateProcessW, ::CreateProcessHook);
		}
	}

	void SystemCore::OnHookInstall(IHookManager *pHookManager)
	{
		if (pHookManager != NULL)
		{
			m_pCreateWindowExWTrampoline = (fnCreateWindowExW)pHookManager->GetTrampolineFunc("CreateWindowEx");
			m_pShellExecuteExTrampoline = (fnShellExecuteEx)pHookManager->GetTrampolineFunc("ShellExecuteEx");
			// m_pCreateProcessTrampoline = (fnCreateProcess)pHookManager->GetTrampolineFunc("CreateProcess");
		}
	}
}