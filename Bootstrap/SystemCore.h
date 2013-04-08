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
	class BootstrapEngine;
	class ModuleService;

	//! the class name of the target window
	#define TARGET_CLASSNAME _T("Shell DocObject View")

	//! \brief Core module used for Win32 API hooking
	class SystemCore : public Windower::ICoreModule
	{
	public:
		SystemCore(BootstrapEngine &Engine_in_out, HookEngine &HookManager_in_out);

		// ICoreModule interface implementation
		void RegisterHooks(HookEngineLib::IHookManager &HookManager_in);
		void OnHookInstall(HookEngineLib::IHookManager &HookManager_in);

		// hooks
		HWND CreateWindowExWHook(DWORD dwExStyle_in, LPCTSTR lpClassName_in, LPCTSTR lpWindowName_in, DWORD dwStyle_in, int X_in, int Y_in,
								 int nWidth_in, int nHeight_in, HWND hWndParent_in, HMENU hMenu_in, HINSTANCE hInstance_in, LPVOID lpParam_in);
		BOOL CreateProcessHook(LPCTSTR lpApplicationName_in, LPTSTR lpCommandLine_in_out, LPSECURITY_ATTRIBUTES lpProcessAttributes_in,
							   LPSECURITY_ATTRIBUTES lpThreadAttributes_in, BOOL bInheritHandles_in, DWORD dwCreationFlags_in,
							   LPVOID lpEnvironment_in, LPCTSTR lpCurrentDirectory_in, LPSTARTUPINFO lpStartupInfo_in, 
							   LPPROCESS_INFORMATION lpProcessInformation_out);

		bool RegisterServices() { return false; }
		void UnsubscribeAll(PluginFramework::IPlugin* pPlugin_in) {}
		bool Subscribe(const string_t& ServiceName_in, PluginFramework::IPlugin* pPlugin_in) { return false; }
		bool Unsubscribe(const string_t& ServiceName_in, PluginFramework::IPlugin* pPlugin_in) { return false; }
		bool Invoke(const string_t& ServiceName_in, const PluginFramework::ServiceParam &Params_in) { return true; }
		void OnSubscribe(Windower::ModuleService *pService_in_out, PluginFramework::IPlugin* pPlugin_in) {}
		void OnUnsubscribe(Windower::ModuleService *pService_in_out, PluginFramework::IPlugin* pPlugin_in) {}
		Windower::ModuleService* RegisterService(const string_t& ServiceName_in, bool InvokePermission_in) { return NULL; }

	protected:
		//! flag specifying if the AutoLogin is active
		bool m_AutoLogin;
		//! function pointer to the original CreateWindowExW function
		fnCreateWindowExW		m_pCreateWindowExWTrampoline;
		//! function pointer to the original CreateProcess function
		fnCreateProcess			m_pCreateProcessTrampoline;
		//! a reference to the windower engine
		BootstrapEngine		   &m_Engine;
		//! a reference to the hook manager
		HookEngine			   &m_HookManager;
	};
}

#endif//__SYSTEM_CORE_H__