/**************************************************************************
	created		:	2011-07-03
	filename	: 	PluginEngine.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin engine
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <HookEngine.h>

#include "PluginsServices.h"
#include "PluginManager.h"
#include "PluginEngine.h"

namespace Windower
{
	using namespace PluginFramework;

	const VersionInfo PluginEngine::m_FrameworkVersion(__PLUGIN_FRAMEWORK_VERSION__);
	PluginServices *PluginEngine::m_pPluginServices = NULL;

	PluginEngine::PluginEngine(const TCHAR *pConfigFile_in)
	{
		m_pPluginServices = new PluginServices(m_FrameworkVersion, m_Modules);
		// create the plugin manager
		m_pPluginManager = new PluginManager(m_pPluginServices);

//		m_pPluginManager->BlacklistPlugin(_T("745E1230-0C81-4220-B099-3A3392EFA03A"));
	}

	PluginEngine::~PluginEngine()
	{
		if (m_pPluginServices != NULL)
		{
			delete m_pPluginServices;
			m_pPluginServices = NULL;
		}

		if (m_pPluginManager != NULL)
		{
			delete m_pPluginManager;
			m_pPluginManager = NULL;
		}
	}

	bool PluginEngine::Detach()
	{
		// destroy the created plugins
		while(m_Plugins.empty() == false)
			UnloadPlugin(m_Plugins.rbegin()->first);

		m_Plugins.clear();

		return true;
	}

	bool PluginEngine::RegisterModule(const string_t ModuleName_in, ICoreModule *pModule_in)
	{
		m_Modules[ModuleName_in] = pModule_in;

		return true;
	}

	bool PluginEngine::UnregisterModule(const string_t ModuleName_in)
	{
		CoreModules::const_iterator Iter = m_Modules.find(ModuleName_in);

		if (Iter != m_Modules.end())
		{
			m_Modules.erase(Iter);

			return true;
		}

		return false;
	}

	IPlugin* PluginEngine::GetPluginInstance(const string_t &PluginName_in)
	{
		WindowerPlugins::const_iterator Iter = m_Plugins.find(PluginName_in);

		if (Iter != m_Plugins.end())
			return Iter->second;

		return NULL;
	}

	IPlugin* PluginEngine::LoadPlugin(const string_t &PluginName_in, bool ForceReload_in)
	{
		IPlugin* pPlugin = NULL;

		if (ForceReload_in)
			UnloadPlugin(PluginName_in);

		// if the plugin hasn't already been loaded
		if (ForceReload_in == false || (pPlugin = GetPluginInstance(PluginName_in)) == NULL)
		{
			// the plugin already exists
			if (pPlugin == NULL)
			{
				// try to load it
				pPlugin = m_pPluginManager->LoadPlugin(PluginName_in);
				// if the plugin was loaded successfuly; store it
				if (pPlugin != NULL)
					m_Plugins[PluginName_in] = pPlugin;
			}
		}

		return pPlugin;
	}

	bool PluginEngine::UnloadPlugin(const string_t &PluginName_in)
	{
		IPlugin *pPlugin = GetPluginInstance(PluginName_in);

		if (pPlugin != NULL && m_pPluginManager->UnloadPlugin(PluginName_in))
		{
			WindowerPlugins::const_iterator Iter = m_Plugins.find(PluginName_in);

			if (Iter != m_Plugins.end())
				m_Plugins.erase(Iter);

			return true;
		}

		return false;
	}
}