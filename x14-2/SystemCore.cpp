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

#include "IRenderable.h"
#include "IDeviceCreateSubscriber.h"

#include "IEventInterface.h"
#include "EventHandler.h"

#include "WindowerCore.h"
#include "SystemCore.h"

namespace Windower
{
	WindowerCore::CallingContext<SystemCore> SystemCore::m_Context;
	WNDPROC SystemCore::m_pGameWndProc = NULL;

	/*! \brief SystemCore constructor
		\param[in,out] pEngine : the windower engine
	*/
	SystemCore::SystemCore()
		: WindowerCore(_T(SYSTEM_MODULE)), m_hMainThread(NULL),
		  m_pSetWindowSubclassTrampoline(NULL),
		  m_pRegisterClassExATrampoline(NULL)
	{
		// set the calling context for the hooks
		m_Context.Set(this);
	}

	/*! \brief Starts the main thread of the windower engine
		\param[in,out] pParam_in_out : a pointer to the windower engine
		\return the exit code of the thread
	*/
	DWORD WINAPI SystemCore::MainThread(LPVOID pParam_in_out)
	{
		if (m_pEngine != NULL)
			return m_pEngine->MainThread();

		return 0xFFFFFFFFUL;
	}

	//! \brief Restores the target WndProc
	void SystemCore::Detach()
	{
		if (m_pGameWndProc != NULL)
		{
			// remove the engine sub-classing procedure
			RemoveWindowSubclass(m_hWnd, &SystemCore::SubclassProcHook, 0UL);
			// restore the original game window procedure (this will kill any game sub-classing)
			SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_pGameWndProc);
			// reset the pointer to the game procedure
			m_pGameWndProc = NULL;
		}
	}

	/*! \brief Registers a window class
			   Hooked to hijack the target process WndProc function
		\param[in] pWndClass_in : pointer to a WNDCLASSEX structure
		\return class atom that uniquely identifies the class being registered if successfuly; NULL otherwise
	*/
	ATOM WINAPI SystemCore::RegisterClassExAHook(const WNDCLASSEXA *pWndClass_in)
	{
		ATOM Result = 0L;

		if (m_Context->m_pRegisterClassExATrampoline != NULL)
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

			Result = m_Context->m_pRegisterClassExATrampoline(pWndClass);

			if (Uninstall)
			{
				// remove the hook since it is no longer needed
				m_pHookManager->UninstallHook("RegisterClassExA");
				m_pHookManager->UnregisterHook("RegisterClassExA");
			}
		}

		return Result;
	}

	/*! \brief Filters the keyboard message received by the game window
		\param[in] hWnd_in : handle to the window
		\param[in] uMsg_in : message
		\param[in] wParam_in : additional message information
		\param[in] lParam_in : additional message information
		\return 1UL if the message should be filtered; 0UL otherwise
	*/
	LRESULT SystemCore::FilterSubClassMessages(HWND hWnd_in, UINT uMsg_in, WPARAM wParam_in, LPARAM lParam_in)
	{
		switch (uMsg_in)
		{
			case WM_LBUTTONDOWN:
				return OnLButtonDown(LOWORD(lParam_in), HIWORD(lParam_in), wParam_in);
			break;
			case WM_LBUTTONUP:
				return OnLButtonUp(LOWORD(lParam_in), HIWORD(lParam_in), wParam_in);
			break;
			case WM_MOUSEMOVE:
				return OnMouseMove(LOWORD(lParam_in), HIWORD(lParam_in), wParam_in);
			break;
			case WM_WINDOWPOSCHANGED:
			{
				WINDOWPOS *pWindowPos = reinterpret_cast<WINDOWPOS*>(lParam_in);

				return OnSize(pWindowPos->cx, pWindowPos->cy, pWindowPos->flags);
			}
			break;
		}

		return 0UL;
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
			case WM_KEYDOWN:
				return OnKeyDown(wParam_in, LOWORD(lParam_in));
			case WM_KEYUP:
				return OnKeyUp(wParam_in, LOWORD(lParam_in));
			break;
			case WM_ACTIVATEAPP:
			case WM_ACTIVATE:
				return OnActivate(LOWORD(wParam_in) != WA_INACTIVE, HIWORD(wParam_in) != 0U);
			break;
			case WM_PAINT:
				return OnPaint();
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
		if (m_Context->FilterMessages(hWnd_in, uMsg_in, wParam_in, lParam_in))
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
		if (m_Context->FilterSubClassMessages(hWnd_in, uMsg_in, wParam_in, lParam_in))
			return 1UL;

		return ::DefSubclassProc(hWnd_in, uMsg_in, wParam_in, lParam_in);
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
		m_pSetWindowSubclassTrampoline = (fnSetWindowSubclass)HookManager_in.GetTrampolineFunc("SetWindowSubclass");
		m_pRegisterClassExATrampoline = (fnRegisterClassExA)HookManager_in.GetTrampolineFunc("RegisterClassExA");
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

		if (m_Context->m_pSetWindowSubclassTrampoline != NULL)
		{
			m_Context->m_hWnd = hWnd_in;

			if (m_Context->m_pSetWindowSubclassTrampoline != NULL)
			{
				static DWORD CallCount = 2UL;

				Result = m_Context->m_pSetWindowSubclassTrampoline(hWnd_in, pfnSubclass_in, uIdSubclass_in, dwRefData_in);
				// remove the hook since it is no longer needed
				m_pHookManager->UnregisterHook("SetWindowSubclass");

				if (--CallCount == 0UL)
					m_Context->SubclassWindow(hWnd_in);
			}
			else
			{
				Result = SetWindowSubclass(hWnd_in, pfnSubclass_in, uIdSubclass_in, dwRefData_in);
			}
		}

		return Result;
	}

	void SystemCore::SubclassWindow(HWND hWnd_in)
	{
		if (SetWindowSubclass(hWnd_in, &SystemCore::SubclassProcHook, 0UL, 0UL))
		{
			DWORD dwThreadID;

			m_pEngine->SetWnd(hWnd_in);
			// create the engine thread
			m_hMainThread = CreateThread(NULL, 0, SystemCore::MainThread, NULL, 0, &dwThreadID);
		}
	}
}