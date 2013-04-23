/**************************************************************************
	created		:	2010-10-10
	filename	: 	SystemCore.h
	author		:	Xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interface with the Win32 API
**************************************************************************/
#ifndef __SYSTEM_CORE_H__
#define __SYSTEM_CORE_H__

//! the class name of the target window
#define FFXIV_WINDOW_CLASSA	"FFXIVGAME"
#define SYSTEM_MODULE "System"

namespace Windower
{
	class WindowerEngine;



	//! \brief Core module used for Win32 API hooking
	class SystemCore : public WindowerCore
	{
	public:
		SystemCore();

		/*! \brief Retrieves the handle to the game window
			\return the handle to the game window
		*/
		HWND GameHWND() const { return m_hGameWnd; }
		/*! \brief Retrieves the ID of the game process
			\return the ID of the game process
		*/
		DWORD GamePID() const { return m_dwPID; }

		HANDLE CreateEngineThread();

		// ICoreModule interface implementation
		void RegisterHooks(HookEngineLib::IHookManager &HookManager_in);
		void OnHookInstall(HookEngineLib::IHookManager &HookManager_in);

		// hooks
		BOOL SetCursorPosAHook(INT X_in, INT Y_in);
		ATOM RegisterClassExAHook(const WNDCLASSEXA *pWndClass_in);
		LRESULT WndProcHook(HWND hWnd_in, UINT uMsg_in, WPARAM wParam_in, LPARAM lParam_in);
		HWND CreateWindowExAHook(DWORD dwExStyle_in, LPCSTR lpClassName_in, LPCSTR lpWindowName_in, DWORD dwStyle_in, int X_in, int Y_in, 
								 int nWidth_in, int nHeight_in, HWND hWndParent_in, HMENU hMenu_in, HINSTANCE hInstance_in, LPVOID lpParam_in);

	protected:
		LRESULT FilterKeyboard(HWND hWnd_in, UINT uMsg_in, WPARAM wParam_in, LPARAM lParam_in);
		static DWORD WINAPI MainThreadStatic(LPVOID pParam_in_out);
		void RestoreWndProc();

		//! function pointer to the original RegisterClassExA function
		fnRegisterClassExA		m_pRegisterClassExATrampoline;
		//! function pointer to the original CreateWindowExA function
		fnCreateWindowExA		m_pCreateWindowExATrampoline;
		//! function pointer to the original SetCursorPos function
		fnSetCursorPos			m_pSetCursorPosTrampoline;

		//! the handle to the main thread
		HANDLE m_hMainThreadHandle;
		//! the original WndProc of the game
		WNDPROC	 m_pGameWndProc;
		//! the ID of the main thread
		DWORD m_MainThreadID;
		//! the handle to the game window
		HWND m_hGameWnd;
		//! the ID of the game process
		DWORD m_dwPID;
	};
}

#endif//__SYSTEM_CORE_H__