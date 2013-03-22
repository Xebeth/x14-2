/**************************************************************************
	created		:	2011-07-03
	filename	: 	BootstrapEngine.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin engine used during the game starting process
					Handles the AutoLogin plugin during the login process
**************************************************************************/
#include "stdafx.h"
#include <SettingsManager.h>
#include <PluginFramework.h>
#include <PluginManager.h>
#include <HookEngine.h>

#include <BaseEngine.h>
#include <PluginEngine.h>
#include "BootstrapEngine.h"

#include "WindowerSettings.h"
#include "WindowerSettingsManager.h"

#include "CreateProcessHook.h"
#include "ShellExecuteExHook.h"
#include "CreateWindowExHook.h"

#include <CommandHandler.h>

#include <ICoreModule.h>
#include <WindowerCore.h>
#include <WindowerCommand.h>
#include <CommandDispatcher.h>
#include "SystemCore.h"

namespace Windower
{
	//! pointer to the plugin manager
	PluginManager* ICoreModule::m_pPluginManager = NULL;
}

namespace Bootstrap
{
	/*! \brief BootstrapEngine constructor
		\param[in] pConfigFile_in : the name of the configuration file
	*/
	BootstrapEngine::BootstrapEngine(HMODULE hModule_in, const TCHAR *pConfigFile_in)
		: PluginEngine(hModule_in, pConfigFile_in)
	{
		string_t ConfigPath;

		m_hTarget = GetModuleHandle(NULL);

		// create the settings
		m_pSettingsManager = new Windower::SettingsManager(m_WorkingDir.c_str(), pConfigFile_in);
		m_pSettings = new Windower::WindowerProfile;

		if (m_pSettingsManager->IsGamePathValid() && m_pSettingsManager->LoadDefaultProfile(*m_pSettings))
		{
			// Win32 related hooks
			m_pSystemCore = new SystemCore(*this, m_HookManager);
			// Commander dispatcher
			m_pCommandDispatcher = new Windower::CommandDispatcher(*this, m_HookManager);
			// load plugins
			m_pPluginManager->ListPlugins(m_WorkingDir + _T("plugins"),
										  PLUGIN_COMPATIBILITY_BOOTSTRAP);

			Windower::ICoreModule::SetPluginManager(*m_pPluginManager);
		}
	}
	
	//! \brief BootstrapEngine destructor
	BootstrapEngine::~BootstrapEngine()
	{
		Detach();

		delete m_pCommandDispatcher;
		m_pCommandDispatcher = NULL;

		delete m_pSystemCore;
		m_pSystemCore = NULL;

		delete m_pSettings;
		m_pSettings = NULL;

		delete m_pSettingsManager;
		m_pSettingsManager = NULL;

		delete m_pPluginManager;
		m_pPluginManager = NULL;
	}

	/*! \brief Attaches the engine to the target process through hooking
		\return true if all the hooks were installed successfully; false otherwise
	*/
	bool BootstrapEngine::Attach()
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

	/*! \brief Detaches the engine from the target process and removes all the installed hooks
		\return true if all the hooks were removed successfully; false otherwise
	*/
	bool BootstrapEngine::Detach()
	{
		PluginEngine::Detach();

		return m_HookManager.UninstallRegisteredHooks();
	}

	/*! \brief Checks if the AutoLogin plugin is active
		\return true if the plugin is active; false otherwise
	*/
	bool BootstrapEngine::IsAutoLoginActive() const
	{
		if (m_pSettings != NULL)
			return m_pSettings->IsPluginActive(_T("AutoLogin"));

		return false;
	}

	/*! \brief Loads the AutoLogin plugin and starts the HMTL forms monitoring thread
		\param[in] hParentWnd_in : the handle to the parent window containing the IE server
		\return the result of the command invocation
	*/
	int BootstrapEngine::InvokeAutoLogin(HWND hParentWnd_in)
	{
		if (m_pCommandDispatcher != NULL)
		{
			// check if the AutoLogin plugin is loaded
			if (LoadPlugin(_T("AutoLogin")) != NULL)
			{
				Windower::WindowerCommand *pAutoLoginCmd;

				if ((pAutoLoginCmd = m_pCommandDispatcher->FindCommand("autologin::create_thread")) != NULL)
				{
					if (pAutoLoginCmd->SetPointerValue("hwnd", (long)hParentWnd_in))
					{
						std::string Feedback;
						// execute the command
						return pAutoLoginCmd->Execute(Feedback);
					}
				}
			}
		}

		return Windower::DISPATCHER_RESULT_INVALID_CALL;
	}

	/*! \brief Replaces the parameters from the command line with user settings
		\param[in,out] CmdLine_in_out : the command line to modify
		\return true if the command line was modified; false otherwise
	*/
	bool BootstrapEngine::UpdateCmdLineFromSettings(string_t &CmdLine_in_out)
	{
		std::list<string_t> Parameters;
		string_t Token, LngParam;

		// break up the command line in parameters
		tokenize<TCHAR>(CmdLine_in_out, Parameters, _T(" "), _T("\""));

		while(Parameters.empty() == false)
		{
			Token = Parameters.front();
			Parameters.pop_front();

			if (Token.find(_T("language")) != STRING_T_NPOS)
			{
				// retrieve the language setting from the current profile
				long Lng = m_pSettings->GetLanguage();

				// replace the language by the user setting
				format(LngParam, _T("language=%ld"), Lng);
				replace(CmdLine_in_out, Token, LngParam);

				return true;
			}
		}

		return false;
	}
}