/**************************************************************************
	created		:	2010-10-10
	filename	: 	SystemCore.cpp
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Core module used for Win32 API hooking
**************************************************************************/
#include "stdafx.h"
#include <ImportTablePatcher.h>
#include <ModuleInjection.h>

#include "BootstrapEngine.h"

#include "CreateProcessHook.h"
#include "CreateWindowExHook.h"

#include "SystemCore.h"

namespace Bootstrap
{
	/*! \brief SystemCore constructor
		\param[in,out] pEngine : a pointer to the windower engine
	*/
	SystemCore::SystemCore(BootstrapEngine &Engine_in_out, HookEngine &HookManager_in_out)
		: m_Engine(Engine_in_out), m_HookManager(HookManager_in_out), m_hIEWnd(nullptr)
	{
		m_AutoLogin = static_cast<BootstrapEngine&>(Engine_in_out).IsAutoLoginActive();

		m_pCreateWindowExWTrampoline = CreateWindowExW;
		m_pCreateProcessTrampoline = CreateProcessW;

		m_Engine.RegisterModule(_T(SYSTEM_MODULE), this);
	}

	/*! \brief CreateWindowExW hook used to start the AutoLogin thread
		\param[in] dwExStyle_in : extended window style of the window being created
		\param[in] lpClassName_in : a null-terminated string or a class atom
		\param[in] lpWindowName_in : the window name
		\param[in] dwStyle_in : the style of the window being created
		\param[in] X_in : the initial horizontal position of the window
		\param[in] Y_in : the initial vertical position of the window
		\param[in] nWidth_in : the width, in device units, of the window
		\param[in] nHeight_in : the height, in device units, of the window
		\param[in] hWndParent_in : handle to the parent or owner window of the window being created
		\param[in] hMenu_in : handle to a menu, or specifies a child-window identifier, depending on the window style
		\param[in] hInstance_in : handle to the instance of the module to be associated with the window
		\param[in] lpParam_in : value to be passed to the window through the CREATESTRUCT structure
		\return a handle to the new window; nullptr otherwise
	*/
	HWND SystemCore::CreateWindowExWHook(DWORD dwExStyle_in, LPCTSTR lpClassName_in, LPCTSTR lpWindowName_in, DWORD dwStyle_in, int X_in, int Y_in,
										 int nWidth_in, int nHeight_in, HWND hWndParent_in, HMENU hMenu_in, HINSTANCE hInstance_in, LPVOID lpParam_in)
	{
		HWND hWndResult = m_pCreateWindowExWTrampoline(dwExStyle_in, lpClassName_in, lpWindowName_in, dwStyle_in, X_in, Y_in,
													   nWidth_in, nHeight_in, hWndParent_in, hMenu_in, hInstance_in, lpParam_in);
		DWORD_PTR ClassAtom = reinterpret_cast<DWORD_PTR>(lpClassName_in);

		if (lpClassName_in != nullptr && (ClassAtom & 0xFFFF0000U) != NULL && _tcscmp(TARGET_CLASSNAME, lpClassName_in) == 0)
		{
			m_hIEWnd = hWndResult;
			m_Engine.InvokeAutoLogin();			
		}

		return hWndResult;
	}

	/*! \brief CreateProcess hook used to inject the bootstrap/windower DLL into the game process
		\param[in] lpApplicationName_in : the name of the module to be executed
		\param[in,out] lpCommandLine_in_out : the command line to be executed
		\param[in] lpProcessAttributes_in : pointer to a SECURITY_ATTRIBUTES structure
		\param[in] lpThreadAttributes_in : pointer to a SECURITY_ATTRIBUTES structure
		\param[in] bInheritHandles_in : flag specifying if each inheritable handle in the calling process is inherited by the new process
		\param[in] dwCreationFlags_in : the flags that control the priority class and the creation of the process
		\param[in] lpEnvironment_in : pointer to the environment block for the new process
		\param[in] lpCurrentDirectory_in : the full path to the current directory for the process
		\param[in] lpStartupInfo_in : pointer to a STARTUPINFO or STARTUPINFOEX structure
		\param[out] lpProcessInformation_out : pointer to a PROCESS_INFORMATION structure that receives identification information about the new process
		\return Non-zero if the process was created successfully; FALSE otherwise
	*/
	BOOL SystemCore::CreateProcessHook(LPCTSTR lpApplicationName_in, LPTSTR lpCommandLine_in_out, LPSECURITY_ATTRIBUTES lpProcessAttributes_in,
									   LPSECURITY_ATTRIBUTES lpThreadAttributes_in, BOOL bInheritHandles_in, DWORD dwCreationFlags_in,
									   LPVOID lpEnvironment_in, LPCTSTR lpCurrentDirectory_in, LPSTARTUPINFO lpStartupInfo_in, 
									   LPPROCESS_INFORMATION lpProcessInformation_out) const
	{
		// at least one of the strings should be valid
		if (m_pCreateProcessTrampoline == nullptr || (lpCommandLine_in_out == nullptr && lpApplicationName_in == nullptr))
			return FALSE;

		string_t WorkingDir = m_Engine.GetWorkingDir();
		TCHAR InjectedDLL[_MAX_PATH] = { '\0', };
		BOOL Result = FALSE;
		bool x64 = false;

		if (lpApplicationName_in == nullptr && lpCommandLine_in_out != nullptr)
		{
			if (_tcsstr(lpCommandLine_in_out, TARGET_PROCESS_GAME) != nullptr)
			{
				_stprintf_s(InjectedDLL, _MAX_PATH, _T("%sx14-2core.x86.dll"), WorkingDir.c_str());
				Result = TRUE;
			}
			else if (_tcsstr(lpCommandLine_in_out, TARGET_PROCESS_GAME_DX11) != nullptr)
			{
#ifdef _DEBUG
				_stprintf_s(InjectedDLL, _MAX_PATH, _T("%sx14-2dbg.x64.dll"), WorkingDir.c_str());
#else
				_stprintf_s(InjectedDLL, _MAX_PATH, _T("%sx14-2core.x64.dll"), WorkingDir.c_str());
#endif
				Result = TRUE;
				x64 = true;
			}
			else
			{
				_stprintf_s(InjectedDLL, _MAX_PATH, _T("%sbootstrap.dll"), WorkingDir.c_str());
				Result = TRUE;
			}
		}

		if (Result)
		{
			if (x64)
			{
#ifdef _DEBUG
				const TCHAR *pFormatString = _T("%sx14-2core.x64.dll");
#else
				const TCHAR *pFormatString = _T("%s");
#endif
				PROCESS_INFORMATION processInfo = { nullptr };
				TCHAR UserData[_MAX_PATH] = { '\0' };
				DWORD DataLength;
				string_t exePath;

				format(exePath, _T("%sgame\\%s"), m_Engine.GetGameDirectory(), TARGET_PROCESS_GAME_DX11);
				DataLength = _stprintf_s(UserData, _MAX_PATH, pFormatString, WorkingDir.c_str());

				if (DataLength != -1 && DataLength > 0UL)
				{
					Result = InjectModule::CreateProcessEx(exePath, processInfo, lpCommandLine_in_out, dwCreationFlags_in, InjectedDLL,
														   FUNCTION_HASH, UserData, (DataLength + 1) * sizeof(TCHAR));
				}
				else
					Result = FALSE;
			}
			else
			{
				char InjectedDLLA[_MAX_PATH] = { '\0', };

				WideCharToMultiByte(CP_ACP, 0, InjectedDLL, _MAX_PATH, InjectedDLLA, _MAX_PATH, nullptr, nullptr);

				// attach the DLL to the next process in the chain
				Result = DetourCreateProcessWithDllExW(lpApplicationName_in, lpCommandLine_in_out, lpProcessAttributes_in,
													   lpThreadAttributes_in, bInheritHandles_in, dwCreationFlags_in,
													   lpEnvironment_in, lpCurrentDirectory_in, lpStartupInfo_in,
													   lpProcessInformation_out, InjectedDLLA, nullptr);
			}
		}
		else
		{
			Result = CreateProcessW(lpApplicationName_in, lpCommandLine_in_out, lpProcessAttributes_in,
									lpThreadAttributes_in, bInheritHandles_in, dwCreationFlags_in, 
									lpEnvironment_in, lpCurrentDirectory_in, lpStartupInfo_in, 
									lpProcessInformation_out);
		}
		
		return Result;
	}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void SystemCore::RegisterHooks(HookEngineLib::IHookManager &HookManager_in)
	{
		// CreateWindowEx hook used to start the AutoLogin thread
		if (m_AutoLogin)
			HookManager_in.RegisterHook("CreateWindowExW", "User32.dll", CreateWindowExW, ::CreateWindowExWHook);
		// CreateProcessW hook used to inject the bootstrap/windower DLL into the game process (Windows 7)
		IATPatcher::PatchIAT(GetModuleHandle(nullptr), "Kernel32.dll", "CreateProcessW", reinterpret_cast<PVOID*>(&m_pCreateProcessTrampoline), ::CreateProcessHook);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void SystemCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		if (m_AutoLogin)
			m_pCreateWindowExWTrampoline = static_cast<fnCreateWindowExW>(HookManager_in.GetTrampolineFunc("CreateWindowExW"));
	}

	/*! \brief Registers the services of the module
		\return true if the services were registered; false otherwise
	*/
	bool SystemCore::RegisterServices()
	{
		// register the services
		return (RegisterService(_T(IE_SERVER_HWND_SERVICE), true) != nullptr);
	}

	/*! \brief Invokes a command registered with the specified service
		\param[in] ServiceName_in : the name of the service
		\param[in] Params_in : the input parameters
		\return true if the command was invoked successfully; false otherwise
	*/
	bool SystemCore::Invoke(const string_t& ServiceName_in, PluginFramework::ServiceParam &Params_in)
	{
		if (ServiceName_in.compare(_T(IE_SERVER_HWND_SERVICE)) == 0
		 && Params_in.pData != nullptr && Params_in.DataType.compare(_T("HWND*")) == 0)
		{
			*static_cast<HWND*>(Params_in.pData) = m_hIEWnd;

			return (m_hIEWnd != nullptr);
		}

		return false;
	}
}