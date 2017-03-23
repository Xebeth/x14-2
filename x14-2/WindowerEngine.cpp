/**************************************************************************
	created		:	2011-06-03
	filename	: 	WindowerEngine.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <d3d11.h>

#include "WindowerEngine.h"

#include "IRenderable.h"
#include "IDeviceCreateSubscriber.h"
#include "ISwapChainCreateSubscriber.h"

#include "IEventInterface.h"
#include "EventHandler.h"

#include "WindowerCore.h"
#include "CmdLineCore.h"
#include "GameChatCore.h"
#include "GraphicsCore.h"
#include "SystemCore.h"
#include "PlayerCore.h"

#include "ModuleService.h"
#include "FormatChatMsgService.h"

#include "Timer.h"

//#define _TESTING

#if defined _DEBUG && defined _TESTING
	#include "IGameChatPlugin.h"
	#include "TestCore.h"
#endif // _DEBUG

extern HINSTANCE g_hDllInstace;

namespace Windower
{
	WindowerEngine::CallingContext<WindowerEngine> WindowerEngine::m_Context;

	/*! \brief WindowerEngine constructor
		\param[in] pConfigFile_in : path to the configuration file
	*/
	WindowerEngine::WindowerEngine(const string_t &WorkingDir_in, const TCHAR *pConfigFile_in)
		: PluginEngine(WorkingDir_in, pConfigFile_in), m_hGameWnd(nullptr),
		  m_bDetached(false), m_bShutdown(false), m_pGameChatCore(nullptr),
		  m_pSystemCore(nullptr), m_pPlayerCore(nullptr), m_pUpdateTimer(nullptr),
		  m_pGraphicsCore(nullptr), m_pCmdLineCore(nullptr), m_pTextLabel(nullptr), 
		  m_MacroThreadID(ThreadState::NONE), m_dwPID(0UL), m_MacroThreadState(0L)
	{
		// set the calling context for the hooks
		m_Context.Set(this);
		// initialize synchronization locks
//		InitializeCriticalSection(&m_PluginLock);
//		InitializeCriticalSection(&m_MacroLock);
		// set the static members of modules
		WindowerCore::Initialize(this, &m_HookManager);
		// create the system core module
		m_pSystemCore = new SystemCore;
		// testing
#ifdef _DEBUG
	#ifdef _TESTING
		// create the testing module
		m_pTestCore = new TestCore;
	#else
		m_pTestCore = nullptr;
	#endif
#endif // _DEBUG

		// check the settings and load the default profile
		if (m_pSettingsManager->IsGamePathValid() && m_pSettingsManager->LoadDefaultProfile(m_Settings))
		{
			// create the game chat module
			m_pGameChatCore = new GameChatCore;
			// create the command line module
			m_pCmdLineCore = new CmdLineCore;
			m_pCmdLineCore->SetMacroDirectry(m_pSettingsManager->GetWorkingDir() + MACROS_DIRECTORY);
			// create the player data module
			m_pPlayerCore = new PlayerCore;
			// create the graphics module
			m_pGraphicsCore = new GraphicsCore(m_Settings.GetVSync());
			// add the graphics core as an event handler
			m_pSystemCore->AddUIH(m_pGraphicsCore);
		}
	}

	/*! \brief WindowerEngine destructor */
	WindowerEngine::~WindowerEngine()
	{
		if (m_pGameChatCore != nullptr)
		{
			delete m_pGameChatCore;
			m_pGameChatCore = nullptr;
		}

		if (m_pGraphicsCore != nullptr)
		{
			delete m_pGraphicsCore;
			m_pGraphicsCore = nullptr;
		}

		if (m_pPlayerCore != nullptr)
		{
			delete m_pPlayerCore;
			m_pPlayerCore = nullptr;
		}

		if (m_pCmdLineCore != nullptr)
		{
			delete m_pCmdLineCore;
			m_pCmdLineCore = nullptr;
		}

		if (m_pSystemCore != nullptr)
		{
			delete m_pSystemCore;
			m_pSystemCore = nullptr;
		}

		if (m_pUpdateTimer != nullptr)
		{
			delete m_pUpdateTimer;
			m_pUpdateTimer = nullptr;
		}

#if defined _DEBUG && defined _TESTING
		if (m_pTestCore != nullptr)
		{
			delete m_pTestCore;
			m_pTestCore = nullptr;
		}
#endif // _DEBUG

		// delete the critical sections last
//		DeleteCriticalSection(&m_PluginLock);
//		DeleteCriticalSection(&m_MacroLock);
	}

	bool WindowerEngine::IsPlayerLoggedIn() const
	{
		return (m_pPlayerCore != nullptr && m_pPlayerCore->IsLoggedIn());
	}

	//! \brief Initializes the plugins
	bool WindowerEngine::InitializePlugins()
	{
		if (m_pPluginManager != nullptr)
		{
			// >>> Critical section
			LockEngineThread();
			// list the available plugins compatible with windower
			m_pPluginManager->ListPlugins(m_WorkingDir + PLUGIN_DIRECTORY,
										  PLUGIN_COMPATIBILITY_WINDOWER);
			// load active plugins
			LoadPlugins(m_Settings.GetActivePlugins());

			return UnlockEngineThread();
			// Critical section <<<
		}

		return false;
	}

	/*! \brief Installs the internal hooks used by the windower
		\return true if the hooks were installed successfully; false otherwise
	*/
	bool WindowerEngine::Attach()
	{
		CoreModules::const_iterator ModuleIt, EndIt = m_Modules.cend();
		ICoreModule *pModule = nullptr;

		// register the hooks
		for (ModuleIt = m_Modules.cbegin(); ModuleIt != EndIt; ++ModuleIt)
		{
			pModule = ModuleIt->second;

			if (pModule != nullptr)
				pModule->RegisterHooks(m_HookManager);
		}
		// install the hooks
		if (m_HookManager.InstallRegisteredHooks())
		{
			for (ModuleIt = m_Modules.cbegin(); ModuleIt != EndIt; ++ModuleIt)
			{
				pModule = ModuleIt->second;

				if (pModule != nullptr)
					pModule->OnHookInstall(m_HookManager);
			}
		}
		// register services in a separate loop since OnHookInstall could have added more modules
		for (ModuleIt = m_Modules.cbegin(); ModuleIt != EndIt; ++ModuleIt)
		{
			pModule = ModuleIt->second;

			if (pModule != nullptr)
				pModule->RegisterServices();
		}
		// terminate sigscan to free the copy of the process memory
		m_HookManager.GetSigScan().Clear();

		return true;
	}

	/*! \brief Uninstalls the internal hooks used by the windower
		\return true if the hooks were uninstalled successfully; false otherwise
	*/
	bool WindowerEngine::Detach()
	{
		if (m_bDetached == false)
		{
			// clear all the player data
			if (m_pPlayerCore != nullptr)
				m_pPlayerCore->Detach();
			// stop the update timer
			if (m_pUpdateTimer != nullptr)
				m_pUpdateTimer->Stop();
			// restore the vtables of the Direct3D objects
			if (m_pGraphicsCore != nullptr)
				m_pGraphicsCore->Detach();
			// restore the window procedures
			if (m_pSystemCore != nullptr)
				m_pSystemCore->Detach();
			// unregister the hooks
			m_HookManager.Shutdown();
			// uninstall the plugins
			PluginEngine::Detach();
			
			m_bDetached = true;
		}

		return m_bDetached;
	}

	void WindowerEngine::ShutdownEngine(bool UnloadDLL_in)
	{
		// terminate the engine thread
		m_bShutdown = true;

		if (m_pSystemCore != nullptr)
			WaitForSingleObject(m_pSystemCore->GetMainThreadHandle(), INFINITE);

#ifdef _DEBUG
		if (UnloadDLL_in)
			CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)::FreeLibrary, g_hDllInstace, 0, nullptr);
#endif // _DEBUG
	}

	/*! \brief The engine main thread
		\return the exit code of the thread
	*/
	DWORD WindowerEngine::MainThread()
	{
		InitializeEngine();

		while (m_bShutdown == false)
		{
			// >>> Critical section
			LockEngineThread();

			UpdateEngine();
			Sleep(0);

			UnlockEngineThread();
			// Critical section <<<
		}

		// remove hooks and unload plugins
		Detach();

		return 0UL;
	}

	//! \brief Updates the engine
	void WindowerEngine::UpdateEngine()
	{
		if (m_bShutdown == false)
		{
			if (m_pUpdateTimer != nullptr)
			{
				m_pUpdateTimer->Update();

				if (m_pUpdateTimer->HasTicked())
				{
					WindowerPlugins::const_iterator PluginIt, EndIt = m_Plugins.cend();
					WindowerPlugin *pPlugin = nullptr;

					for (PluginIt = m_Plugins.cbegin(); PluginIt != EndIt; ++PluginIt)
					{
						pPlugin = static_cast<WindowerPlugin*>(PluginIt->second);

						if (pPlugin != nullptr)
							pPlugin->Update();
					}
					// configure any plugin queued
					PopPluginConfigure();
				}
			}
		}
	}

	void WindowerEngine::UpdateMacroProgress(unsigned long step, unsigned long total, bool stop)
	{
		if (m_pGraphicsCore != nullptr)
			m_pGraphicsCore->ShowMacroProgress(step, total, !stop && IsMacroThreadActive());

		if (m_pCmdLineCore != nullptr)
			m_pCmdLineCore->AbortMacro();
	}

	bool WindowerEngine::PressKey(long key, long delay, long repeat)
	{
		if (m_pSystemCore != nullptr)
		{
			if (delay < 50L)
				delay = 50L;

			for (long i = 0; i < repeat && IsMacroThreadActive(); ++i)
			{
				::PostMessage(m_pSystemCore->GameHWND(), WM_KEYDOWN, key, 0);
				::Sleep(50);
				::PostMessage(m_pSystemCore->GameHWND(), WM_KEYUP, key, 0);
				::Sleep(delay);
			}

			return true;
		}

		return false;
	}

	bool WindowerEngine::AbortMacro()
	{
		if (m_MacroThreadID != 0UL && m_MacroThreadState == ThreadState::SUSPENDED)
		{
			SetMacroThreadState(ThreadState::RUNNING);
			InterlockedExchange(&m_MacroThreadState, ThreadState::ABORTED);
		}
		else
			InterlockedExchange(&m_MacroThreadID, 0UL);

		UpdateMacroProgress(0UL, 0UL, true);
		
		return true;
	}

	void WindowerEngine::LockMacroThread()
	{
//		EnterCriticalSection(&m_MacroLock);
	}

	bool WindowerEngine::UnlockMacroThread()
	{
//		LeaveCriticalSection(&m_MacroLock);

		return true;
	}

	bool WindowerEngine::IsMacroThreadActive() const
	{
		return (m_MacroThreadID != 0UL && (m_MacroThreadState == ThreadState::RUNNING || m_MacroThreadState == ThreadState::SUSPENDED));
	}
	
	bool WindowerEngine::IsMacroThreadSuspended() const
	{
		return (m_MacroThreadID != 0UL && m_MacroThreadState == ThreadState::SUSPENDED);
	}
	
	//! \brief Initializes the engine
	void WindowerEngine::InitializeEngine()
	{
		if (m_pSystemCore != nullptr)
		{
			m_hGameWnd = m_pSystemCore->GameHWND();
			m_dwPID = ::GetCurrentProcessId();
		}
		
		if (m_pUpdateTimer == nullptr)
		{
			m_pUpdateTimer = new Timer;
			
			m_pUpdateTimer->SetRawTickInterval(100000);
			m_pUpdateTimer->Start();
		}

		InitializePlugins();
	}

	void WindowerEngine::LockEngineThread()
	{
//		EnterCriticalSection(&m_PluginLock);
	}

	bool WindowerEngine::UnlockEngineThread()
	{
//		LeaveCriticalSection(&m_PluginLock);

		return true;
	}

	bool WindowerEngine::CreateMacroThread(const string_t &file, unsigned long repeat)
	{
		MacroParam *pParam = new MacroParam(file, repeat);
		HANDLE hThread;

		if (IsMacroThreadActive() == false)
		{
			DWORD threadID = 0UL;

			UpdateMacroProgress(0UL, repeat, false);
			// create the macro thread
			hThread = ::CreateThread(nullptr, 0, WindowerEngine::MacroThread, pParam, 0, &threadID);

			if (hThread != nullptr)
			{
				InterlockedExchange(&m_MacroThreadState, ThreadState::RUNNING);
				InterlockedExchange(&m_MacroThreadID, threadID);
				::CloseHandle(hThread);
			}
			else
			{
				InterlockedExchange(&m_MacroThreadState, ThreadState::NONE);
				InterlockedExchange(&m_MacroThreadID, 0UL);
			}
		}

		return true;
	}
	
	DWORD WINAPI WindowerEngine::MacroThread(LPVOID pParam_in_out)
	{
		MacroParam* pParam = static_cast<MacroParam*>(pParam_in_out);
		DWORD Result = (DWORD)-1L;

		if (pParam != nullptr)
		{
			Result = m_Context->m_pCmdLineCore->ExecuteMacroFile(pParam->first, pParam->second) ? 1UL : 0UL;
			InterlockedExchange(&m_Context->m_MacroThreadState, ThreadState::NONE);
			InterlockedExchange(&m_Context->m_MacroThreadID, 0UL);
			delete pParam;
		}

		return Result;
	}

	bool WindowerEngine::SetMacroThreadState(ThreadState state)
	{
		bool result = false;

		switch (state)
		{
			case ThreadState::SUSPENDED:
				if (m_MacroThreadState == ThreadState::RUNNING)
				{
					InterlockedExchange(&m_MacroThreadState, state);
					result = true;
				}
			break;
			case ThreadState::RUNNING:
				if (m_MacroThreadState == ThreadState::SUSPENDED)
				{
					InterlockedExchange(&m_MacroThreadState, state);
					result = true;
				}
			break;
		}

		return result;
	}

	bool WindowerEngine::SuspendMacroThread(const std::string &condition)
	{
		m_ExpectCondition = condition;

		if (IsMacroThreadActive())
			return SetMacroThreadState(ThreadState::SUSPENDED);

		return false;
	}
	
	/*! \brief Callback invoked when the game chat receives a new line
		\param[in] MessageType_in : the type of the message
		\param[in] pSender_in : the sender of the message
		\param[in] MsgSize_in : the size of the unmodified message
		\param[in] pOriginalMsg_in : a pointer to the unmodified message
		\param[in] pModifiedMsg_in_out : the resulting text modified by the plugin
		\param[in] DWORD ModifiedSize_in : the modified message size
		\return the new size of the message
	*/
	DWORD_PTR WindowerEngine::OnChatMessage(CHAT_MESSAGE_TYPE MessageType_in, const char* pSender_in, DWORD_PTR MsgSize_in, const char *pOriginalMsg_in,
											char **pModifiedMsg_in_out, DWORD_PTR ModifiedSize_in, DWORD &MessageFlags_out)
	{
		if (m_ExpectCondition.empty() || m_ExpectCondition[0] == '*' || strstr(pOriginalMsg_in, m_ExpectCondition.c_str()) != nullptr)
			SetMacroThreadState(ThreadState::RUNNING);

		return MsgSize_in;
	}

	bool WindowerEngine::Exit(std::string& Feedback_out)
	{
		Feedback_out = "Exiting the game...";

		// stop the engine thread
		ShutdownEngine();

		return (PostMessage(m_hGameWnd, WM_QUIT, 0UL, 0UL) != FALSE);
	}

	DWORD_PTR WindowerEngine::MemoryScan(const std::string &Pattern_in,
										 MemoryScanResult &Results_in_out)
	{
		SigScan::SigScan &MemScan = m_HookManager.GetSigScan();
		DWORD_PTR Result = 0UL;

		if (MemScan.Initialize(m_dwPID, _T(SIGSCAN_GAME_PROCESSA)))
		{
			Result = MemScan.ScanMemory(Pattern_in, 0L, Results_in_out);
			MemScan.Clear();
		}

		return Result;
	}

	void WindowerEngine::Inject()
	{
		HWND hWnd = ::FindWindow(_T(FFXIV_WINDOW_CLASSA), nullptr);

		if (hWnd != nullptr && m_pSystemCore != nullptr)
		{
			// subclass the game window and start the engine thread
			m_pSystemCore->SubclassWindow(hWnd);
			// @TODO : find the Direct3D device
		}
	}

	void WindowerEngine::SerializeLabel(const string_t &Name_in, long X_in, long Y_in,
										unsigned long TextColor_in, const string_t &FontName_in,
										unsigned short FontSize_in, bool Bold_in,
										bool Italic_in, bool Collapsed_in)
	{
		m_Settings.SerializeLabel(Name_in, X_in, Y_in, TextColor_in,
								  FontName_in, FontSize_in,
								  Bold_in, Italic_in, Collapsed_in);

		if (m_pSettingsManager != nullptr)
			m_pSettingsManager->CopySettings(&m_Settings);
	}

	bool WindowerEngine::DeserializeLabel(const string_t &Name_in, long &X_out, long &Y_out,
										  unsigned long &TextColor_out, string_t &FontName_out,
										  unsigned short &FontSize_out, bool &Bold_out,
										  bool &Italic_out, bool &Collapsed_out)
	{
		return m_Settings.DeserializeLabel(Name_in, X_out, Y_out, TextColor_out, FontName_out,
										   FontSize_out, Bold_out, Italic_out, Collapsed_out);
	}

	void WindowerEngine::SetWnd(HWND hWnd_in)
	{
		if (m_pGraphicsCore != nullptr)
			m_pGraphicsCore->SetWnd(hWnd_in);
	}

	long WindowerEngine::GetCraftingCondition() const
	{
		if (m_pPlayerCore != nullptr)
			return m_pPlayerCore->GetCraftingCondition();

		return Crafting::Invalid;
	}
}