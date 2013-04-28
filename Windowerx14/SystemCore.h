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
#define SYSTEM_MODULE		"System"

namespace Windower
{
	class WindowerEngine;

	//! \brief Core module used for Win32 API hooking
	class SystemCore : public WindowerCore
	{
		class CallingContext
		{
		public:
			CallingContext(fnSetWindowSubclass pSetWindowSubclassTrampoline_in,
						   fnRegisterClassExA pRegisterClassExATrampoline_in,
						   HWND &hGameWnd_in, HANDLE &hMainThread_in)
				: m_pRegisterClassExATrampoline(pRegisterClassExATrampoline_in), 
				  m_pSetWindowSubclassTrampoline(pSetWindowSubclassTrampoline_in),
				  m_hGameWnd(hGameWnd_in),m_hMainThread(hMainThread_in) {}

			//! hook to sub class over the game sub-classing
			fnSetWindowSubclass m_pSetWindowSubclassTrampoline;
			//! function pointer to the original RegisterClassExA function
			fnRegisterClassExA m_pRegisterClassExATrampoline;		
			//! the handle to the game window
			HWND &m_hGameWnd;
			//! Engine thread handle
			HANDLE &m_hMainThread;
		};

	public:
		SystemCore();
		~SystemCore();

		void RestoreWndProc();

		/*! \brief Retrieves the handle to the game window
			\return the handle to the game window
		*/
		HWND GameHWND() const { return m_hGameWnd; }
		/*! \brief Retrieves the handle to the engine thread
			\return the handle to the engine thread
		*/
		HANDLE GetMainThreadHandle() { return m_hMainThread; }

		// ICoreModule interface implementation
		void RegisterHooks(HookEngineLib::IHookManager &HookManager_in);
		void OnHookInstall(HookEngineLib::IHookManager &HookManager_in);

		// hooks
		static LRESULT CALLBACK SubclassProcHook(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam,  UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
		static BOOL WINAPI SetWindowSubclassHook(HWND hWnd_in, SUBCLASSPROC pfnSubclass_in, UINT_PTR uIdSubclass_in, DWORD_PTR dwRefData_in);
		static LRESULT WINAPI WndProcHook(HWND hWnd_in, UINT uMsg_in, WPARAM wParam_in, LPARAM lParam_in);
		static ATOM WINAPI RegisterClassExAHook(const WNDCLASSEXA *pWndClass_in);

	protected:
		static LRESULT FilterMessages(HWND hWnd_in, UINT uMsg_in, WPARAM wParam_in, LPARAM lParam_in);
		static LRESULT FilterKeyboard(HWND hWnd_in, UINT uMsg_in, WPARAM wParam_in, LPARAM lParam_in);
		static DWORD WINAPI MainThreadStatic(LPVOID pParam_in_out);

		//! the original WndProc of the game
		static WNDPROC m_pGameWndProc;
		//! the handle to the game window
		HWND m_hGameWnd;
		//! Engine thread handle
		HANDLE m_hMainThread;
		//! calling context for the module hooks
		static CallingContext *m_pContext;
	};
}

#endif//__SYSTEM_CORE_H__