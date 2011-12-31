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
#include <d3d9.h>
#include "version.h"

#include "WindowerSettings.h"
#include "WindowerSettingsManager.h"

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
#include "CommandHandler.h"
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
		// Commander dispatcher
		m_pCommandDispatcher = new CommandDispatcher(*this);
		// Commander parser
		m_pCommandParser = new CommandParser(*this, *m_pCommandDispatcher);
		// Game chat related hooks
		m_pGameChatCore = new GameChatCore(*this, *m_pCommandParser, *m_pCommandDispatcher);
		// Direct3D related hooks
		m_pGraphicsCore = new GraphicsCore(*this, m_Settings.GetResX(), m_Settings.GetResY(), m_Settings.GetVSync());

		m_pPluginManager->ListPlugins(m_pSettingsManager->GetPluginsAbsoluteDir());
		ICoreModule::SetPluginManager(*m_pPluginManager);

		// load plugins
		PluginEngine::LoadPlugin(_T("TellDetect"));
		PluginEngine::LoadPlugin(_T("Timestamp"));
		PluginEngine::LoadPlugin(_T("ExpWatch"));

		WindowerCommand *pCommand;

		// register the "load" command
		pCommand = new WindowerCommand(ENGINE_KEY, CMD_LOAD_PLUGIN, "load", "Loads a plugin given its name.", this);

		if (pCommand != NULL)
		{
			pCommand->AddStringParam("plugin", false, "", "the name of the plugin to load");

			if (m_pCommandDispatcher->RegisterCommand(pCommand) == false)
				delete pCommand;
		}
		// register the "unload" command
		pCommand = new WindowerCommand(ENGINE_KEY, CMD_UNLOAD_PLUGIN, "unload", "Unloads a plugin given its name.", this);

		if (pCommand != NULL)
		{
			pCommand->AddStringParam("plugin", false, "", "the name of the plugin to unload");

			if (m_pCommandDispatcher->RegisterCommand(pCommand) == false)
				delete pCommand;
		}
		// injects the windower version on the main menu
		m_pInjectVersion = new InjectVersion();
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

	
	/*! \brief Executes the command specified by its ID
		\param[in] CmdID_in : the ID of the command to execute
		\param[in] Command_in : the command to execute
		\param[out] Feedback_out : the result of the execution
		\return true if the command was executed successfully; false otherwise
	*/
	bool WindowerEngine::ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string &Feedback_out)
	{
		switch(CmdID_in)
		{
			case CMD_LOAD_PLUGIN:
			{
				std::string PluginName = Command_in.GetStringValue("plugin");
				string_t PluginNameW;

				if (LoadPlugin(convert_utf8(PluginName, PluginNameW)))
				{
					format(Feedback_out, "The plugin '%s' was loaded successfully.", PluginName.c_str());

					return true;
				}
				else
				{
					format(Feedback_out, "The plugin '%s' couldn't be unloaded.", PluginName.c_str());

					return false;
				}
			}
			break;
			case CMD_UNLOAD_PLUGIN:
			{
				std::string PluginName = Command_in.GetStringValue("plugin");
				string_t PluginNameW;

				if (UnloadPlugin(convert_utf8(PluginName, PluginNameW)))
				{
					format(Feedback_out, "The plugin '%s' was unloaded successfully.", PluginName.c_str());

					return true;
				}
				else
				{
					format(Feedback_out, "The plugin '%s' couldn't be unloaded.", PluginName.c_str());

					return false;
				}
			}
			break;
			case CMD_LIST_PLUGINS:
			{
				return ListPlugins(Feedback_out);
			}
			break;
		}

		return false;
	}

	/*! \brief Verifies that the specified command is valid and is compatible with the invoker
		\param[in] pCommand_in : the command to validate
		\return true if the command is valid; false otherwise
	*/
	bool WindowerEngine::IsCommandValid(const WindowerCommand *pCommand_in)
	{
		if (m_pCommandDispatcher != NULL)
			return m_pCommandDispatcher->IsCommandValid(pCommand_in);
	
		return false;
	}

	/*! \brief Lists all the available plugins and their state (loaded or not)
		\param[out] Feedback_out : the string receiving the result
		\return true if the list was retrieved successfully; false otherwise
	*/
	bool WindowerEngine::ListPlugins(std::string &Feedback_out) const
	{
		if  (m_pPluginManager != NULL)
		{
			const PluginFramework::RegisteredPlugins &Plugins = m_pPluginManager->GetRegisteredPlugins();
			PluginFramework::RegisteredPlugins::const_iterator PluginIt = Plugins.begin();
			Feedback_out = "List of plugins:\n";
			std::string Info;
			UINT Count = 0U;

			for (; PluginIt != Plugins.end(); ++PluginIt)
			{
				convert_ansi(PluginIt->second.ToString(), Info);

				append_format(Feedback_out, "%s\nLoaded:\t%s\n", Info.c_str(),
							  m_pPluginManager->IsPluginLoaded(PluginIt->second.GetName()) ? "yes" : "no");
				++Count;
			}

			if (Count > 0U)
				append_format(Feedback_out, "%u plugin(s) found.", Count);
			else
				append_format(Feedback_out, "No plugin found.");

			return true;
		}

		return false;
	}
}