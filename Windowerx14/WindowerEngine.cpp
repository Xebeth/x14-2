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
#include <HookEngine.h>
#include <d3d9.h>
#include <queue>

#include "WindowerSettings.h"
#include "WindowerSettingsManager.h"

#include "version.h"
#include "BaseEngine.h"
#include "PluginEngine.h"
#include "WindowerEngine.h"
#include "PluginsServices.h"

#include "FormatChatMessageHook.h"
#include "RegisterClassExHook.h"
#include "CreateWindowExHook.h"
#include "Direct3D9Hook.h"
#ifdef _DEBUG
	#include "TestHook.h"
#endif // _DEBUG

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "GameChatCore.h"
#include "GraphicsCore.h"
#include "SystemCore.h"
#ifdef _DEBUG
	#include "TestCore.h"
#endif // _DEBUG

#include "WindowerCommand.h"
#include "CommandParser.h"
#include "CommandDispatcher.h"

#include "ICreateTextNodePlugin.h"
#include "InjectVersion.h"

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
		// create the hook manager
		m_pSettingsManager->LoadDefaultProfile(m_Settings);

		// create the services

		// testing
#ifdef _DEBUG
		m_pTestCore = new TestCore(*this);
		RegisterModule(_T("Testing"), m_pTestCore);
#endif // _DEBUG
		// Win32 related hooks
 		m_pSystemCore = NULL; // new SystemCore(*this);
// 		RegisterModule(_T("System"), m_pSystemCore);
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
		m_pGraphicsCore = new GraphicsCore(*this, m_Settings.GetResX(), m_Settings.GetResY(), m_Settings.GetVSync());
		RegisterModule(_T("Graphics"), m_pGraphicsCore);

		m_pPluginManager->ListPlugins(m_pSettingsManager->GetPluginsAbsoluteDir());
		ICoreModule::SetPluginManager(*m_pPluginManager);

		// load plugins
		PluginEngine::LoadPlugin(_T("TellDetect"));
		PluginEngine::LoadPlugin(_T("Timestamp"));
		PluginEngine::LoadPlugin(_T("ExpWatch"));

		// register commands
		CallerParam Caller("WindowerEngine", this);
	
		// register the "load" command
		const char *pParamName = "plugin_name";
		CommandParameters PluginParams;

		PluginParams[pParamName].Name = pParamName;
		PluginParams[pParamName].Type = COMMAND_PARAM_TYPE_STRING;
		PluginParams[pParamName].Value = "";
		PluginParams[pParamName].Description = "the name of the plugin to load";
		m_pCommandDispatcher->RegisterCommand(PLUGIN_REGKEY, "load", "Loads a plugin given its name.", Caller, LoadPlugin, 1, 1, PluginParams);
		// register the "unload" command
		PluginParams[pParamName].Description = "the name of the plugin to unload";
		m_pCommandDispatcher->RegisterCommand(PLUGIN_REGKEY, "unload", "Unloads a plugin given its name.", Caller, UnloadPlugin, 1, 1, PluginParams);

		// injects the windower version on the main menu
		m_pInjectVersion = new InjectVersion(m_pPluginServices);
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

		delete m_pSettingsManager;
		m_pSettingsManager = NULL;

		delete m_pInjectVersion;
		m_pInjectVersion = NULL;

#ifdef _DEBUG
		delete m_pTestCore;
		m_pTestCore = NULL;
#endif // _DEBUG
	}

	/*! \brief Installs the internal hooks used by the windower
		\return true if the hooks were installed successfully; false otherwise
	*/
	bool WindowerEngine::Attach()
	{
		CoreModules::const_iterator Iter;

		for (Iter = m_Modules.begin(); Iter != m_Modules.end(); ++Iter)
			if (Iter->second != NULL)
				Iter->second->RegisterHooks(m_HookManager);

		if (m_HookManager.InstallRegisteredHooks())
		{
			for (Iter = m_Modules.begin(); Iter != m_Modules.end(); ++Iter)
				if (Iter->second != NULL)
					Iter->second->OnHookInstall(m_HookManager);

			return true;
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
			Sleep(100);
		}

		return 0L;
	}

	//! \brief Updates the engine
	void WindowerEngine::UpdateEngine()
	{
		if (m_FeedbackMessages.empty() == false)
		{
			// @TODO inject text in the chat log
			m_FeedbackMessages.pop_front();
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

	/*! \brief Unload plugin command callback ("unload")
		\param[in] pCommand_in : the command passed by the command dispatcher
		\return DISPATCHER_RESULT_SUCCESS if the command was successful; DISPATCHER_RESULT_INVALID_CALL otherwise
	*/
	int WindowerEngine::UnloadPlugin(const WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL && pCommand_in->Caller.DataType.compare("WindowerEngine") == 0)
		{
			WindowerEngine *pEngine = reinterpret_cast<WindowerEngine*>(pCommand_in->Caller.pData);
			const WindowerCommandParam *pParam = pCommand_in->GetParameter("plugin_name");

			if (pEngine != NULL && pParam != NULL)
			{
				string_t PluginName;

				if (static_cast<PluginEngine*>(pEngine)->UnloadPlugin(pParam->GetWideStringValue(PluginName)))
					format(pCommand_in->ResultMsg, "The plugin '%s' was unloaded successfully.", pParam->Value.c_str());
				else
					format(pCommand_in->ResultMsg, "The plugin '%s' couldn't be unloaded.", pParam->Value.c_str());

				return DISPATCHER_RESULT_SUCCESS;
			}

			return DISPATCHER_RESULT_INVALID_PARAMETERS;
		}

		return DISPATCHER_RESULT_INVALID_CALL;
	}

	/*! \brief Load plugin command callback ("load")
		\param[in] pCommand_in : the command passed by the command dispatcher
		\return DISPATCHER_RESULT_SUCCESS if the command was successful; DISPATCHER_RESULT_INVALID_CALL otherwise
	*/
	int WindowerEngine::LoadPlugin(const WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL && pCommand_in->Caller.DataType.compare("WindowerEngine") == 0)
		{
			WindowerEngine *pEngine = reinterpret_cast<WindowerEngine*>(pCommand_in->Caller.pData);
			const WindowerCommandParam *pParam = pCommand_in->GetParameter("plugin_name");

			if (pEngine != NULL && pParam != NULL)
			{
				std::string Feedback;
				string_t PluginName;

				if (static_cast<PluginEngine*>(pEngine)->LoadPlugin(pParam->GetWideStringValue(PluginName)))
					format(pCommand_in->ResultMsg, "The plugin '%s' was loaded successfully.", pParam->Value.c_str());
				else
					format(pCommand_in->ResultMsg, "The plugin '%s' couldn't be loaded.", pParam->Value.c_str());

				return DISPATCHER_RESULT_SUCCESS;
			}

			return DISPATCHER_RESULT_INVALID_PARAMETERS;
		}

		return DISPATCHER_RESULT_INVALID_CALL;
	}
}