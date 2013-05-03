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

typedef LRESULT (CALLBACK *SUBCLASSPROC)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
typedef BOOL (WINAPI *fnSetWindowSubclass)(HWND hWnd_in, SUBCLASSPROC pfnSubclass_in, UINT_PTR uIdSubclass_in, DWORD_PTR dwRefData_in);

namespace Windower
{
	class WindowerEngine;

	//! \brief Core module used for Win32 API hooking
	class SystemCore : public WindowerCore, public EventHandler
	{
	public:
		SystemCore();

		void Detach();

		/*! \brief Retrieves the handle to the game window
			\return the handle to the game window
		*/
		HWND GameHWND() const { return m_hWnd; }
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
		void SubclassWindow(HWND hWnd_in);

	protected:
		LRESULT FilterMessages(HWND hWnd_in, UINT uMsg_in, WPARAM wParam_in, LPARAM lParam_in);
		LRESULT FilterKeyboard(HWND hWnd_in, UINT uMsg_in, WPARAM wParam_in, LPARAM lParam_in);
		static DWORD WINAPI MainThreadStatic(LPVOID pParam_in_out);

		//! hook to sub class over the game sub-classing
		fnSetWindowSubclass m_pSetWindowSubclassTrampoline;
		//! Engine thread handle
		HANDLE m_hMainThread;
		//! calling context for the module hooks
		static CallingContext<SystemCore> m_Context;
	};
}

#endif//__SYSTEM_CORE_H__