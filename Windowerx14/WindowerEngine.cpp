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

#define _TESTING

namespace Windower
{
	PluginManager* ICoreModule::m_pPluginManager = NULL;

	/*! \brief WindowerEngine constructor
		\param[in] pConfigFile_in : path to the configuration file
	*/
	WindowerEngine::WindowerEngine(HMODULE hModule_in, const TCHAR *pConfigFile_in)
		: PluginEngine(hModule_in, pConfigFile_in), m_bThreadInit(false), m_pGameChatCore(NULL),
		  m_bShutdown(false), m_hGameWnd(NULL), m_pCommandDispatcher(NULL), m_pSystemCore(NULL)
	{
		// create the settings manager
		m_pSettingsManager = new SettingsManager(m_WorkingDir.c_str(), pConfigFile_in);
		// testing
#if defined _DEBUG && defined _TESTING
		// create the testing module
		m_pTestCore = new TestCore(*this, m_HookManager);
#endif // _DEBUG

		// register the built-in commands
		RegisterCommands();

		// check the settings and load the default profile
		if (m_pSettingsManager->IsGamePathValid() && m_pSettingsManager->LoadDefaultProfile(m_Settings))
		{
			// create the game chat module
			// m_pGameChatCore = new GameChatCore(*this, m_HookManager);
			// create the graphics module
			m_pGraphicsCore = new GraphicsCore(*this, m_HookManager, m_Settings.GetVSync());
			// list the available plugins compatible with windower
			m_pPluginManager->ListPlugins(m_WorkingDir + _T("plugins"),
										  PLUGIN_COMPATIBILITY_WINDOWER);
			// set the plugin manager of the modules
			ICoreModule::SetPluginManager(*m_pPluginManager);
			// load active plugins
			LoadPlugins(m_Settings.GetActivePlugins());
		}
	}

	/*! \brief WindowerEngine destructor */
	WindowerEngine::~WindowerEngine()
	{
		UnregisterCommands();
		Detach();

		delete m_pSettingsManager;
		m_pSettingsManager = NULL;

		delete m_pGameChatCore;
		m_pGameChatCore = NULL;

		delete m_pGraphicsCore;
		m_pGraphicsCore = NULL;

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

			pCommand = m_pCommandDispatcher->FindCommand("load");
			Result &= UnregisterCommand(pCommand);

			pCommand = m_pCommandDispatcher->FindCommand("list");
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