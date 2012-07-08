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

#include <CommandHandler.h>
#include "WindowerEngine.h"
#include "PluginsServices.h"

#include "FormatChatMessageHook.h"
#include "RegisterClassExHook.h"
#include "CreateWindowExHook.h"
#include "ReadConfigHook.h"
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

#define _TESTING

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
#if defined _DEBUG && defined _TESTING
		m_pTestCore = new TestCore(*this);
		RegisterModule(_T("Testing"), m_pTestCore);
#endif // _DEBUG
		// Win32 related hooks
		m_pSystemCore = new SystemCore(*this);
		// Commander dispatcher
		m_pCommandDispatcher = new CommandDispatcher(*this);
		// Commander parser
		m_pCommandParser = new CommandParser(*this, *m_pCommandDispatcher);
		// Game chat related hooks
		m_pGameChatCore = new GameChatCore(*this, *m_pCommandParser, *m_pCommandDispatcher);
		// Direct3D related hooks
		m_pGraphicsCore = new GraphicsCore(*this, m_Settings.GetResX(), m_Settings.GetResY(), m_Settings.GetVSync());

		m_pPluginManager->ListPlugins(m_pSettingsManager->GetPluginsAbsoluteDir(),
									  PLUGIN_COMPATIBILITY_WINDOWER);
		ICoreModule::SetPluginManager(*m_pPluginManager);

		// load active plugins
		const ActivePlugins &Plugins = m_Settings.GetActivePlugins();
		ActivePlugins::const_iterator PluginIt;

		for (PluginIt = Plugins.begin(); PluginIt != Plugins.end(); ++PluginIt)
			PluginEngine::LoadPlugin(*PluginIt);

		RegisterCommands();

		// injects the windower version on the main menu
		m_pInjectVersion = new InjectVersion(m_pPluginServices);
	}

	/*! \brief WindowerEngine destructor */
	WindowerEngine::~WindowerEngine()
	{
		UnregisterCommands();
		Detach();

		// destroy before GameChatCore or any subscribed services
		delete m_pInjectVersion;
		m_pInjectVersion = NULL;

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

#if defined _DEBUG && defined _TESTING
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

	/*! \brief Registers the commands with the command dispatcher
		\return true if all the commands were registered successfully; false otherwise
	*/
	bool WindowerEngine::RegisterCommands()
	{
		if (m_pCommandDispatcher != NULL)
		{
			// register the "load" command
			WindowerCommand *pCommand = new WindowerCommand(ENGINE_KEY, CMD_LOAD_PLUGIN, "load",
															"Loads a plugin given its name.", this);
			bool Result = true;

			if (pCommand != NULL)
			{
				pCommand->AddStringParam("plugin", false, "", "the name of the plugin to load");

				if (RegisterCommand(pCommand) == false)
				{
					delete pCommand;
					pCommand = NULL;
				}
			}

			Result &= (pCommand != NULL);
			// register the "unload" command
			pCommand = new WindowerCommand(ENGINE_KEY, CMD_UNLOAD_PLUGIN, "unload",
										   "Unloads a plugin given its name.", this);

			if (pCommand != NULL)
			{
				pCommand->AddStringParam("plugin", false, "", "the name of the plugin to unload");

				if (RegisterCommand(pCommand) == false)
				{
					delete pCommand;
					pCommand = NULL;
				}
			}

			Result &= (pCommand != NULL);

			// register the "list" command
			pCommand = new WindowerCommand(ENGINE_KEY, CMD_LIST_PLUGINS, "list",
										   "Gives a list of the available plugins.", this);

			if (pCommand != NULL && RegisterCommand(pCommand) == false)
			{
				delete pCommand;
				pCommand = NULL;
			}

			Result &= (pCommand != NULL);

			return Result;
		}

		return false;
	}

	/*! \brief Inserts a command in the collection of registered commands
		\param[in] pCommand_in : the command to add
	*/
	bool WindowerEngine::RegisterCommand(WindowerCommand *pCommand_in)
	{
		if (m_pCommandDispatcher != NULL)
			return m_pCommandDispatcher->RegisterCommand(pCommand_in);

		return false;
	}

	/*! \brief Removes a command from the collection of registered commands
		\param[in] pCommand_in : the command to remove
	*/
	bool WindowerEngine::UnregisterCommand(WindowerCommand *pCommand_in)
	{
		bool Result = false;

		if (m_pCommandDispatcher != NULL)
		{
			Result = m_pCommandDispatcher->UnregisterCommand(pCommand_in);
			delete pCommand_in;
		}

		return Result;
	}

	/*! \brief Unregisters the commands with the command dispatcher
		\return true if all the commands were unregistered successfully; false otherwise
	*/
	bool WindowerEngine::UnregisterCommands()
	{
		if (m_pCommandDispatcher != NULL)
		{
			WindowerCommand *pCommand;
			bool Result = true;

			pCommand = m_pCommandDispatcher->FindCommand("unload");
			Result &= UnregisterCommand(pCommand);

			pCommand = m_pCommandDispatcher->FindCommand("unload");
			Result &= UnregisterCommand(pCommand);

			pCommand = m_pCommandDispatcher->FindCommand("unload");
			Result &= UnregisterCommand(pCommand);

			return Result;
		}

		return false;
	}

	/*! \brief Verifies that the specified command is valid and is compatible with the invoker
		\param[in] pCommand_in : the command to validate
		\return true if the command is valid; false otherwise
	*/
	bool WindowerEngine::IsCommandValid(const WindowerCommand *pCommand_in) const
	{
		if (m_pCommandDispatcher != NULL)
			return m_pCommandDispatcher->IsCommandValid(pCommand_in);

		return false;
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
					format(Feedback_out, "The plugin '%s' couldn't be loaded.", PluginName.c_str());

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

	/*! \brief Lists all the available plugins and their state (loaded or not)
		\param[out] Feedback_out : the string receiving the result
		\return true if the list was retrieved successfully; false otherwise
	*/
	bool WindowerEngine::ListPlugins(std::string &Feedback_out) const
	{
		if  (m_pPluginManager != NULL)
		{
			const PluginFramework::RegisteredPlugins &Plugins = m_pPluginManager->GetRegisteredPlugins();
			PluginFramework::RegisteredPlugins::const_iterator PluginEnd = Plugins.end();
			PluginFramework::RegisteredPlugins::const_iterator PluginIt = Plugins.begin();
			std::string Info, LoadedList, AvailableList;
			UINT LoadedCount = 0U, AvailableCount = 0U;

			for (; PluginIt != PluginEnd; ++PluginIt)
			{
				convert_ansi(PluginIt->second.ToString(), Info);

				if (m_pPluginManager->IsPluginLoaded(PluginIt->second.GetName()))
				{
					append_format(LoadedList, "- %s", Info.c_str());
					++LoadedCount;
				}
				else
				{
					append_format(AvailableList, "- %s", Info.c_str());
					++AvailableCount;
				}
			}

			if (LoadedCount > 0U)
			{
				append_format(Feedback_out, "Loaded plugins (%u found):\n%s",
							  LoadedCount, LoadedList.c_str());
			}

			if (AvailableCount > 0U)
			{
				if (LoadedCount > 0U)
					Feedback_out += "\n";

				append_format(Feedback_out, "Available plugins (%u found):\n%s",
							  AvailableCount, AvailableList.c_str());
			}

			return true;
		}

		return false;
	}
}