/**************************************************************************
	created		:	2010-10-10
	filename	: 	SystemCore.h
	author		:	Xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interface with the Win32 API
**************************************************************************/
#ifndef __SYSTEM_CORE_H__
#define __SYSTEM_CORE_H__

namespace Bootstrap
{
	typedef HookEngineLib::IHookManager IHookManager;

	#define TARGET_CLASSNAME _T("Shell DocObject View")

	class SystemCore : public Windower::WindowerCore
	{
	public:
		SystemCore(Windower::PluginEngine &Engine_in);

		// ICoreModule interface implementation
		void RegisterHooks(IHookManager *pHookManager);
		void OnHookInstall(IHookManager *pHookManager);

		HWND CreateWindowExWHook(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int X, int Y,
								 int nWidth, int nHeight,HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
		BOOL CreateProcessHook(LPCTSTR lpApplicationName, LPTSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
							   LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags,
							   LPVOID lpEnvironment, LPCTSTR lpCurrentDirectory, LPSTARTUPINFO lpStartupInfo,
							   LPPROCESS_INFORMATION lpProcessInformation);
		BOOL ShellExecuteExHook(LPSHELLEXECUTEINFO lpExecInfo);


	protected:
		fnCreateWindowExW		m_pCreateWindowExWTrampoline;
		fnCreateProcess			m_pCreateProcessTrampoline;
		fnShellExecuteEx		m_pShellExecuteExTrampoline;
	};
}

#endif//__SYSTEM_CORE_H__