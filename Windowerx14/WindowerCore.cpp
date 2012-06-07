/**************************************************************************
	created		:	2011-07-03
	filename	: 	WindowerCore.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower core module base class
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <HookEngine.h>

#include <CommandHandler.h>
#include "WindowerSettings.h"
#include "WindowerEngine.h"

#include "ICoreModule.h"
#include "WindowerCore.h"

namespace Windower
{
	//! \brief WindowerCore destructor
	WindowerCore::~WindowerCore()
	{
		ModuleServices::iterator ServiceIt;

		for (ServiceIt = m_Services.begin(); ServiceIt != m_Services.end(); ++ServiceIt)
			delete ServiceIt->second;
	}

	/*! \brief Registers a service in the module
		\param[in] ServiceName_in : the name of the service
		\param[in] InvokePermission_in : flag specifying if the service can be invoked
		\return a pointer to the newly created service if successful; NULL otherwise
	*/
	ModuleService* WindowerCore::RegisterService(const string_t &ServiceName_in, bool InvokePermission_in)
	{
		ModuleServices::const_iterator Iter = m_Services.find(ServiceName_in);
		ModuleService *pResult = NULL;

		// the service doesn't exist
		if (Iter == m_Services.end())
			m_Services[ServiceName_in] = pResult = new ModuleService(InvokePermission_in);

		return pResult;
	}

	/*! \brief Checks if the specified plugin is compatible with the module
		\param[in] pPlugin_in : the plugin to check for compatibility
		\return true if the plugin is compatible; false otherwise
	*/
	bool WindowerCore::IsPluginCompatible(PluginFramework::IPlugin* pPlugin_in)
	{
		return (pPlugin_in != NULL && m_CompatiblePlugins.find(pPlugin_in->GetUUID()) != m_CompatiblePlugins.end());
	}

	/*! \brief Revokes all the subscriptions of the specified plugin
		\param[in] pPlugin_in : the plugin revoking its subscriptions
	*/
	void WindowerCore::UnsubscribeAll(PluginFramework::IPlugin* pPlugin_in)
	{
		ModuleServices::const_iterator Iter;

		for (Iter = m_Services.begin(); Iter != m_Services.end(); ++Iter)
			Unsubscribe(Iter->first, pPlugin_in);
	}

	/*! \brief Adds a plugin subscription to the specified service
		\param[in] ServiceName_in : the name of the service
		\param[in] pPlugin_in : the plugin subscribing to the service
		\return true if successful; false otherwise
	*/
	bool WindowerCore::Subscribe(const string_t &ServiceName_in, PluginFramework::IPlugin* pPlugin_in)
	{
		if (IsPluginCompatible(pPlugin_in))
		{
			ModuleServices::iterator Iter = m_Services.find(ServiceName_in);

			if (Iter != m_Services.end())
			{
				PluginSet::const_iterator SubsIter = Iter->second->Subscribers.find(pPlugin_in);

				if (SubsIter == Iter->second->Subscribers.end())
				{
					Iter->second->Subscribers.insert(pPlugin_in);
					OnSubscribe(ServiceName_in, Iter->second->Subscribers);
				}

				return true;
			}
		}

		return false;
	}

	/*! \brief Removes a plugin subscription from the specified service
		\param[in] ServiceName_in : the name of the service
		\param[in] pPlugin_in : the plugin unsubscribing from the service
		\return true if successful; false otherwise
	*/
	bool WindowerCore::Unsubscribe(const string_t &ServiceName_in, PluginFramework::IPlugin* pPlugin_in)
	{
		ModuleServices::iterator Iter = m_Services.find(ServiceName_in);

		if (Iter != m_Services.end())
		{
			PluginSet::const_iterator SubsIter = Iter->second->Subscribers.find(pPlugin_in);

			if (SubsIter != Iter->second->Subscribers.end())
			{
				Iter->second->Subscribers.erase(SubsIter);
				OnUnsubscribe(ServiceName_in, Iter->second->Subscribers);
			}

			return true;
		}

		return false;
	}
}
