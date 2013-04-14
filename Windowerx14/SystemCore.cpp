/**************************************************************************
	created		:	2010-10-10
	filename	: 	SystemCore.cpp
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interface with the Win32 API
**************************************************************************/
#include "stdafx.h"

#include "WindowerEngine.h"

#include "RegisterClassExHook.h"
#include "CreateWindowExHook.h"
#include "SetCursorPosHook.h"
#include "Direct3D9Hook.h"
#include "WndProcHook.h"

#include "IRenderable.h"

#include "WindowerCore.h"
#include "GraphicsCore.h"
#include "SystemCore.h"

namespace Windower
{
	/*! \brief SystemCore constructor
		\param[in,out] pEngine : the windower engine
	*/
	SystemCore::SystemCore(WindowerEngine &Engine_in_out, HookEngine &HookManager_in_out) 
		: WindowerCore(_T("System"), Engine_in_out, HookManager_in_out), m_hGameWnd(NULL),
		  m_pRegisterClassExATrampoline(RegisterClassExA), m_dwPID (GetCurrentProcessId()),
		  m_pCreateWindowExATrampoline(CreateWindowExA), m_hMainThreadHandle(NULL),
		  m_pSetCursorPosTrampoline(SetCursorPos), m_MinimizeVKey(VK_F11),
		  m_pGameWndProc(NULL), m_MainThreadID(0) {}

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
			hWnd_in = m_pCreateWindowExATrampoline(dwExStyle_in, lpClassName_in, lpWindowName_in, dwStyle_in, X_in, Y_in,
												   nWidth_in, nHeight_in, hWndParent_in, hMenu_in, hInstance_in, lpParam_in);

			if (m_hGameWnd == NULL && hWnd_in != NULL				// stop checking after the game HWND was found
			 && (ATOM)lpClassName_in != NULL						// lpClassName can be an ATOM (16-bit)
			 && strcmp(lpClassName_in, FFXIV_WINDOW_CLASSA) == 0)	// check for the game class name
			{
				CreateEngineThread();
				m_hGameWnd = hWnd_in;
				// remove the hook since it is no longer needed
				m_HookManager.UninstallHook("CreateWindowExA");
				m_HookManager.UnregisterHook("CreateWindowExA");
			}
		}

		return hWnd_in;
	}

	/*! \brief Registers a window class
			   Hooked to hijack the target process WndProc function
		\param[in] pWndClass_in : pointer to a WNDCLASSEX structure
		\return class atom that uniquely identifies the class being registered if successfuly; NULL otherwise
	*/
	ATOM SystemCore::RegisterClassExAHook(const WNDCLASSEXA *pWndClass_in)
	{
		ATOM Result = 0L;

		if (m_pRegisterClassExATrampoline != NULL)
		{
			// replacing the pointer to the window procedure
			WNDCLASSEXA *pWndClass = const_cast<WNDCLASSEXA *>(pWndClass_in);

			// check for the game class name
			if (strcmp(pWndClass->lpszClassName, FFXIV_WINDOW_CLASSA) == 0)
			{
				// save the original window procedure then replace it
				m_pGameWndProc = pWndClass->lpfnWndProc;
				pWndClass->lpfnWndProc = ::WndProcHook;
				// remove the hook since it is no longer needed
				m_HookManager.UninstallHook("RegisterClassExA");
				m_HookManager.UnregisterHook("RegisterClassExA");
			}

			Result = m_pRegisterClassExATrampoline(pWndClass);
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
		switch (uMsg_in)
		{
			case WM_SIZE:
				m_Engine.Graphics().SetWindowSize(LOWORD(lParam_in), HIWORD(lParam_in));
			break;
/*
			case WM_LBUTTONDOWN:
				if (m_Engine.Graphics().OnLButtonDown(LOWORD(lParam_in), HIWORD(lParam_in), wParam_in))
					return 0;
			break;
			case WM_LBUTTONUP:
				if (m_Engine.Graphics().OnLButtonUp(LOWORD(lParam_in), HIWORD(lParam_in), wParam_in))
					return 0;
			break;
			case WM_MOUSEMOVE:
				m_Engine.Graphics().OnMouseMove(LOWORD(lParam_in), HIWORD(lParam_in), wParam_in);
			break;
*/
			case WM_QUIT:
			case WM_CLOSE:
			case WM_DESTROY:
			case WM_NCDESTROY:
				// start shutting down the engine
				m_Engine.OnShutdown();
				// remove the WndProc hook before the game window is destroyed
				RestoreWndProc();
			break;
			case WM_ACTIVATEAPP:
			case WM_ACTIVATE:
				m_Engine.Graphics().SetRendering(true);
			break;
			case WM_KEYDOWN:
			case WM_KEYUP:
				return FilterKeyboard(hWnd_in, uMsg_in, wParam_in, lParam_in);
			break;
		}

		if (m_pGameWndProc != NULL)
			return m_pGameWndProc(hWnd_in, uMsg_in, wParam_in, lParam_in);
		else
			return ::DefWindowProc(hWnd_in, uMsg_in, wParam_in, lParam_in);
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
			bool bCtrl = ((GetAsyncKeyState(VK_CONTROL) & 0x8000) == 0x8000);

			if (wParam_in == m_MinimizeVKey)
			{
				if (bCtrl)
				{
					m_Engine.Graphics().ToggleRendering();
				}
				else
				{
					m_Engine.Graphics().SetRendering(false);
					ShowWindow(m_hGameWnd, SW_MINIMIZE);
				}

				Result = FALSE;
			}
			else if (wParam_in == VK_F12 && bCtrl)
			{
				m_Engine.Graphics().ToggleFPS();
			}
		}

		return Result;
	}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void SystemCore::RegisterHooks(HookEngineLib::IHookManager &HookManager_in)
	{
		// register the RegisterClassExA hook
		HookManager_in.RegisterHook("RegisterClassExA", "user32.dll", RegisterClassExA, ::RegisterClassExAHook);
		// register the CreateWindowExA hook 
		HookManager_in.RegisterHook("CreateWindowExA", "user32.dll", CreateWindowExA, ::CreateWindowExAHook);
		// register the SetCursorPos hook 
		// HookManager_in.RegisterHook("SetCursorPos", "user32.dll", SetCursorPos, ::SetCursorPosAHook);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void SystemCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		m_pRegisterClassExATrampoline = (fnRegisterClassExA)HookManager_in.GetTrampolineFunc("RegisterClassExA");
		m_pCreateWindowExATrampoline = (fnCreateWindowExA)HookManager_in.GetTrampolineFunc("CreateWindowExA");
		m_pSetCursorPosTrampoline = (fnSetCursorPos)HookManager_in.GetTrampolineFunc("SetCursorPos");
	}

	BOOL SystemCore::SetCursorPosAHook(INT X_in, INT Y_in)
	{
		if (m_Engine.Graphics().IsMouseCaptured() == false)
			return m_pSetCursorPosTrampoline(X_in, Y_in);

		return TRUE;
	}
}