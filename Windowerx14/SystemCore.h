/**************************************************************************
	created		:	2010-10-10
	filename	: 	SystemCore.h
	author		:	Xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interface with the Win32 API
**************************************************************************/
#ifndef __SYSTEM_CORE_H__
#define __SYSTEM_CORE_H__

namespace Windower
{
	typedef HookEngineLib::IHookManager IHookManager;

	#define FFXIV_WINDOW_CLASSW		_T("RAPTURE")
	#define FFXIV_WINDOW_NAMEA		"FINAL FANTASY XIV"

	class SystemCore : public WindowerCore
	{
	public:
		SystemCore(WindowerEngine &Engine_in);

		// accessors
		HWND				GameHWND()		{ return m_hGameWnd; }
		DWORD				GamePID()		{ return m_dwPID; }

		HANDLE CreateEngineThread();

		// ICoreModule interface implementation
		void RegisterHooks(IHookManager *pHookManager);
		void OnHookInstall(IHookManager *pHookManager);

		LRESULT WndProcHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		ATOM WINAPI RegisterClassExWHook(const WNDCLASSEXW *pWndClass_in);
		HWND CreateWindowExAHook(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y,
								 int nWidth, int nHeight,HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

	protected:
		fnRegisterClassExW		m_pRegisterClassExWTrampoline;
		fnCreateWindowExA		m_pCreateWindowExATrampoline;

		LRESULT FilterKeyboard(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static DWORD WINAPI MainThreadStatic(void* pParam);
		void RestoreWndProc();

		HANDLE					m_hMainThreadHandle;
		WNDPROC					m_pGameWndProc;
		DWORD					m_MainThreadID;

		UINT					m_MinimizeVKey;
		HWND					m_hGameWnd;
		DWORD					m_dwPID;
	};
}

#endif//__SYSTEM_CORE_H__