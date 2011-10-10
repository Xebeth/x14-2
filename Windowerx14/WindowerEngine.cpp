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
		PluginEngine::LoadPlugin(_T("Tell detect"));
		PluginEngine::LoadPlugin(_T("Timestamp"));

		// register commands
		CallerParam Caller("WindowerEngine", this);
		const char *pParamName = "text";
		CommandParameters InjectParams;

		// register the "list" command
		m_pCommandDispatcher->RegisterCommand(PLUGIN_REGKEY, "list", "Lists the loaded plugins.", Caller, ListPlugins);

		// register the "inject" command
		InjectParams[pParamName].Name = pParamName;
		InjectParams[pParamName].Type = COMMAND_PARAM_TYPE_STRING;
		InjectParams[pParamName].Value = "";
		InjectParams[pParamName].Description = "text to inject in the chat log";
		m_pCommandDispatcher->RegisterCommand(PLUGIN_REGKEY, "inject", "Injects text in the chat log.", Caller, InjectText, 1, 1, InjectParams, false, true);
		// register the "load" command
		CommandParameters PluginParams;
		pParamName = "plugin_name";

		PluginParams[pParamName].Name = pParamName;
		PluginParams[pParamName].Type = COMMAND_PARAM_TYPE_STRING;
		PluginParams[pParamName].Value = "";
		PluginParams[pParamName].Description = "the name of the plugin to load";
		m_pCommandDispatcher->RegisterCommand(PLUGIN_REGKEY, "load", "Loads a plugin given its name.", Caller, LoadPlugin, 1, 1, PluginParams);
		// register the "unload" command
		InjectParams[pParamName].Description = "the name of the plugin to unload";
		m_pCommandDispatcher->RegisterCommand(PLUGIN_REGKEY, "unload", "Unloads a plugin given its name.", Caller, UnloadPlugin, 1, 1, PluginParams);
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
				Sleep(100);
			}
		}

		return 0L;
	}

	void WindowerEngine::UpdateEngine()
	{

		if (m_FeedbackMessages.empty() == false)
		{
			InjectText(m_FeedbackMessages.front());
			m_FeedbackMessages.pop_front();
		}

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

	/*! \brief Inject text command callback ("inject")
		\param[in] pCommand_in : the command passed by the command dispatcher
		\return DISPATCHER_RESULT_SUCCESS if the command was successful; DISPATCHER_RESULT_INVALID_CALL otherwise
	*/
	int WindowerEngine::InjectText(const WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL && pCommand_in->Caller.DataType.compare("WindowerEngine") == 0)
		{
			WindowerEngine *pEngine = reinterpret_cast<WindowerEngine*>(pCommand_in->Caller.pData);
			const WindowerCommandParam *pParam = pCommand_in->GetParameter("text");

			if (pEngine != NULL && pParam != NULL && pEngine->InjectText(pParam->Value))
				return DISPATCHER_RESULT_SUCCESS;
		}

		return DISPATCHER_RESULT_INVALID_CALL;
	}

	/*! \brief Inject text command implementation ("inject")
		\param[in] Text_in : the text to inject in the chat log
		\param[in] MessageType_in : the type of the message
		\return true if the text was injected in the chat log; false otherwise
	*/
	bool WindowerEngine::InjectText(const std::string& Text_in, USHORT MessageType_in)
	{
		GameChatTextObject Message;

		Message.dwSize = Text_in.length() + 1;
		Message.pResBuf = Text_in.c_str();		
		Message.dwUnknown = 0x0040;
		
		return m_pGameChatCore->FormatChatMessageHook(NULL, MessageType_in, NULL, &Message);
	}

	/*! \brief List plugins command callback ("list")
		\param[in] pCommand_in : the command passed by the command dispatcher
		\return DISPATCHER_RESULT_SUCCESS if the command was successful; DISPATCHER_RESULT_INVALID_CALL otherwise
	*/
	int WindowerEngine::ListPlugins(const WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL && pCommand_in->Caller.DataType.compare("WindowerEngine") == 0)
		{
			WindowerEngine *pEngine = reinterpret_cast<WindowerEngine*>(pCommand_in->Caller.pData);

			if (pEngine != NULL && pEngine->ListPlugins())
				return DISPATCHER_RESULT_SUCCESS;
		}

		return DISPATCHER_RESULT_INVALID_CALL;
	}

	/*! \brief List plugins command implementation ("list")
		\return true if the plugins were listed successfully; false otherwise
	*/
	bool WindowerEngine::ListPlugins()
	{
		if (m_pPluginManager != NULL)
		{
			size_t PluginCount = m_Plugins.size();
			std::string PluginData;

			switch(PluginCount)
			{
				case 0:
					PluginData = "No plugin is currently loaded.";
				break;
				case 1:
					PluginData = "1 plugin is currently loaded:";
				break;
				default:
					format(PluginData, "%i plugins are currently loaded:", PluginCount);
			}

			if (PluginCount > 0)
			{
				WindowerPlugins::const_iterator PluginIt;
				string_t PluginDescW;				

				InjectText(PluginData);

				for (PluginIt = m_Plugins.begin(); PluginIt != m_Plugins.end(); ++PluginIt)
				{
					PluginDescW = PluginIt->second->GetInfo().ToString();
					
					InjectText(convert_ansi(PluginDescW, PluginData));
				}

				return true;
			}
			else
				return InjectText(PluginData);
		}

		return false;
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
				std::string Feedback;
				string_t PluginName;

				if (static_cast<PluginEngine*>(pEngine)->UnloadPlugin(pParam->GetWideStringValue(PluginName)))
					format(Feedback,"The plugin '%s' was unloaded successfully.", pParam->Value.c_str());
				else
					format(Feedback, "The plugin '%s' couldn't be unloaded.", pParam->Value.c_str());

				// pEngine->AddFeedbackMessage(Feedback);

				return DISPATCHER_RESULT_SUCCESS;
			}
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
					format(Feedback, "The plugin '%s' was loaded successfully.", pParam->Value.c_str());
				else
					format(Feedback, "The plugin '%s' couldn't be loaded.", pParam->Value.c_str());

				pEngine->AddFeedbackMessage(Feedback);

				return DISPATCHER_RESULT_SUCCESS;
			}
		}

		return DISPATCHER_RESULT_INVALID_CALL;
	}
}