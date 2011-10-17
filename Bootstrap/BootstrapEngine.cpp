/**************************************************************************
	created		:	2011-07-03
	filename	: 	BootstrapEngine.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <SettingsManager.h>
#include <PluginFramework.h>
#include <NonCopyable.h>
#include <HookEngine.h>

#include <BaseEngine.h>
#include <PluginEngine.h>
#include "BootstrapEngine.h"

#include "WindowerSettings.h"
#include "WindowerSettingsManager.h"

#include "CreateProcessHook.h"
#include "ShellExecuteExHook.h"
#include "CreateWindowExHook.h"

#include <ICoreModule.h>
#include <WindowerCore.h>
#include <WindowerCommand.h>
#include <CommandDispatcher.h>
#include "SystemCore.h"

namespace Windower
{
	PluginManager* ICoreModule::m_pPluginManager = NULL;
}

namespace Bootstrap
{
	BootstrapEngine::BootstrapEngine(const TCHAR *pConfigFile_in)
		: PluginEngine(pConfigFile_in)
	{
		m_hTarget = GetModuleHandle(NULL);

		// create the settings
		m_pSettingsManager = new Windower::SettingsManager(pConfigFile_in);
		m_pSettings = new Windower::WindowerProfile;
		// create the hook manager
		m_pHookManager = new HookEngine;

		m_pSettingsManager->LoadDefaultProfile(&m_pSettings);
		// Win32 related hooks
		m_pSystemCore = new SystemCore(*this);
		RegisterModule(_T("System"), m_pSystemCore);
		// Commander dispatcher
		m_pCommandDispatcher = new Windower::CommandDispatcher(*this);
		RegisterModule(_T("CommandDispatcher"), m_pCommandDispatcher);
		// load plugins
		m_pPluginManager->ListPlugins(m_pSettings->GetPluginsAbsoluteDir());
		Windower::ICoreModule::SetPluginManager(m_pPluginManager);
	}
	
	BootstrapEngine::~BootstrapEngine()
	{
		Detach();

		delete m_pHookManager;
		m_pHookManager = NULL;

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

		delete m_pPluginManager;
		m_pPluginManager = NULL;
	}

	bool BootstrapEngine::Attach()
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

	bool BootstrapEngine::Detach()
	{
		PluginEngine::Detach();

		if (m_pHookManager != NULL)
			return m_pHookManager->UninstallRegisteredHooks();

		return false;
	}

	void BootstrapEngine::InvokeAutoLogin(HWND hParentWnd_in)
	{
		if (m_pCommandDispatcher != NULL)
		{
			// load plugins
			if (m_pSettingsManager->GetAutoLogin())
			{
				// check if the AutoLogin plugin is loaded
				if (LoadPlugin(_T("AutoLogin")))
				{
					Windower::WindowerCommand *pAutoLoginCmd;

					if ((pAutoLoginCmd = m_pCommandDispatcher->FindCommand("autologin::startthread")) != NULL)
					{
						 format(pAutoLoginCmd->Parameters["hwnd"].Value, "%ld", hParentWnd_in);
						 m_pCommandDispatcher->Dispatch(*pAutoLoginCmd);
					}
				}
			}
		}
	}
}