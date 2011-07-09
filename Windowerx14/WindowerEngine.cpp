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
#include <NonCopyable.h>
#include <HookEngine.h>
#include <d3dx9.h>
#include <d3d9.h>
#include <queue>

#include "BaseEngine.h"
#include "PluginEngine.h"
#include "WindowerEngine.h"
#include "PluginsServices.h"

#include "FormatChatMessageHook.h"
#include "RegisterClassExHook.h"
#include "CreateWindowExHook.h"
#include "Direct3D9Hook.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "GameChatCore.h"
#include "GraphicsCore.h"
#include "SystemCore.h"

#include "WindowerSettings.h"
#include "WindowerSettingsManager.h"

#include "WindowerCommand.h"
#include "CommandParser.h"
#include "CommandDispatcher.h"

namespace Windower
{
	PluginManager* ICoreModule::m_pPluginManager = NULL;

	/*! \brief WindowerEngine constructor
		\param[in] pConfigFile_in : path to the configuration file
	*/
	WindowerEngine::WindowerEngine(const TCHAR *pConfigFile_in)
		: PluginEngine(pConfigFile_in)
	{
		m_bThreadInit = false;
		m_bShutdown = false;
		m_hGameWnd = NULL;

		// create the settings
		m_pSettingsManager = new SettingsManager(pConfigFile_in);
		m_pSettings = new WindowerProfile;
		// create the hook manager
		m_pHookManager = new HookEngine;

		m_pSettingsManager->LoadDefaultProfile(&m_pSettings);

		// create the services

		// Win32 related hooks
		m_pSystemCore = new SystemCore(*this);
		RegisterModule(_T("System"), m_pSystemCore);
		// Commander dispatcher
		m_pCommandDispatcher = new CommandDispatcher(*this);
		RegisterModule(_T("CommandDispatcher"), m_pCommandDispatcher);
		// Commander parser
		m_pCommandParser = new CommandParser(*this, *m_pCommandDispatcher);
		RegisterModule(_T("CommandParser"), m_pCommandParser);
		// Game chat related hooks
		m_pGameChatCore = new GameChatCore(*this, *m_pCommandParser, *m_pCommandDispatcher);
		RegisterModule(_T("GameChat"), m_pGameChatCore);
		// Direct3D related hooks
		m_pGraphicsCore = new GraphicsCore(*this, m_pSettings->GetResX(), m_pSettings->GetResY(), m_pSettings->GetVSync());
		RegisterModule(_T("Graphics"), m_pGraphicsCore);

		m_pPluginManager->ListPlugins(m_pSettings->GetPluginsAbsoluteDir());
		ICoreModule::SetPluginManager(m_pPluginManager);
		// load plugins
		LoadPlugin(_T("Tell detect"));
		LoadPlugin(_T("Timestamp"));
		// LoadPlugin(_T("Chat log")); not working

		// register commands
		// CallerParam Caller("WindowerEngine", this);
		// m_pCommandDispatcher->RegisterCommand(PLUGIN_REGKEY, "list", "lists the loaded plugins", Caller, ListPlugins);
	}

	/*! \brief WindowerEngine destructor */
	WindowerEngine::~WindowerEngine()
	{
		Detach();

		delete m_pGraphicsCore;
		m_pGraphicsCore = NULL;

		delete m_pGameChatCore;
		m_pGameChatCore = NULL;

		delete m_pCommandParser;
		m_pCommandParser = NULL;

		delete m_pCommandDispatcher;
		m_pCommandDispatcher = NULL;

		delete m_pSystemCore;
		m_pSystemCore = NULL;

		delete m_pHookManager;
		m_pHookManager = NULL;

		delete m_pSettings;
		m_pSettings = NULL;

		delete m_pSettingsManager;
		m_pSettingsManager = NULL;
	}

	/*! \brief Installs the internal hooks used by the windower
		\return true if the hooks were installed successfully; false otherwise
	*/
	bool WindowerEngine::Attach()
	{
		if (m_pHookManager != NULL)
		{
			CoreModules::const_iterator Iter;

			for (Iter = m_Modules.begin(); Iter != m_Modules.end(); ++Iter)
				if (Iter->second != NULL)
					Iter->second->RegisterHooks(m_pHookManager);

			if (m_pHookManager->InstallRegisteredHooks())
			{
				for (Iter = m_Modules.begin(); Iter != m_Modules.end(); ++Iter)
					if (Iter->second != NULL)
						Iter->second->OnHookInstall(m_pHookManager);

				return true;
			}
		}

		return false;
	}

	/*! \brief Uninstalls the internal hooks used by the windower
		\return true if the hooks were uninstalled successfully; false otherwise
	*/
	bool WindowerEngine::Detach()
	{
		PluginEngine::Detach();

		if (m_pHookManager != NULL)
			return m_pHookManager->UninstallRegisteredHooks();

		return false;
	}

	DWORD WindowerEngine::MainThread()
	{
		if (InitializeEngine())
		{
			m_bThreadInit = true;

			while (!m_bShutdown)
			{
				UpdateEngine();
				Sleep(0);
			}
		}

		return 0L;
	}

	void WindowerEngine::UpdateEngine()
	{

	}

	bool WindowerEngine::InitializeEngine()
	{
		if (m_pSystemCore != NULL)
		{
			m_hGameWnd = m_pSystemCore->GameHWND();
			m_dwPID = m_pSystemCore->GamePID();
		}

		return true;
	}

	void WindowerEngine::ShutdownEngine()
	{

	}

	/*! \brief Callback invoked when the game is shutting down */
	void WindowerEngine::OnShutdown()
	{
		m_bShutdown = true;
	}
}