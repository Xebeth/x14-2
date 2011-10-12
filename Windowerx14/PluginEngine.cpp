/**************************************************************************
	created		:	2011-07-03
	filename	: 	PluginEngine.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <NonCopyable.h>
#include <HookEngine.h>

#include "PluginsServices.h"

#include "BaseEngine.h"
#include "PluginEngine.h"

namespace Windower
{
	PluginEngine::PluginEngine(const TCHAR *pConfigFile_in)
	{
		// create the plugin services
		m_pPluginServices = new PluginServices(_T("1.0.0"), m_Modules);
		// create the plugin manager
		m_pPluginManager = new PluginManager(m_pPluginServices);											 
	}

	PluginEngine::~PluginEngine()
	{
		delete m_pPluginServices;
		m_pPluginServices = NULL;

		delete m_pPluginManager;
		m_pPluginManager = NULL;
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

	PluginFramework::IPlugin* PluginEngine::GetPluginInstance(const string_t &PluginName_in)
	{
		WindowerPlugins::const_iterator Iter = m_Plugins.find(PluginName_in);

		if (Iter != m_Plugins.end())
			return Iter->second;

		return NULL;
	}

	bool PluginEngine::LoadPlugin(const string_t &PluginName_in, bool ForceReload_in)
	{
		PluginFramework::IPlugin* pPlugin = NULL;

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

		return (pPlugin != NULL);
	}

	bool PluginEngine::UnloadPlugin(const string_t &PluginName_in)
	{
		PluginFramework::IPlugin *pPlugin = GetPluginInstance(PluginName_in);

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