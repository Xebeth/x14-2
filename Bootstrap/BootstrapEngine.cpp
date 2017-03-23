/**************************************************************************
	created		:	2011-07-03
	filename	: 	BootstrapEngine.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin engine used during the game starting process
					Handles the AutoLogin plugin during the login process
**************************************************************************/
#include "stdafx.h"
#include "BootstrapEngine.h"

#include "CreateWindowExHook.h"
#include "CreateProcessHook.h"
#include "SystemCore.h"

namespace Bootstrap
{
	/*! \brief BootstrapEngine constructor
		\param[in] pConfigFile_in : the name of the configuration file
	*/
	BootstrapEngine::BootstrapEngine(const string_t &WorkingDir_in, const TCHAR *pConfigFile_in)
		: PluginEngine(WorkingDir_in, pConfigFile_in)
	{
		string_t ConfigPath;

		m_hTarget = GetModuleHandle(nullptr);

		// create the settings
		m_pSettingsManager = new Windower::SettingsManager(m_WorkingDir.c_str(), pConfigFile_in);
		m_pSettings = new Windower::WindowerProfile;

		if (m_pSettingsManager->IsGamePathValid() && m_pSettingsManager->LoadDefaultProfile(*m_pSettings))
		{
			// Win32 related hooks
			m_pSystemCore = new SystemCore(*this, m_HookManager);
			// load plugins
			m_pPluginManager->ListPlugins(m_WorkingDir + PLUGIN_DIRECTORY,
										  PLUGIN_COMPATIBILITY_BOOTSTRAP);
			// install the hooks
			BootstrapEngine::Attach();
		}
	}
	
	//! \brief BootstrapEngine destructor
	BootstrapEngine::~BootstrapEngine()
	{
		// detach will not unload plugins if called from
		// dllmain which is sadly the most likely scenario
		BootstrapEngine::Detach();

		delete m_pSystemCore;
		m_pSystemCore = nullptr;

		delete m_pSettings;
		m_pSettings = nullptr;

		delete m_pSettingsManager;
		m_pSettingsManager = nullptr;

		delete m_pPluginManager;
		m_pPluginManager = nullptr;
	}

	/*! \brief Attaches the engine to the target process through hooking
		\return true if all the hooks were installed successfully; false otherwise
	*/
	bool BootstrapEngine::Attach()
	{
		CoreModules::const_iterator ModuleIt, EndIt = m_Modules.cend();
		Windower::ICoreModule *pModule;

		for (ModuleIt = m_Modules.cbegin(); ModuleIt != EndIt; ++ModuleIt)
		{
			pModule = ModuleIt->second;

			if (pModule != nullptr)
				pModule->RegisterHooks(m_HookManager);
		}

		if (m_HookManager.InstallRegisteredHooks())
		{
			for (ModuleIt = m_Modules.cbegin(); ModuleIt != EndIt; ++ModuleIt)
			{
				pModule = ModuleIt->second;

				if (pModule != nullptr)
				{
					pModule->OnHookInstall(m_HookManager);
					pModule->RegisterServices();
				}
			}

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

		return m_HookManager.UnregisterHooks();
	}

	/*! \brief Checks if the AutoLogin plugin is active
		\return true if the plugin is active; false otherwise
	*/
	bool BootstrapEngine::IsAutoLoginActive() const
	{
		if (m_pSettings != nullptr)
			return m_pSettings->IsPluginActive(_T("AutoLogin"));

		return false;
	}

	/*! \brief Loads the AutoLogin plugin and starts the HMTL forms monitoring thread
		\return true if the plugin was loaded; false otherwise
	*/
	bool BootstrapEngine::InvokeAutoLogin()
	{
		// load the AutoLogin plugin
		if (IsAutoLoginActive())
			return (LoadPlugin(_T("AutoLogin")) != nullptr);

		return false;
	}

	/*! \brief Replaces the parameters from the command line with user settings
		\param[in,out] CmdLine_in_out : the command line to modify
		\return true if the command line was modified; false otherwise
	*/
	bool BootstrapEngine::UpdateCmdLineFromSettings(string_t &CmdLine_in_out) const
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