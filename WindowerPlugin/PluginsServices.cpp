/**************************************************************************
	created		:	2011-07-03
	filename	: 	PluginsServices.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin services used to (un)subscribe to services and invoke them
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <PluginManager.h>
#include <HookEngine.h>
#include <HookManager.h>

#include "PluginsServices.h"
#include "ICoreModule.h"

namespace Windower
{
	/*! \brief Adds a plugin subscription to the service in the specified module
		\param[in] ModuleName_in : the name of the module
		\param[in] ServiceName_in : the name of the service
		\param[in] pPlugin_in : the plugin subscribing to the service
		\return true if successful; false otherwise
	*/
	bool PluginServices::SubscribeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
										  PluginFramework::IPlugin* pPlugin_in) const
	{
		if (pPlugin_in != NULL)
		{
			CoreModules::const_iterator Iter = m_Modules.find(ModuleName_in);

			if (Iter != m_Modules.end() && Iter->second != NULL)
				return Iter->second->Subscribe(ServiceName_in, pPlugin_in);
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
		if (pPlugin_in != NULL)
		{
			CoreModules::const_iterator Iter = m_Modules.find(ModuleName_in);

			if (Iter != m_Modules.end() && Iter->second != NULL)
				return Iter->second->Unsubscribe(ServiceName_in, pPlugin_in);
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
									   const PluginFramework::ServiceParam &Params_in) const
	{
		CoreModules::const_iterator Iter = m_Modules.find(ModuleName_in);

		if (Iter != m_Modules.end() && Iter->second != NULL)
			return Iter->second->Invoke(ServiceName_in, Params_in);

		return false;
	}
}