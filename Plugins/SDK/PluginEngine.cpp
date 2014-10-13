/**************************************************************************
	created		:	2011-07-03
	filename	: 	PluginEngine.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin engine
**************************************************************************/
#include "stdafx.h"

#include "BaseEngine.h"
#include "PluginEngine.h"

#include "PluginsServices.h"
#include "WindowerSettings.h"
#include "WindowerSettingsManager.h"

namespace Windower
{
	const PluginFramework::VersionInfo PluginEngine::m_FrameworkVersion(__PLUGIN_FRAMEWORK_VERSION__);

	PluginEngine::PluginEngine(HMODULE hModule_in, const wchar_t *pConfigFile_in)
		: m_pPluginManager(NULL), m_pPluginServices(NULL), m_pSettingsManager()
	{
		// set the working directory
		SetWorkingDir(hModule_in);
		// create the settings manager
		m_pSettingsManager = new SettingsManager(m_WorkingDir.c_str(), pConfigFile_in);
		// create the plugin services
		m_pPluginServices = new PluginServices(m_FrameworkVersion, m_Modules, m_pSettingsManager);
		// create the plugin manager
		m_pPluginManager = new PluginFramework::PluginManager(m_pPluginServices);
	}

	PluginEngine::~PluginEngine()
	{
		if (m_pSettingsManager != NULL)
		{
			delete m_pSettingsManager;
			m_pSettingsManager = NULL;
		}

		if (m_pPluginManager != NULL)
		{
			delete m_pPluginManager;
			m_pPluginManager = NULL;
		}

		if (m_pPluginServices != NULL)
		{
			delete m_pPluginServices;
			m_pPluginServices = NULL;
		}
	}

	bool PluginEngine::Detach()
	{
		// destroy the created plugins
		while(m_Plugins.empty() == false)
			UnloadPlugin(m_Plugins.rbegin()->first);

		return true;
	}

	bool PluginEngine::RegisterModule(const string_t& ModuleName_in, ICoreModule *pModule_in)
	{
		m_Modules[ModuleName_in] = pModule_in;

		return true;
	}

	bool PluginEngine::UnregisterModule(const string_t& ModuleName_in)
	{
		CoreModules::const_iterator ModuleIt = m_Modules.find(ModuleName_in);

		if (ModuleIt != m_Modules.cend())
		{
			m_Modules.erase(ModuleIt);

			return true;
		}

		return false;
	}

	PluginFramework::IPlugin* PluginEngine::GetPluginInstance(const string_t& PluginName_in)
	{
		WindowerPlugins::const_iterator PluginIt = m_Plugins.find(PluginName_in);

		if (PluginIt != m_Plugins.cend())
			return PluginIt->second;

		return NULL;
	}

	PluginFramework::IPlugin* PluginEngine::LoadPlugin(const string_t& PluginName_in, bool ForceReload_in)
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

		return pPlugin;
	}

	bool PluginEngine::UnloadPlugin(const string_t& PluginName_in)
	{
		PluginFramework::IPlugin *pPlugin = GetPluginInstance(PluginName_in);

		if (pPlugin != NULL && m_pPluginManager->UnloadPlugin(PluginName_in))
		{
			WindowerPlugins::const_iterator PluginIt = m_Plugins.find(PluginName_in);

			if (PluginIt != m_Plugins.cend())
				m_Plugins.erase(PluginIt);

			return true;
		}

		return false;
	}

	bool PluginEngine::ConfigurePlugin(string_t &PluginName_in)
	{
		PluginFramework::IPlugin *pPlugin = GetPluginInstance(PluginName_in);

		if (pPlugin != NULL)
		{
			PushPluginConfigure(pPlugin);

			return true;
		}

		return false;
	}


	size_t PluginEngine::LoadPlugins(const ActivePlugins& PluginSet_in)
	{
		size_t Count = 0U;

		ActivePlugins::const_iterator PluginIt, EndIt = PluginSet_in.cend();

		for (PluginIt = PluginSet_in.cbegin(); PluginIt != EndIt; ++PluginIt)
			if (LoadPlugin(*PluginIt))
				++Count;

		return Count;
	}

	bool PluginEngine::ListPlugins(std::string& Feedback_out) const
	{
		if  (m_pPluginManager != NULL)
		{
			const PluginFramework::RegisteredPlugins &Plugins = m_pPluginManager->GetRegisteredPlugins();
			PluginFramework::RegisteredPlugins::const_iterator PluginIt, PluginEnd = Plugins.cend();
			std::string Info, LoadedList, AvailableList;
			UINT LoadedCount = 0U, AvailableCount = 0U;

			for (PluginIt = Plugins.cbegin(); PluginIt != PluginEnd; ++PluginIt)
			{
				convert_ansi(PluginIt->second.ToString(), Info);

				if (m_pPluginManager->IsPluginLoaded(PluginIt->second.GetName()))
				{
					append_format(LoadedList, "\xee\x81\xaf %s", Info.c_str());
					++LoadedCount;
				}
				else
				{
					append_format(AvailableList, "\xee\x81\xaf %s", Info.c_str());
					++AvailableCount;
				}
			}

			if (LoadedCount > 0U)
			{
				append_format(Feedback_out, "Loaded plugins (%u found):\n%s",
							  LoadedCount, LoadedList.c_str());
			}
			else
			{
				Feedback_out += "No plugin loaded.\n";
			}

			if (AvailableCount > 0U)
			{
				if (LoadedCount > 0U)
					Feedback_out += '\n';

				append_format(Feedback_out, "Available plugins (%u found):\n%s",
							  AvailableCount, AvailableList.c_str());
			}

			return true;
		}

		return false;
	}

	const wchar_t* PluginEngine::GetCompatibilityFlagsText(DWORD Flags_in)
	{
		DWORD Flags = (Flags_in & PLUGIN_COMPATIBILITY_MASK);

		switch (Flags)
		{
			case PLUGIN_COMPATIBILITY_WINDOWER:
				return L"Windower";
			case PLUGIN_COMPATIBILITY_BOOTSTRAP:
				return L"Bootstrap";
			case PLUGIN_COMPATIBILITY_ANY:
				return L"Any";
		}

		return L"None";
	}

	void PluginEngine::SetWorkingDir(HMODULE hModule_in)
	{
		wchar_t DirPath[_MAX_PATH] = { '\0' };
		
		// retrieve the name of the module
		if (GetModuleFileName(hModule_in, DirPath, _MAX_PATH) != 0UL)
		{
			// find the last backslash
			wchar_t *pLastSlash = pLastSlash = wcsrchr(DirPath, '\\') + 1;

			if (pLastSlash != NULL)
			{
				*pLastSlash = '\0';
				// set the working directory
				m_WorkingDir.assign(DirPath);
			}
		}
	}

	const string_t& PluginEngine::GetWorkingDir() const
	{
		return m_WorkingDir;
	}

	void PluginEngine::PushPluginConfigure(PluginFramework::IPlugin *pPlugin_in)
	{
		if (pPlugin_in != NULL && pPlugin_in->IsConfigurable())
			m_ConfigQueue.push_back(pPlugin_in);
	}

	bool PluginEngine::PopPluginConfigure()
	{
		if (m_ConfigQueue.empty() == false)
		{
			PluginFramework::IPlugin *pPlugin = m_ConfigQueue.front();

			if (pPlugin != NULL && pPlugin->IsConfigurable())
			{
				// remove the plugin from the queue
				m_ConfigQueue.erase(m_ConfigQueue.cbegin());

				return pPlugin->Configure(NULL);
			}
		}

		return true;
	}
}