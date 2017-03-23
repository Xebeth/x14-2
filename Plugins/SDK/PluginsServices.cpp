/**************************************************************************
	created		:	2011-07-03
	filename	: 	PluginsServices.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin services used to (un)subscribe to services and invoke them
**************************************************************************/
#include "stdafx.h"

#include "ICoreModule.h"
#include "PluginsServices.h"
#include "WindowerSettings.h"
#include "WindowerSettingsManager.h"

namespace Windower
{
	/*! \brief PluginServices constructor
		\param[in] Version_in : the version of the plugin services
		\param[in] Modules_in : hash map of modules available to the plugins
		\param[in] ConfigFile_in : the absolute path of the configuration file
	*/
	PluginServices::PluginServices(const PluginFramework::VersionInfo &Version_in,
								   const CoreModules &Modules_in,
								   SettingsManager *pManager_in)
		: IPluginServices(Version_in), m_Modules(Modules_in),
		  m_pSettingsManager(pManager_in) {}

	/*! \brief Adds a plugin subscription to the service in the specified module
		\param[in] ModuleName_in : the name of the module
		\param[in] ServiceName_in : the name of the service
		\param[in] pPlugin_in : the plugin subscribing to the service
		\return true if successful; false otherwise
	*/
	bool PluginServices::SubscribeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
										  PluginFramework::IPlugin* pPlugin_in) const
	{
		if (pPlugin_in != nullptr)
		{
			CoreModules::const_iterator ModuleIt = m_Modules.find(ModuleName_in);

			if (ModuleIt != m_Modules.cend() && ModuleIt->second != nullptr)
				return ModuleIt->second->Subscribe(ServiceName_in, pPlugin_in);
		}

		return false;
	}

	/*! \brief Removes a plugin subscription from the service in the specified module
		\param[in] ModuleName_in : the name of the module
		\param[in] ServiceName_in : the name of the service
		\param[in] pPlugin_in : the plugin unsubscribing from the service
		\return true if successful; false otherwise
	*/
	bool PluginServices::UnsubscribeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
											PluginFramework::IPlugin* pPlugin_in) const
	{
		if (pPlugin_in != nullptr)
		{
			CoreModules::const_iterator ModuleIt = m_Modules.find(ModuleName_in);

			if (ModuleIt != m_Modules.cend() && ModuleIt->second != nullptr)
				return ModuleIt->second->Unsubscribe(ServiceName_in, pPlugin_in);
		}

		return false;
	}

	/*! \brief Invokes a command registered with the service in the specified module
		\param[in] ModuleName_in : the name of the module
		\param[in] ServiceName_in : the name of the service
		\param[in] Params_in : the input parameters
		\return true if the command was invoked successfully; false otherwise
	*/
	bool PluginServices::InvokeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
									   PluginFramework::ServiceParam &Params_in) const
	{
		CoreModules::const_iterator ModuleIt = m_Modules.find(ModuleName_in);

		if (ModuleIt != m_Modules.cend() && ModuleIt->second != nullptr)
			return ModuleIt->second->Invoke(ServiceName_in, Params_in);

		return false;
	}

	bool PluginServices::LoadSettings(PluginFramework::IUserSettings* pSettings_out) const
	{
		if (m_pSettingsManager != nullptr)
			return m_pSettingsManager->LoadSettings(static_cast<WindowerProfile*>(pSettings_out));
		
		return false;
	}

	bool PluginServices::SaveSettings(const PluginFramework::IUserSettings *pSettings_in)
	{
		if (m_pSettingsManager != nullptr)
			return m_pSettingsManager->CopySettings(static_cast<const WindowerProfile*>(pSettings_in));

		return false;
	}

	const TCHAR* PluginServices::GetWorkingDir() const
	{
		if (m_pSettingsManager != nullptr)
			return m_pSettingsManager->GetWorkingDir().c_str();

		return nullptr;
	}
}