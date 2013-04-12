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

namespace Windower
{
	const PluginFramework::VersionInfo PluginEngine::m_FrameworkVersion(__PLUGIN_FRAMEWORK_VERSION__);
	PluginServices *PluginEngine::m_pPluginServices = NULL;

	PluginEngine::PluginEngine(HMODULE hModule_in, const wchar_t *pConfigFile_in)
	{
		// set the working directory
		SetWorkingDir(hModule_in);
		// create the plugin services
		m_pPluginServices = new PluginServices(m_FrameworkVersion, m_Modules, m_WorkingDir + pConfigFile_in);
		// create the plugin manager
		m_pPluginManager = new PluginFramework::PluginManager(m_pPluginServices);
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

	bool PluginEngine::RegisterModule(const string_t& ModuleName_in, ICoreModule *pModule_in)
	{
		m_Modules[ModuleName_in] = pModule_in;

		return true;
	}

	bool PluginEngine::UnregisterModule(const string_t& ModuleName_in)
	{
		CoreModules::const_iterator Iter = m_Modules.find(ModuleName_in);

		if (Iter != m_Modules.end())
		{
			m_Modules.erase(Iter);

			return true;
		}

		return false;
	}

	PluginFramework::IPlugin* PluginEngine::GetPluginInstance(const string_t& PluginName_in)
	{
		WindowerPlugins::const_iterator Iter = m_Plugins.find(PluginName_in);

		if (Iter != m_Plugins.end())
			return Iter->second;

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
			WindowerPlugins::const_iterator Iter = m_Plugins.find(PluginName_in);

			if (Iter != m_Plugins.end())
				m_Plugins.erase(Iter);

			return true;
		}

		return false;
	}

	size_t PluginEngine::LoadPlugins(const ActivePlugins& PluginSet_in)
	{
		size_t Count = 0U;

		ActivePlugins::const_iterator PluginIt;

		for (PluginIt = PluginSet_in.begin(); PluginIt != PluginSet_in.end(); ++PluginIt)
			if (LoadPlugin(*PluginIt))
				++Count;

		return Count;
	}

	bool PluginEngine::ListPlugins(std::string& Feedback_out) const
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
					append_format(LoadedList, "\xe2\x87\x92 %s", Info.c_str());
					++LoadedCount;
				}
				else
				{
					append_format(AvailableList, "\xe2\x87\x92 %s", Info.c_str());
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
		switch(Flags_in)
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
		wchar_t *pLastSlash = NULL;

		// retrieve the name of the module
		GetModuleFileName(hModule_in, DirPath, _MAX_PATH);
		// find the last backslash
		pLastSlash = wcsrchr(DirPath, '\\') + 1;

		if (pLastSlash != NULL)
		{
			*pLastSlash = '\0';
			// set the working directory
			m_WorkingDir.assign(DirPath);
		}
	}

	const string_t& PluginEngine::GetWorkingDir() const
	{
		return m_WorkingDir;
	}
}