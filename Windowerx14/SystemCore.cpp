/**************************************************************************
	created		:	2010-10-10
	filename	: 	SystemCore.cpp
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interface with the Win32 API
**************************************************************************/
#include "stdafx.h"
#include <CommCtrl.h>

#include "WindowerEngine.h"

#include "WndHook.h"
#include "Direct3D9Hook.h"

#include "IRenderable.h"

#include "WindowerCore.h"
#include "GraphicsCore.h"
#include "SystemCore.h"

namespace Windower
{
	fnSetWindowSubclass SystemCore::m_pSetWindowSubclassTrampoline = NULL;
	fnRegisterClassExA SystemCore::m_pRegisterClassExATrampoline = NULL;
	WNDPROC SystemCore::m_pGameWndProc = NULL;
	HANDLE SystemCore::m_hMainThread = NULL;
	DWORD SystemCore::m_SubClassRef = 2UL;
	HWND SystemCore::m_hGameWnd = NULL;	

	/*! \brief SystemCore constructor
		\param[in,out] pEngine : the windower engine
	*/
	SystemCore::SystemCore() : WindowerCore(_T(SYSTEM_MODULE)) {}

	/*! \brief Starts the main thread of the windower engine
		\param[in,out] pParam_in_out : a pointer to the windower engine
		\return the exit code of the thread
	*/
	DWORD WINAPI SystemCore::MainThreadStatic(LPVOID pParam_in_out)
	{
		if (m_pEngine != NULL)
			m_pEngine->MainThread();

		return 0UL;
	}

	//! \brief Restores the target WndProc
	void SystemCore::RestoreWndProc()
	{
		SetWindowLongPtr(m_hGameWnd, GWL_WNDPROC, (LONG_PTR)m_pGameWndProc);
		RemoveWindowSubclass(m_hGameWnd, &SystemCore::SubclassProcHook, 0UL);
	}

	/*! \brief Registers a window class
			   Hooked to hijack the target process WndProc function
		\param[in] pWndClass_in : pointer to a WNDCLASSEX structure
		\return class atom that uniquely identifies the class being registered if successfuly; NULL otherwise
	*/
	ATOM WINAPI SystemCore::RegisterClassExAHook(const WNDCLASSEXA *pWndClass_in)
	{
		ATOM Result = 0L;

		if (m_pRegisterClassExATrampoline != NULL)
		{
			// replacing the pointer to the window procedure
			WNDCLASSEXA *pWndClass = const_cast<WNDCLASSEXA *>(pWndClass_in);
			bool Uninstall = false;

			// check for the game class name
			if (strcmp(pWndClass->lpszClassName, FFXIV_WINDOW_CLASSA) == 0)
			{
				Uninstall = true;
				// save the original window procedure then replace it
				m_pGameWndProc = pWndClass->lpfnWndProc;
				pWndClass->lpfnWndProc = &SystemCore::WndProcHook;
			}

			Result = m_pRegisterClassExATrampoline(pWndClass);

			if (Uninstall)
			{
				// remove the hook since it is no longer needed
				m_pHookManager->UninstallHook("RegisterClassExA");
				m_pHookManager->UnregisterHook("RegisterClassExA");
			}
		}

		return Result;
	}

	/*! \brief Filters the message received by the game window
		\param[in] hWnd_in : handle to the window
		\param[in] uMsg_in : message
		\param[in] wParam_in : additional message information
		\param[in] lParam_in : additional message information
		\return 1UL if the message should be filtered; 0UL otherwise
	*/
	LRESULT SystemCore::FilterMessages(HWND hWnd_in, UINT uMsg_in, WPARAM wParam_in, LPARAM lParam_in)
	{
		switch (uMsg_in)
		{
			case WM_SIZE:
				if (m_pEngine != NULL)
					m_pEngine->Graphics().SetWindowSize(LOWORD(lParam_in), HIWORD(lParam_in));
			break;
			case WM_LBUTTONDOWN:
				if (m_pEngine != NULL && m_pEngine->Graphics().OnLButtonDown(LOWORD(lParam_in), HIWORD(lParam_in), wParam_in))
					return 1UL; // filtered
			break;
			case WM_LBUTTONUP:
				if (m_pEngine != NULL && m_pEngine->Graphics().OnLButtonUp(LOWORD(lParam_in), HIWORD(lParam_in), wParam_in))
					return 1UL; // filtered
			break;
			case WM_MOUSEMOVE:
				if (m_pEngine != NULL)
					m_pEngine->Graphics().OnMouseMove(LOWORD(lParam_in), HIWORD(lParam_in), wParam_in);
			break;
			case WM_ACTIVATEAPP:
			case WM_ACTIVATE:
				if (m_pEngine != NULL)
					m_pEngine->Graphics().SetRendering(true);
			break;
			case WM_KEYDOWN:
			case WM_KEYUP:
				return FilterKeyboard(hWnd_in, uMsg_in, wParam_in, lParam_in);
			break;
		}

		return 0UL;
	}

	/*! \brief Processes messages sent to a window
			   Hooked to intercept messages received by the game window
		\param[in] hWnd_in : handle to the window
		\param[in] uMsg_in : message
		\param[in] wParam_in : additional message information
		\param[in] lParam_in : additional message information
	*/
	LRESULT WINAPI SystemCore::WndProcHook(HWND hWnd_in, UINT uMsg_in, WPARAM wParam_in, LPARAM lParam_in)
	{
		if (FilterMessages(hWnd_in, uMsg_in, wParam_in, lParam_in))
			return 0UL;
		else if (m_pGameWndProc != NULL)
			return m_pGameWndProc(hWnd_in, uMsg_in, wParam_in, lParam_in);
		else
			return ::DefWindowProc(hWnd_in, uMsg_in, wParam_in, lParam_in);
	}


	/*! \brief Processes messages sent to a sub-classed window
			   Hooked to intercept messages received by the game window
		\param[in] hWnd_in : handle to the window
		\param[in] uMsg_in : message
		\param[in] wParam_in : additional message information
		\param[in] lParam_in : additional message information
		\return result of the message processing that depends on the message sent
	*/
	LRESULT CALLBACK SystemCore::SubclassProcHook(HWND hWnd_in, UINT uMsg_in, WPARAM wParam_in, LPARAM lParam_in,  UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
	{
		if (FilterMessages(hWnd_in, uMsg_in, wParam_in, lParam_in))
			return 0UL;
		else
			return ::DefSubclassProc(hWnd_in, uMsg_in, wParam_in, lParam_in);
	}

	/*! \brief Filters key presses
		\param[in] hWnd_in : handle to the window
		\param[in] uMsg_in : message
		\param[in] wParam_in : additional message information
		\param[in] lParam_in : additional message information
		\return 1UL if the message should be filtered; 0UL otherwise
	*/
	LRESULT SystemCore::FilterKeyboard(HWND hWnd_in, UINT uMsg_in, WPARAM wParam_in, LPARAM lParam_in)
	{
		if (hWnd_in == m_hGameWnd && uMsg_in == WM_KEYUP && m_pEngine != NULL)
		{
			bool bCtrl = ((GetAsyncKeyState(VK_CONTROL) & 0x8000) == 0x8000);

			if (wParam_in == VK_F10 && bCtrl)
			{
				m_pEngine->Graphics().ToggleRendering();
				
				return 1UL; // filtered
			}
			else if (wParam_in == VK_F11 && bCtrl)
			{
				m_pEngine->Graphics().SetRendering(false);
				::ShowWindow(m_hGameWnd, SW_MINIMIZE);

				return 1UL; // filtered
			}
			else if (wParam_in == VK_F12 && bCtrl)
			{
				m_pEngine->Graphics().ToggleFPS();

				return 1UL; // filtered
			}
		}

		return 0UL;
	}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void SystemCore::RegisterHooks(HookEngineLib::IHookManager &HookManager_in)
	{
		// register the RegisterClassExA hook
		HookManager_in.RegisterHook("RegisterClassExA", "user32.dll", RegisterClassExA, &SystemCore::RegisterClassExAHook);
		// register the SetWindowSubclass hook
		HookManager_in.RegisterHook("SetWindowSubclass", "comctl32.dll", NULL, &SystemCore::SetWindowSubclassHook);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void SystemCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		m_pRegisterClassExATrampoline = (fnRegisterClassExA)HookManager_in.GetTrampolineFunc("RegisterClassExA");
		m_pSetWindowSubclassTrampoline = (fnSetWindowSubclass)HookManager_in.GetTrampolineFunc("SetWindowSubclass");
	}

	/*! \brief Installs or updates a window subclass callback
		\param[in] hWnd_in : handle of the window being sub-classed
		\param[in] pfnSubclass_in : pointer to a window procedure
		\param[in] uIdSubclass_in : subclass ID
		\param[in] dwRefData_in : reference data (application specific)
		\return TRUE if the subclass callback was successfully installed; FALSE otherwise
	*/
	BOOL WINAPI SystemCore::SetWindowSubclassHook(HWND hWnd_in, SUBCLASSPROC pfnSubclass_in, UINT_PTR uIdSubclass_in, DWORD_PTR dwRefData_in)
	{
		BOOL Result = FALSE;

		m_hGameWnd = hWnd_in;

		if (m_pSetWindowSubclassTrampoline != NULL)
		{
			Result = m_pSetWindowSubclassTrampoline(hWnd_in, pfnSubclass_in, uIdSubclass_in, dwRefData_in);
			
			if (--m_SubClassRef == 0)
			{
				if (m_pSetWindowSubclassTrampoline(hWnd_in, &SystemCore::SubclassProcHook, 0UL, 0UL))
				{
					DWORD dwThreadID;

					// remove the hook since it is no longer needed
					m_pHookManager->UnregisterHook("SetWindowSubclass");
					m_pSetWindowSubclassTrampoline = NULL;
					// create the engine thread
					m_hMainThread = CreateThread(NULL, 0, SystemCore::MainThreadStatic, NULL, 0, &dwThreadID);
				}
			}
		}

		return Result;
	}
}