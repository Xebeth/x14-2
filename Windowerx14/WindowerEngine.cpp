/**************************************************************************
	created		:	2011-06-03
	filename	: 	WindowerEngine.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "WindowerEngine.h"

#include "RegisterClassExHook.h"
#include "CreateWindowExHook.h"
#include "SetCursorPosHook.h"
#include "PlayerDataHook.h"
#include "Direct3D9Hook.h"

#ifdef _DEBUG
	#include "TestHook.h"
#endif // _DEBUG

#include "IRenderable.h"

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
	PluginFramework::PluginManager* ICoreModule::m_pPluginManager = NULL;

	/*! \brief WindowerEngine constructor
		\param[in] pConfigFile_in : path to the configuration file
	*/
	WindowerEngine::WindowerEngine(HMODULE hModule_in, const TCHAR *pConfigFile_in)
		: PluginEngine(hModule_in, pConfigFile_in), m_bShutdown(false), m_bThreadInit(false), 
		  m_pGameChatCore(NULL), m_hGameWnd(NULL), m_pSystemCore(NULL), m_pPlayerCore(NULL),
		  m_pGraphicsCore(NULL), m_pCmdLineCore(NULL), m_pUpdateTimer(NULL), m_dwPID(0UL)
	{
		InitializeCriticalSection(&m_PluginLock);

		WindowerCore::Initialize(this, &m_HookManager);

		// create the settings manager
		m_pSettingsManager = new SettingsManager(m_WorkingDir.c_str(), pConfigFile_in);
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
			// create the player data module
			m_pPlayerCore = new PlayerCore;
			// create the graphics module
			m_pGraphicsCore = new GraphicsCore(m_Settings.GetVSync());
		}
	}

	/*! \brief WindowerEngine destructor */
	WindowerEngine::~WindowerEngine()
	{
		Detach();

		if (m_pSettingsManager != NULL)
		{
			delete m_pSettingsManager;
			m_pSettingsManager = NULL;
		}

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

#if defined _DEBUG && defined _TESTING
		if (m_pTestCore != NULL)
		{
			delete m_pTestCore;
			m_pTestCore = NULL;
		}
#endif // _DEBUG

		DeleteCriticalSection(&m_PluginLock);
	}

	//! \brief Initializes the plugins
	bool WindowerEngine::InitializePlugins()
	{
		if (m_pPluginManager != NULL)
		{
			// >>> Critical section
			LockPlugins();

			// list the available plugins compatible with windower
			m_pPluginManager->ListPlugins(m_WorkingDir + _T("plugins"),
										  PLUGIN_COMPATIBILITY_WINDOWER);
			// set the plugin manager of the modules
			ICoreModule::SetPluginManager(*m_pPluginManager);

			// load active plugins
			LoadPlugins(m_Settings.GetActivePlugins());

			return UnlockPlugins();
			// Critical section <<<
		}

		return false;
	}

	/*! \brief Installs the internal hooks used by the windower
		\return true if the hooks were installed successfully; false otherwise
	*/
	bool WindowerEngine::Attach()
	{
		CoreModules::const_iterator Iter;
		ICoreModule *pModule = NULL;
		
		// register the hooks
		for (Iter = m_Modules.begin(); Iter != m_Modules.end(); ++Iter)
		{
			pModule = Iter->second;

			if (pModule != NULL)
				pModule->RegisterHooks(m_HookManager);
		}
		// install the hooks
		if (m_HookManager.InstallRegisteredHooks())
		{
			for (Iter = m_Modules.begin(); Iter != m_Modules.end(); ++Iter)
			{
				pModule = Iter->second;

				if (pModule != NULL)
					pModule->OnHookInstall(m_HookManager);
			}
		}
		// register services in a separate loop since OnHookInstall could have added more modules
		for (Iter = m_Modules.begin(); Iter != m_Modules.end(); ++Iter)
		{
			pModule = Iter->second;

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
		PluginEngine::Detach();

		return m_HookManager.UninstallRegisteredHooks();
	}

	/*! \brief The engine main thread
		\return the exit code of the thread
	*/
	DWORD WindowerEngine::MainThread()
	{
		m_bThreadInit = true;
		InitializeEngine();
		
		while (m_bShutdown == false)
		{
			UpdateEngine();
			Sleep(0);
		}

		return 0L;
	}

	//! \brief Updates the engine
	void WindowerEngine::UpdateEngine()
	{
		if (m_bShutdown == false && m_pUpdateTimer != NULL)
		{
			m_pUpdateTimer->Update();

			if (m_pUpdateTimer->HasTicked() && m_pPlayerCore->IsLoggedIn())
			{
				WindowerPlugins::const_iterator PluginIt = m_Plugins.cbegin();
				WindowerPlugins::const_iterator EndIt = m_Plugins.cend();
				WindowerPlugin *pPlugin = NULL;

				// >>> Critical section
				LockPlugins();

				for (; PluginIt != EndIt; ++PluginIt)
				{
					pPlugin = static_cast<WindowerPlugin*>(PluginIt->second);

					if (pPlugin != NULL)
						pPlugin->Update();
				}

				UnlockPlugins();
				// Critical section <<<
			}
		}
	}

	//! \brief Initializes the engine
	void WindowerEngine::InitializeEngine()
	{
		if (m_pSystemCore != NULL)
		{
			m_hGameWnd = m_pSystemCore->GameHWND();
			m_dwPID = m_pSystemCore->GamePID();
		}

		if (m_pUpdateTimer == NULL)
		{
			m_pUpdateTimer = new Timer;

			m_pUpdateTimer->SetRawTickInterval(100000);
			m_pUpdateTimer->Start();			
		}

		InitializePlugins();
	}

	//! \brief Shuts the engine down
	void WindowerEngine::ShutdownEngine()
	{

	}

	/*! \brief Callback invoked when the game is shutting down */
	void WindowerEngine::OnShutdown()
	{
		m_bShutdown = true;
	}

	void WindowerEngine::LockPlugins()
	{
		EnterCriticalSection(&m_PluginLock);
	}

	bool WindowerEngine::UnlockPlugins()
	{
		LeaveCriticalSection(&m_PluginLock);

		return true;
	}

	bool WindowerEngine::Exit(std::string& Feedback_out) const
	{
		Feedback_out = "Exiting the game...";

		return (PostMessage(m_hGameWnd, WM_QUIT, 0UL, 0UL) != FALSE);
	}

	void WindowerEngine::DisplayHelp()
	{
		if (m_pPlayerCore->IsLoggedIn())
			CmdLineCore::InjectCommand("//help");
	}

	DWORD_PTR WindowerEngine::FindAddress(const std::string &Pattern_in, DWORD Offset_in)
	{
		SigScan::SigScan &MemScan = m_HookManager.GetSigScan();
		DWORD_PTR Result = NULL;

		if (MemScan.Initialize(m_dwPID, _T(SIGSCAN_GAME_PROCESSA)))
		{
			Result = MemScan.ScanMemory(Pattern_in, Offset_in);
			MemScan.Clear();
		}

		return Result;
	}
}