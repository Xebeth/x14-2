/**************************************************************************
	created		:	2010-10-10
	filename	: 	SystemCore.h
	author		:	Xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Core module used for Win32 API hooking
**************************************************************************/
#ifndef __SYSTEM_CORE_H__
#define __SYSTEM_CORE_H__

namespace Windower
{
	class ModuleService;
}

namespace Bootstrap
{
	typedef HookEngineLib::IHookManager IHookManager;

	//! the class name of the target window
	#define TARGET_CLASSNAME _T("Shell DocObject View")

	//! \brief Core module used for Win32 API hooking
	class SystemCore : public Windower::WindowerCore
	{
	public:
		SystemCore(Windower::PluginEngine &Engine_in_out, HookEngine &HookManager_in_out);

		// ICoreModule interface implementation
		void RegisterHooks(IHookManager &HookManager_in);
		void OnHookInstall(IHookManager &HookManager_in);

		// hooks
		HWND CreateWindowExWHook(DWORD dwExStyle_in, LPCTSTR lpClassName_in, LPCTSTR lpWindowName_in, DWORD dwStyle_in, int X_in, int Y_in,
								 int nWidth_in, int nHeight_in, HWND hWndParent_in, HMENU hMenu_in, HINSTANCE hInstance_in, LPVOID lpParam_in);
		BOOL CreateProcessHook(LPCTSTR lpApplicationName_in, LPTSTR lpCommandLine_in_out, LPSECURITY_ATTRIBUTES lpProcessAttributes_in,
							   LPSECURITY_ATTRIBUTES lpThreadAttributes_in, BOOL bInheritHandles_in, DWORD dwCreationFlags_in,
							   LPVOID lpEnvironment_in, LPCTSTR lpCurrentDirectory_in, LPSTARTUPINFO lpStartupInfo_in, 
							   LPPROCESS_INFORMATION lpProcessInformation_out);

	protected:
		//! flag specifying if the AutoLogin is active
		bool m_AutoLogin;
		//! function pointer to the original CreateWindowExW function
		fnCreateWindowExW		m_pCreateWindowExWTrampoline;
		//! function pointer to the original CreateProcess function
		fnCreateProcess			m_pCreateProcessTrampoline;
	};
}

#endif//__SYSTEM_CORE_H__