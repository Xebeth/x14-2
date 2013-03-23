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

#include "utf8_convert.h"

namespace Windower
{
	using namespace PluginFramework;

	const VersionInfo PluginEngine::m_FrameworkVersion(__PLUGIN_FRAMEWORK_VERSION__);
	PluginServices *PluginEngine::m_pPluginServices = NULL;

	PluginEngine::PluginEngine(HMODULE hModule_in, const TCHAR *pConfigFile_in)
	{
		// set the working directory
		SetWorkingDir(hModule_in);
		// create the plugin services
		m_pPluginServices = new PluginServices(m_FrameworkVersion, m_Modules, m_WorkingDir + pConfigFile_in);
		// create the plugin manager
		m_pPluginManager = new PluginManager(m_pPluginServices);
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

	IPlugin* PluginEngine::GetPluginInstance(const string_t& PluginName_in)
	{
		WindowerPlugins::const_iterator Iter = m_Plugins.find(PluginName_in);

		if (Iter != m_Plugins.end())
			return Iter->second;

		return NULL;
	}

	IPlugin* PluginEngine::LoadPlugin(const string_t& PluginName_in, bool ForceReload_in)
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

	bool PluginEngine::UnloadPlugin(const string_t& PluginName_in)
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

	/*! \brief Loads a set of plugins
		\param[in] PluginSet_in : a list of plugins name to load
		\return the number of loaded plugins
	*/
	size_t PluginEngine::LoadPlugins(const ActivePlugins& PluginSet_in)
	{
		size_t Count = 0U;

		ActivePlugins::const_iterator PluginIt;

		for (PluginIt = PluginSet_in.begin(); PluginIt != PluginSet_in.end(); ++PluginIt)
			if (LoadPlugin(*PluginIt))
				++Count;

		return Count;
	}

   

	/*! \brief Lists all the available plugins and their state (loaded or not)
		\param[out] Feedback_out : the string receiving the result
		\return true if the list was retrieved successfully; false otherwise
	*/
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
					append_format(LoadedList, "- %s", Info.c_str());
					++LoadedCount;
				}
				else
				{
					append_format(AvailableList, "- %s", Info.c_str());
					++AvailableCount;
				}
			}

			if (LoadedCount > 0U)
			{
				append_format(Feedback_out, "Loaded plugins (%u found):\n%s",
							  LoadedCount, LoadedList.c_str());
			}

			if (AvailableCount > 0U)
			{
				if (LoadedCount > 0U)
					Feedback_out += "\n";

				append_format(Feedback_out, "Available plugins (%u found):\n%s",
							  AvailableCount, AvailableList.c_str());
			}

			return true;
		}

		return false;
	}

	/*! \brief Retrieves the compatibility flags of the plugin
		\return the compatibility flags text
	*/
	const TCHAR* PluginEngine::GetCompatibilityFlagsText(DWORD Flags_in)
	{
		switch(Flags_in)
		{
			case PLUGIN_COMPATIBILITY_WINDOWER:
				return _T("Windower");
			case PLUGIN_COMPATIBILITY_BOOTSTRAP:
				return _T("Bootstrap");
			case PLUGIN_COMPATIBILITY_ANY:
				return _T("Any");
		}

		return _T("None");
	}

	/*! \brief Sets the working directory of windower
		\param[in] hModule_in : the handle of the DLL
	*/
	void PluginEngine::SetWorkingDir(HMODULE hModule_in)
	{
		TCHAR DirPath[_MAX_PATH] = { '\0' };
		TCHAR *pLastSlash = NULL;

		// retrieve the name of the module
		GetModuleFileName(hModule_in, DirPath, _MAX_PATH);
		// find the last backslash
		pLastSlash = _tcsrchr(DirPath, '\\') + 1;

		if (pLastSlash != NULL)
		{
			*pLastSlash = '\0';
			// set the working directory
			m_WorkingDir.assign(DirPath);
		}
	}
}