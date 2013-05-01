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
#include "IDeviceCreateSubscriber.h"

#include "WindowerCore.h"
#include "GraphicsCore.h"
#include "SystemCore.h"

namespace Windower
{

	SystemCore::CallingContext *SystemCore::m_pContext = NULL;

	/*! \brief SystemCore constructor
		\param[in,out] pEngine : the windower engine
	*/
	SystemCore::SystemCore()
		: WindowerCore(_T(SYSTEM_MODULE)),
		  m_hGameWnd(NULL), m_hMainThread(NULL) {}
	
	//! \brief SystemCore constructor
	SystemCore::~SystemCore()
	{
		if (m_pContext != NULL)
		{
			delete m_pContext;
			m_pContext = NULL;
		}
	}

	/*! \brief Starts the main thread of the windower engine
		\param[in,out] pParam_in_out : a pointer to the windower engine
		\return the exit code of the thread
	*/
	DWORD WINAPI SystemCore::MainThreadStatic(LPVOID pParam_in_out)
	{
		if (m_pEngine != NULL)
		{
			DWORD Result = m_pEngine->MainThread();

			// detach the DLL
			::FreeLibraryAndExitThread((HMODULE)Result, 0UL);
		}

		return 0xFFFFFFFFUL;
	}

	//! \brief Restores the target WndProc
	void SystemCore::Detach()
	{
		// remove the engine sub-classing procedure
		RemoveWindowSubclass(m_hGameWnd, &SystemCore::SubclassProcHook, 0UL);
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
			case WM_WINDOWPOSCHANGED:
				if (m_pEngine != NULL)
				{
					WINDOWPOS *pWindowPos = reinterpret_cast<WINDOWPOS*>(lParam_in);

					m_pEngine->Graphics().SetWindowSize(pWindowPos->cx, pWindowPos->cy);
				}
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
		// Ctrl + <key> shortcuts
		if (uMsg_in == WM_KEYUP && m_pEngine != NULL)
		{
			if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) == 0x8000)
			{
				switch(wParam_in)
				{
#ifdef _DEBUG
					case VK_F4:
						m_pEngine->OnClose();

						return 1UL;
					break;
#endif // _DEBUG
					case VK_F10:
						m_pEngine->Graphics().ToggleRendering();
				
						return 1UL; // filtered
					break;
					case VK_F11:
						m_pEngine->Graphics().SetRendering(false);
						::ShowWindow(hWnd_in, SW_MINIMIZE);
				
						return 1UL; // filtered
					break;
					case VK_F12:
						m_pEngine->Graphics().ToggleFPS();
				
						return 1UL; // filtered
					break;
#ifdef _DEBUG
					case 'X':
					case 'x':
						m_pEngine->Exit(std::string());

						return 1UL;
					break;
#endif // _DEBUG
				}
			}
		}

		return 0UL;
	}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void SystemCore::RegisterHooks(HookEngineLib::IHookManager &HookManager_in)
	{
		// register the SetWindowSubclass hook
		HookManager_in.RegisterHook("SetWindowSubclass", "comctl32.dll", NULL, &SystemCore::SetWindowSubclassHook);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void SystemCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		fnSetWindowSubclass pSetWindowSubclassTrampoline = (fnSetWindowSubclass)HookManager_in.GetTrampolineFunc("SetWindowSubclass");

		if (m_pContext == NULL)
		{
			// create the calling context
			m_pContext = new CallingContext(pSetWindowSubclassTrampoline,
											m_hGameWnd, m_hMainThread);
		}	
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

		if (m_pContext != NULL && m_pContext->m_pSetWindowSubclassTrampoline != NULL)
		{
			m_pContext->m_hGameWnd = hWnd_in;

			if (m_pContext->m_pSetWindowSubclassTrampoline != NULL)
			{
				static DWORD CallCount = 2UL;

				Result = m_pContext->m_pSetWindowSubclassTrampoline(hWnd_in, pfnSubclass_in, uIdSubclass_in, dwRefData_in);

				if (--CallCount == 0UL && m_pContext->m_pSetWindowSubclassTrampoline(hWnd_in, &SystemCore::SubclassProcHook, 0UL, 0UL))
				{
					DWORD dwThreadID;

					// remove the hook since it is no longer needed
					m_pHookManager->UnregisterHook("SetWindowSubclass");
					// create the engine thread
					m_pContext->m_hMainThread = CreateThread(NULL, 0, SystemCore::MainThreadStatic, NULL, 0, &dwThreadID);
					// delete the context
					delete m_pContext;
					m_pContext = NULL;
				}
			}
		}

		return Result;
	}
}