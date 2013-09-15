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
		\param[in] InvokePermission_in : flag specifying if the service can be invoked
	*/
	ModuleService::ModuleService(const string_t& Name_in, bool InvokePermission_in)
		: BaseModuleService(Name_in, InvokePermission_in) {}

	/*! \brief Adds a plugin to the subscribers of the service
		\param[in] pPlugin_in : the plugin to add to the set
		\return true if the plugin was added; false if it had already subscribed
	*/
	bool ModuleService::AddSubscriber(PluginFramework::IPlugin *pPlugin_in)
	{
		PluginFramework::PluginSet::const_iterator PluginIt = m_Subscribers.find(pPlugin_in);

		if (PluginIt == m_Subscribers.cend())
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

		if (PluginIt != m_Subscribers.cend())
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
	bool ModuleService::IsSubscriberKeyValid(PluginFramework::IPlugin* pPlugin_in)
	{
		return (pPlugin_in != NULL && m_CompatiblePlugins.find(pPlugin_in->GetUUID()) != m_CompatiblePlugins.cend());
	}

	/*! \brief Adds a hook pointer to the service
		\param[in] HookName_in : the name of the hook
		\param[in] pPointer_in : the pointer for the hook
		\param[in] Create_in : flag specifying if the pointer is to be created if it doesn't exist
		\return true if the pointer was set; false otherwise
	*/
	bool ModuleService::SetPointer(const std::string &HookName_in, LPVOID pPointer_in, bool Create_in)
	{
		if (Create_in || m_ServiceHooks.find(HookName_in) != m_ServiceHooks.cend())
		{
			m_ServiceHooks[HookName_in] = pPointer_in;
			OnPointerChange(HookName_in, pPointer_in);

			return true;
		}

		return false;
	}
}