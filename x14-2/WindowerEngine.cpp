/**************************************************************************
	created		:	2011-06-03
	filename	: 	WindowerEngine.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "WindowerEngine.h"

#include "IRenderable.h"
#include "IDeviceCreateSubscriber.h"

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

#define _TESTING

#if defined _DEBUG && defined _TESTING
	#include "TestCore.h"
#endif // _DEBUG

namespace Windower
{
	/*! \brief WindowerEngine constructor
		\param[in] pConfigFile_in : path to the configuration file
	*/
	WindowerEngine::WindowerEngine(HMODULE hModule_in, const TCHAR *pConfigFile_in)
		: PluginEngine(hModule_in, pConfigFile_in), m_hGameWnd(NULL), m_dwPID(0UL),
		  m_bDetached(false), m_bShutdown(false), m_hWindowerDLL(hModule_in),
		  m_pGameChatCore(NULL), m_pSystemCore(NULL), m_pPlayerCore(NULL),
		  m_pUpdateTimer(NULL), m_pGraphicsCore(NULL), m_pCmdLineCore(NULL)
	{
		InitializeCriticalSection(&m_PluginLock);
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
		m_pTestCore = NULL;
	#endif
#endif // _DEBUG

		// check the settings and load the default profile
		if (m_pSettingsManager->IsGamePathValid() && m_pSettingsManager->LoadDefaultProfile(m_Settings))
		{
			// create the game chat module
			m_pGameChatCore = new GameChatCore;
			// create the command line module
			m_pCmdLineCore = new CmdLineCore;
/*
			// create the player data module
			m_pPlayerCore = new PlayerCore;
*/
			// create the graphics module
			m_pGraphicsCore = new GraphicsCore(m_Settings.GetVSync());
			// add the graphics core as an event handler
			m_pSystemCore->AddUIH(m_pGraphicsCore);
		}
	}

	/*! \brief WindowerEngine destructor */
	WindowerEngine::~WindowerEngine()
	{
		if (m_pGameChatCore != NULL)
		{
			delete m_pGameChatCore;
			m_pGameChatCore = NULL;
		}

		if (m_pGraphicsCore != NULL)
		{
			delete m_pGraphicsCore;
			m_pGraphicsCore = NULL;
		}

		if (m_pPlayerCore != NULL)
		{
			delete m_pPlayerCore;
			m_pPlayerCore = NULL;
		}

		if (m_pCmdLineCore != NULL)
		{
			delete m_pCmdLineCore;
			m_pCmdLineCore = NULL;
		}

		if (m_pSystemCore != NULL)
		{
			delete m_pSystemCore;
			m_pSystemCore = NULL;
		}

		if (m_pUpdateTimer != NULL)
		{
			delete m_pUpdateTimer;
			m_pUpdateTimer = NULL;
		}

#if defined _DEBUG && defined _TESTING
		if (m_pTestCore != NULL)
		{
			delete m_pTestCore;
			m_pTestCore = NULL;
		}
#endif // _DEBUG

		// delete the critical sections last
		DeleteCriticalSection(&m_PluginLock);
	}

	bool WindowerEngine::IsPlayerLoggedIn() const
	{
		return (m_pPlayerCore != NULL && m_pPlayerCore->IsLoggedIn());
	}

	//! \brief Initializes the plugins
	bool WindowerEngine::InitializePlugins()
	{
		if (m_pPluginManager != NULL)
		{
			// >>> Critical section
			LockEngineThread();

			// list the available plugins compatible with windower
			m_pPluginManager->ListPlugins(m_WorkingDir + _T("plugins"),
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
		ICoreModule *pModule = NULL;
		
		// register the hooks
		for (ModuleIt = m_Modules.cbegin(); ModuleIt != EndIt; ++ModuleIt)
		{
			pModule = ModuleIt->second;

			if (pModule != NULL)
				pModule->RegisterHooks(m_HookManager);
		}
		// install the hooks
		if (m_HookManager.InstallRegisteredHooks())
		{
			for (ModuleIt = m_Modules.cbegin(); ModuleIt != EndIt; ++ModuleIt)
			{
				pModule = ModuleIt->second;

				if (pModule != NULL)
					pModule->OnHookInstall(m_HookManager);
			}
		}
		// register services in a separate loop since OnHookInstall could have added more modules
		for (ModuleIt = m_Modules.cbegin(); ModuleIt != EndIt; ++ModuleIt)
		{
			pModule = ModuleIt->second;

			if (pModule != NULL)
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
			if (m_pPlayerCore != NULL)
				m_pPlayerCore->Detach();
			// stop the update timer
			if (m_pUpdateTimer != NULL)
				m_pUpdateTimer->Stop();
			// restore the vtables of the Direct3D objects
			if (m_pGraphicsCore != NULL)
				m_pGraphicsCore->Detach();
			// restore the window procedures
			if (m_pSystemCore != NULL)
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

		if (m_pSystemCore != NULL)
			WaitForSingleObject(m_pSystemCore->GetMainThreadHandle(), INFINITE);

		if (UnloadDLL_in)
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)::FreeLibrary, m_hWindowerDLL, 0, NULL);
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
		if (m_bShutdown == false && m_pUpdateTimer != NULL)
		{
			m_pUpdateTimer->Update();

			if (m_pUpdateTimer->HasTicked())
			{
				WindowerPlugins::const_iterator PluginIt, EndIt = m_Plugins.cend();
				WindowerPlugin *pPlugin = NULL;

				for (PluginIt = m_Plugins.cbegin(); PluginIt != EndIt; ++PluginIt)
				{
					pPlugin = static_cast<WindowerPlugin*>(PluginIt->second);

					if (pPlugin != NULL)
						pPlugin->Update();
				}
				// configure any plugin queued
				PopPluginConfigure();
			}
		}
	}

	//! \brief Initializes the engine
	void WindowerEngine::InitializeEngine()
	{
		if (m_pSystemCore != NULL)
		{
			m_hGameWnd = m_pSystemCore->GameHWND();
			m_dwPID = ::GetCurrentProcessId();
		}

		if (m_pUpdateTimer == NULL)
		{
			m_pUpdateTimer = new Timer;

			m_pUpdateTimer->SetRawTickInterval(100000);
			m_pUpdateTimer->Start();			
		}

		InitializePlugins();
	}

	void WindowerEngine::LockEngineThread()
	{
		EnterCriticalSection(&m_PluginLock);
	}

	bool WindowerEngine::UnlockEngineThread()
	{
		LeaveCriticalSection(&m_PluginLock);

		return true;
	}

	bool WindowerEngine::Exit(std::string& Feedback_out)
	{
		Feedback_out = "Exiting the game...";

		// stop the engine thread
		ShutdownEngine();

		return (PostMessage(m_hGameWnd, WM_QUIT, 0UL, 0UL) != FALSE);
	}

	DWORD WindowerEngine::MemoryScan(const std::string &Pattern_in,
									 MemoryScanResult &Results_in_out)
	{
		SigScan::SigScan &MemScan = m_HookManager.GetSigScan();
		DWORD Result = 0UL;

		if (MemScan.Initialize(m_dwPID, _T(SIGSCAN_GAME_PROCESSA)))
		{
			Result = MemScan.ScanMemory(Pattern_in, 0L, Results_in_out);
			MemScan.Clear();
		}

		return Result;
	}

	void WindowerEngine::Inject()
	{
		HWND hWnd = ::FindWindow(_T(FFXIV_WINDOW_CLASSA), NULL);

		if (hWnd != NULL && m_pSystemCore != NULL)
		{
			// subclass the game window and start the engine thread
			m_pSystemCore->SubclassWindow(hWnd);
			// @TODO : find the Direct3D device
		}
	}
}