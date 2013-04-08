/**************************************************************************
	created		:	2013-03-12
	filename	: 	ModuleService.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Module service class
**************************************************************************/
#include "stdafx.h"

#include "ModuleService.h"

namespace Windower
{
	/*! \brief ModuleService constructor
		\param[in] Name_in : the name of the service
		\param[in] Hooks_in : the hooks associated with the service
		\param[in] InvokePermission_in : flag specifying if the service can be invoked
	*/
	ModuleService::ModuleService(const string_t& Name_in, const HookPointers &Hooks_in, bool InvokePermission_in)
		: m_InvokePermission(InvokePermission_in), m_ServiceName(Name_in), m_ServiceHooks(Hooks_in) {}

	/*! \brief Adds a plugin to the subscribers of the service
		\param[in] pPlugin_in : the plugin to add to the set
		\return true if the plugin was added; false if it had already subscribed
	*/
	bool ModuleService::AddSubscriber(PluginFramework::IPlugin *pPlugin_in)
	{
		PluginFramework::PluginSet::const_iterator PluginIt = m_Subscribers.find(pPlugin_in);

		if (PluginIt == m_Subscribers.end())
		{
			m_Subscribers.insert(pPlugin_in);

			return true;
		}
		
		return false;
	}

	/*! \brief Removes a plugin to the subscribers of the service
		\param[in] pPlugin_in : the plugin to remove from the set
		\return true if the plugin was removed; false if not found
	*/
	bool ModuleService::RemoveSubscriber(PluginFramework::IPlugin *pPlugin_in)
	{
		PluginFramework::PluginSet::const_iterator PluginIt = m_Subscribers.find(pPlugin_in);

		if (PluginIt != m_Subscribers.end())
		{
			m_Subscribers.erase(PluginIt);

			return true;
		}

		return false;
	}

	/*! \brief Checks if the specified plugin is compatible with the service
		\param[in] pPlugin_in : the plugin to check for compatibility
		\return true if the plugin is compatible; false otherwise
	*/
	bool ModuleService::IsPluginCompatible(PluginFramework::IPlugin* pPlugin_in)
	{
		return (pPlugin_in != NULL && m_CompatiblePlugins.find(pPlugin_in->GetUUID()) != m_CompatiblePlugins.end());
	}
}