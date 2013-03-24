/**************************************************************************
	created		:	2011-06-03
	filename	: 	WindowerEngine.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <SettingsManager.h>
#include <PluginFramework.h>
#include <PluginManager.h>
#include <HookEngine.h>

#include "WindowerSettings.h"
#include "WindowerSettingsManager.h"

#include "WindowerEngine.h"

#include "RegisterClassExHook.h"
#include "CreateWindowExHook.h"
#include "Direct3D9Hook.h"
#ifdef _DEBUG
	#include "TestHook.h"
#endif // _DEBUG

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "CmdLineCore.h"
#include "GameChatCore.h"
#include "GraphicsCore.h"
#include "SystemCore.h"
#ifdef _DEBUG
	#include "TestCore.h"
#endif // _DEBUG

#define _TESTING

namespace Windower
{
	PluginManager* ICoreModule::m_pPluginManager = NULL;

	/*! \brief WindowerEngine constructor
		\param[in] pConfigFile_in : path to the configuration file
	*/
	WindowerEngine::WindowerEngine(HMODULE hModule_in, const TCHAR *pConfigFile_in)
		: PluginEngine(hModule_in, pConfigFile_in), m_bThreadInit(false), m_pGameChatCore(NULL),
		  m_bShutdown(false), m_hGameWnd(NULL), m_pSystemCore(NULL)
	{
		// create the settings manager
		m_pSettingsManager = new SettingsManager(m_WorkingDir.c_str(), pConfigFile_in);
		// testing
#if defined _DEBUG && defined _TESTING
		// create the testing module
		m_pTestCore = new TestCore(*this, m_HookManager);
#endif // _DEBUG

		// check the settings and load the default profile
		if (m_pSettingsManager->IsGamePathValid() && m_pSettingsManager->LoadDefaultProfile(m_Settings))
		{
			// create the game chat module
			m_pGameChatCore = new GameChatCore(*this, m_HookManager);
			// create the command line module
			m_pCmdLineCore = new CmdLineCore(*this, m_HookManager);
			// create the graphics module
			m_pGraphicsCore = NULL; //new GraphicsCore(*this, m_HookManager, m_Settings.GetVSync());
		}
	}

	/*! \brief WindowerEngine destructor */
	WindowerEngine::~WindowerEngine()
	{
		Detach();

		delete m_pSettingsManager;
		m_pSettingsManager = NULL;

		delete m_pGameChatCore;
		m_pGameChatCore = NULL;

		delete m_pGraphicsCore;
		m_pGraphicsCore = NULL;

		delete m_pCmdLineCore;
		m_pCmdLineCore = NULL;

#if defined _DEBUG && defined _TESTING
		delete m_pTestCore;
		m_pTestCore = NULL;
#endif // _DEBUG
	}

	//! \brief Initializes the plugins
	bool WindowerEngine::InitializePlugins()
	{
		if (m_pPluginManager != NULL)
		{
			// list the available plugins compatible with windower
			m_pPluginManager->ListPlugins(m_WorkingDir + _T("plugins"),
										  PLUGIN_COMPATIBILITY_WINDOWER);
			// set the plugin manager of the modules
			ICoreModule::SetPluginManager(*m_pPluginManager);
			// load active plugins
			LoadPlugins(m_Settings.GetActivePlugins());

			return true;
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

		for (Iter = m_Modules.begin(); Iter != m_Modules.end(); ++Iter)
		{
			pModule = Iter->second;

			if (pModule != NULL)
				pModule->RegisterHooks(m_HookManager);
		}

		if (m_HookManager.InstallRegisteredHooks())
		{
			for (Iter = m_Modules.begin(); Iter != m_Modules.end(); ++Iter)
			{
				pModule = Iter->second;

				if (pModule != NULL)
				{
					pModule->OnHookInstall(m_HookManager);
					pModule->RegisterServices();
				}
			}

			return InitializePlugins();
		}

		return false;
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

	}

	//! \brief Initializes the engine
	void WindowerEngine::InitializeEngine()
	{
		if (m_pSystemCore != NULL)
		{
			m_hGameWnd = m_pSystemCore->GameHWND();
			m_dwPID = m_pSystemCore->GamePID();
		}
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

	/*! \brief Optional callback to inform the engine that a successful call to the hook was made
		\param[in] pHookName_in : the name of the hook
	*/
	void WindowerEngine::OnHookCall(const char *pHookName_in)
	{
		if (strcmp(pHookName_in, "RegisterClassExA") == 0
		 || strcmp(pHookName_in, "CreateWindowExA") == 0)
		{
			// uninstall and unregister one-time-use hooks
			if (m_HookManager.UninstallHook(pHookName_in))
				m_HookManager.UnregisterHook(pHookName_in);
		}
	}
}