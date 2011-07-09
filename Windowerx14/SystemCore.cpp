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

#include "BaseEngine.h"
#include "PluginEngine.h"
#include "WindowerEngine.h"

#include "RegisterClassExHook.h"
#include "CreateWindowExHook.h"
#include "WndProcHook.h"

#include "WindowerSettings.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "SystemCore.h"

namespace Windower
{
	/*! \brief System constructor
		\param[in,out] pEngine : a pointer to the windower engine
	*/
	SystemCore::SystemCore(WindowerEngine &Engine_in) : WindowerCore(Engine_in)
	{
		m_pRegisterClassExWTrampoline = RegisterClassExW;
		m_pCreateWindowExATrampoline = CreateWindowExA;
		m_hMainThreadHandle = NULL;
		m_MinimizeVKey = VK_F11;
		m_pGameWndProc = NULL;
		m_MainThreadID = 0;
		m_hGameWnd = NULL;

		m_dwPID = GetCurrentProcessId();
	}

	DWORD WINAPI SystemCore::MainThreadStatic(void* pParam)
	{
		return ((WindowerEngine*)pParam)->MainThread();
	}

	HANDLE SystemCore::CreateEngineThread()
	{
		if (m_hMainThreadHandle == NULL)
		{
			// create the main thread
			m_hMainThreadHandle = CreateThread(NULL, 0, SystemCore::MainThreadStatic, &m_Engine, 0, &m_MainThreadID);
		}

		return m_hMainThreadHandle;
	}

	HWND SystemCore::CreateWindowExAHook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y,
										 int nWidth, int nHeight,HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
	{
		HWND hWnd = NULL;

		if (m_pCreateWindowExATrampoline != NULL)
		{
			bool bIsGameWnd = (lpWindowName != NULL && strcmp(lpWindowName, FFXIV_WINDOW_NAMEA) == 0);

			if (bIsGameWnd)
			{
				// force the game window to appear in the taskbar
				dwExStyle |= WS_EX_APPWINDOW;

				X = static_cast<WindowerEngine&>(m_Engine).Settings().GetResX();
				Y = static_cast<WindowerEngine&>(m_Engine).Settings().GetResY();
			}

			hWnd = m_pCreateWindowExATrampoline(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y,
												nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

			if (bIsGameWnd && hWnd != NULL)
			{
				CreateEngineThread();
				m_hGameWnd = hWnd;
			}
		}

		return hWnd;
	}

	ATOM SystemCore::RegisterClassExWHook(const WNDCLASSEXW *pWndClass_in)
	{
		ATOM Result = 0L;

		if (m_pRegisterClassExWTrampoline != NULL)
		{
			WNDCLASSEXW *pWndClass = const_cast<WNDCLASSEXW *>(pWndClass_in);

			if (_tcscmp(pWndClass->lpszClassName, FFXIV_WINDOW_CLASSW) == 0)
			{
				m_pGameWndProc = pWndClass->lpfnWndProc;
				pWndClass->lpfnWndProc = ::WndProcHook;
			}

			Result = m_pRegisterClassExWTrampoline(pWndClass);
		}

		return Result;
	}

	void SystemCore::RestoreWndProc()
	{
		SetWindowLongPtr(m_hGameWnd, GWL_WNDPROC, (LONG)(LONG_PTR)m_pGameWndProc);
	}

	LRESULT SystemCore::WndProcHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_NCDESTROY)
		{
			// start shutting down the engine
			static_cast<WindowerEngine&>(m_Engine).OnShutdown();
			// remove the WndProc hook before the game window is destroyed
			RestoreWndProc();
		}
		else if (uMsg == WM_KEYUP || uMsg == WM_KEYDOWN)
		{
			return FilterKeyboard(hWnd, uMsg, wParam, lParam);
		}

		if (m_pGameWndProc != NULL)
			return m_pGameWndProc(hWnd, uMsg, wParam, lParam);
		else
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	LRESULT SystemCore::FilterKeyboard(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT Result = TRUE;

		if (hWnd == m_hGameWnd && uMsg == WM_KEYUP)
		{
			if (wParam == m_MinimizeVKey)
			{
				ShowWindow(m_hGameWnd, SW_MINIMIZE);
				Result = FALSE;
			}
		}

		return Result;
	}

	void SystemCore::RegisterHooks(IHookManager *pHookManager)
	{
		if (pHookManager != NULL)
		{
			// register the RegisterClassExW hook
			pHookManager->RegisterHook("RegisterClassExW", "user32.dll", NULL, ::RegisterClassExWHook);
			// register the CreateWindowExA hook 
			// pHookManager->RegisterHook("CreateWindowExA", "user32.dll", NULL, ::CreateWindowExAHook);
		}
	}

	void SystemCore::OnHookInstall(IHookManager *pHookManager)
	{
		if (pHookManager != NULL)
		{
 			m_pRegisterClassExWTrampoline = (fnRegisterClassExW)pHookManager->GetTrampolineFunc("RegisterClassExW");
 			// m_pCreateWindowExATrampoline = (fnCreateWindowExA)pHookManager->GetTrampolineFunc("CreateWindowExA");
		}
	}
}