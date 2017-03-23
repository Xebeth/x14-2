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

	PluginEngine::PluginEngine(const string_t &WorkingDir_in, const TCHAR *pConfigFile_in)
		: m_pPluginManager(nullptr), m_pPluginServices(nullptr), m_pSettingsManager()
	{
		// set the working directory
		SetWorkingDir(WorkingDir_in.c_str());
		// create the settings manager
		m_pSettingsManager = new SettingsManager(m_WorkingDir.c_str(), pConfigFile_in);
		// create the plugin services
		m_pPluginServices = new PluginServices(m_FrameworkVersion, m_Modules, m_pSettingsManager);
		// create the plugin manager
		m_pPluginManager = new PluginFramework::PluginManager(m_pPluginServices);
	}

	PluginEngine::~PluginEngine()
	{
		if (m_pSettingsManager != nullptr)
		{
			delete m_pSettingsManager;
			m_pSettingsManager = nullptr;
		}

		if (m_pPluginManager != nullptr)
		{
			delete m_pPluginManager;
			m_pPluginManager = nullptr;
		}

		if (m_pPluginServices != nullptr)
		{
			delete m_pPluginServices;
			m_pPluginServices = nullptr;
		}
	}

	bool PluginEngine::Detach()
	{
		// destroy the created plugins
		WindowerPlugins::const_iterator pluginIt;

		while (m_Plugins.empty() == false)
		{
			pluginIt = m_Plugins.cbegin();
			UnloadPlugin(pluginIt->first);
		}

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

		return nullptr;
	}

	PluginFramework::IPlugin* PluginEngine::LoadPlugin(const string_t& PluginName_in, bool ForceReload_in)
	{
		PluginFramework::IPlugin* pPlugin = nullptr;

		if (ForceReload_in)
			UnloadPlugin(PluginName_in);

		// if the plugin hasn't already been loaded
		if (ForceReload_in == false || (pPlugin = GetPluginInstance(PluginName_in)) == nullptr)
		{
			// the plugin already exists
			if (pPlugin == nullptr)
			{
				// try to load it
				pPlugin = m_pPluginManager->LoadPlugin(PluginName_in);
				// if the plugin was loaded successfuly; store it
				if (pPlugin != nullptr)
					m_Plugins[PluginName_in] = pPlugin;
			}
		}

		return pPlugin;
	}

	bool PluginEngine::UnloadPlugin(const string_t& PluginName_in)
	{
		PluginFramework::IPlugin *pPlugin = GetPluginInstance(PluginName_in);

		if (pPlugin != nullptr && m_pPluginManager->UnloadPlugin(PluginName_in))
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

		if (pPlugin != nullptr)
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
		if  (m_pPluginManager != nullptr)
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

	const TCHAR* PluginEngine::GetCompatibilityFlagsText(DWORD Flags_in)
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

	void PluginEngine::SetWorkingDir(const TCHAR *pWorkingDir_in)
	{
		if (pWorkingDir_in != nullptr)
			initialize_path(pWorkingDir_in, m_WorkingDir);
	}

	const string_t& PluginEngine::GetWorkingDir() const
	{
		return m_WorkingDir;
	}

	const TCHAR* PluginEngine::GetGameDirectory() const
	{
		return m_pSettingsManager->GetGamePath();
	}

	void PluginEngine::PushPluginConfigure(PluginFramework::IPlugin *pPlugin_in)
	{
		if (pPlugin_in != nullptr && pPlugin_in->IsConfigurable())
			m_ConfigQueue.push_back(pPlugin_in);
	}

	bool PluginEngine::PopPluginConfigure()
	{
		if (m_ConfigQueue.empty() == false)
		{
			PluginFramework::IPlugin *pPlugin = m_ConfigQueue.front();

			if (pPlugin != nullptr && pPlugin->IsConfigurable())
			{
				// remove the plugin from the queue
				m_ConfigQueue.erase(m_ConfigQueue.cbegin());

				return pPlugin->Configure(nullptr);
			}
		}

		return true;
	}
}