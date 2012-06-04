/**************************************************************************
	created		:	2010-10-10
	filename	: 	SystemCore.cpp
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interface with the Win32 API
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <HookEngine.h>

#include "WindowerSettings.h"
#include "WindowerEngine.h"

#include "RegisterClassExHook.h"
#include "CreateWindowExHook.h"
#include "WndProcHook.h"

#include "WindowerSettings.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "SystemCore.h"

#include "Direct3D9Hook.h"
#include "GraphicsCore.h"

namespace Windower
{
	/*! \brief SystemCore constructor
		\param[in,out] pEngine : the windower engine
	*/
	SystemCore::SystemCore(WindowerEngine &Engine_in_out) : WindowerCore(Engine_in_out)
	{
		m_pRegisterClassExWTrampoline = RegisterClassExW;
		m_pCreateWindowExATrampoline = CreateWindowExA;
		m_dwPID = GetCurrentProcessId();
		m_hMainThreadHandle = NULL;
		m_MinimizeVKey = VK_F11;
		m_pGameWndProc = NULL;
		m_MainThreadID = 0;
		m_hGameWnd = NULL;		
		// register the module
		m_Engine.RegisterModule(_T("System"), this);
	}

	/*! \brief Starts the main thread of the windower engine
		\param[in,out] pParam_in_out : a pointer to the windower engine
		\return the exit code of the thread
	*/
	DWORD WINAPI SystemCore::MainThreadStatic(LPVOID pParam_in_out)
	{
		return reinterpret_cast<WindowerEngine*>(pParam_in_out)->MainThread();
	}

	/*! \brief Creates the main thread of the windower engine
		\return a handle to the thread
	*/
	HANDLE SystemCore::CreateEngineThread()
	{
		if (m_hMainThreadHandle == NULL)
		{
			// create the main thread
			m_hMainThreadHandle = CreateThread(NULL, 0, SystemCore::MainThreadStatic, &m_Engine, 0, &m_MainThreadID);
		}

		return m_hMainThreadHandle;
	}

	/*! \brief Creates an overlapped, pop-up, or child window with an extended window style
			   Hooked to set the dimensions of the window before its creation and start the engine thread
		\param[in] dwExStyle_in : extended window style of the window being created
		\param[in] lpClassName_in : a null-terminated string or a class atom
		\param[in] lpWindowName_in : the window name
		\param[in] dwStyle_in : the style of the window being created
		\param[in] X_in : the initial horizontal position of the window
		\param[in] Y_in : the initial vertical position of the window
		\param[in] nWidth_in : the width, in device units, of the window
		\param[in] nHeight_in : the height, in device units, of the window
		\param[in] hWndParent_in : handle to the parent or owner window of the window being created
		\param[in] hMenu_in : handle to a menu, or specifies a child-window identifier, depending on the window style
		\param[in] hInstance_in : handle to the instance of the module to be associated with the window
		\param[in] lpParam_in : value to be passed to the window through the CREATESTRUCT structure
		\return a handle to the new window; NULL otherwise
	*/
	HWND SystemCore::CreateWindowExAHook(DWORD dwExStyle_in, LPCSTR lpClassName_in, LPCSTR lpWindowName_in, DWORD dwStyle_in, int X_in, int Y_in, 
										 int nWidth_in, int nHeight_in, HWND hWndParent_in, HMENU hMenu_in, HINSTANCE hInstance_in, LPVOID lpParam_in)
	{
		HWND hWnd_in = NULL;

		if (m_pCreateWindowExATrampoline != NULL)
		{
			bool bIsGameWnd = (lpWindowName_in != NULL && strcmp(lpWindowName_in, FFXIV_WINDOW_NAMEA) == 0);

			if (bIsGameWnd)
			{
				// force the game window to appear in the taskbar
				dwExStyle_in |= WS_EX_APPWINDOW;

				X_in = static_cast<WindowerEngine&>(m_Engine).Settings().GetResX();
				Y_in = static_cast<WindowerEngine&>(m_Engine).Settings().GetResY();
			}

			hWnd_in = m_pCreateWindowExATrampoline(dwExStyle_in, lpClassName_in, lpWindowName_in, dwStyle_in, X_in, Y_in,
												   nWidth_in, nHeight_in, hWndParent_in, hMenu_in, hInstance_in, lpParam_in);

			if (bIsGameWnd && hWnd_in != NULL)
			{
				CreateEngineThread();
				m_hGameWnd = hWnd_in;
			}
		}

		return hWnd_in;
	}

	/*! \brief Registers a window class
			   Hooked to hijack the target process WndProc function
		\param[in] pWndClass_in : pointer to a WNDCLASSEX structure
		\return class atom that uniquely identifies the class being registered if successfuly; NULL otherwise
	*/
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

	//! \brief Restores the target WndProc
	void SystemCore::RestoreWndProc()
	{
		SetWindowLongPtr(m_hGameWnd, GWL_WNDPROC, (LONG)(LONG_PTR)m_pGameWndProc);
	}

	/*! \brief Processes messages sent to a window
			   Hooked to intercept messages received by the game window
		\param[in] hWnd_in : handle to the window
		\param[in] uMsg_in : message
		\param[in] wParam_in : additional message information
		\param[in] lParam_in : additional message information
		\return result of the message processing that depends on the message sent
	*/
	LRESULT SystemCore::WndProcHook(HWND hWnd_in, UINT uMsg_in, WPARAM wParam_in, LPARAM lParam_in)
	{
		if (uMsg_in == WM_NCDESTROY)
		{
			// start shutting down the engine
			static_cast<WindowerEngine&>(m_Engine).OnShutdown();
			// remove the WndProc hook before the game window is destroyed
			RestoreWndProc();
		}
		else if (uMsg_in == WM_KEYUP || uMsg_in == WM_KEYDOWN)
		{
			return FilterKeyboard(hWnd_in, uMsg_in, wParam_in, lParam_in);
		}
		else if (uMsg_in == WM_ACTIVATE && wParam_in == WA_ACTIVE)
		{
			static_cast<WindowerEngine&>(m_Engine).Graphics().SetRendering(true);

			return FALSE;
		}

		if (m_pGameWndProc != NULL)
			return m_pGameWndProc(hWnd_in, uMsg_in, wParam_in, lParam_in);
		else
			return DefWindowProc(hWnd_in, uMsg_in, wParam_in, lParam_in);
	}

	/*! \brief Filters key presses
		\param[in] hWnd_in : handle to the window
		\param[in] uMsg_in : message
		\param[in] wParam_in : additional message information
		\param[in] lParam_in : additional message information
		\return result of the message processing that depends on the message sent
	*/
	LRESULT SystemCore::FilterKeyboard(HWND hWnd_in, UINT uMsg_in, WPARAM wParam_in, LPARAM lParam_in)
	{
		LRESULT Result = TRUE;

		if (hWnd_in == m_hGameWnd && uMsg_in == WM_KEYUP)
		{
			if (wParam_in == m_MinimizeVKey)
			{
				if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) == 0x8000)
				{
					static_cast<WindowerEngine&>(m_Engine).Graphics().ToggleRendering();
				}
				else
				{
					static_cast<WindowerEngine&>(m_Engine).Graphics().SetRendering(false);
					ShowWindow(m_hGameWnd, SW_MINIMIZE);
				}

				Result = FALSE;
			}
		}

		return Result;
	}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void SystemCore::RegisterHooks(IHookManager &HookManager_in)
	{
		// register the RegisterClassExW hook
		HookManager_in.RegisterHook("RegisterClassExW", "user32.dll", NULL, ::RegisterClassExWHook);
		// register the CreateWindowExA hook 
		HookManager_in.RegisterHook("CreateWindowExA", "user32.dll", NULL, ::CreateWindowExAHook);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void SystemCore::OnHookInstall(IHookManager &HookManager_in)
	{
		m_pRegisterClassExWTrampoline = (fnRegisterClassExW)HookManager_in.GetTrampolineFunc("RegisterClassExW");
		m_pCreateWindowExATrampoline = (fnCreateWindowExA)HookManager_in.GetTrampolineFunc("CreateWindowExA");
	}
}