/**************************************************************************
	created		:	2011-07-03
	filename	: 	WindowerCore.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower core module base class
**************************************************************************/
#include "stdafx.h"

#include "WindowerCore.h"
#include "WindowerSettings.h"
#include "WindowerEngine.h"

namespace Windower
{
	/*! \brief WindowerCore constructor
		\param[in] ModuleName_in : the name of the module
		\param[in,out] Engine_in_out : the plugin engine
		\param[in,out] HookManager_in_out : the hook manager
	*/
	WindowerCore::WindowerCore(const string_t& ModuleName_in, WindowerEngine &Engine_in_out, HookEngine &HookManager_in_out)
	  : m_Engine(Engine_in_out), m_HookManager(HookManager_in_out)
	{
		m_Engine.RegisterModule(ModuleName_in, this);
	}

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
		\return a pointer to the service if successful; NULL otherwise
	*/
	ModuleService* WindowerCore::RegisterService(const string_t& ServiceName_in, bool InvokePermission_in)
	{
		ModuleServices::const_iterator Iter = m_Services.find(ServiceName_in);
		ModuleService *pService = NULL;

		// the service doesn't exist
		if (Iter == m_Services.end())
		{
			HookPointers Hooks;

			// register the hooks associated with the service
			if (RegisterHooks(ServiceName_in, Hooks))
			{
				// create a new service
				pService = CreateService(ServiceName_in, Hooks, InvokePermission_in);
				// insert the service and its hooks
				m_Services[ServiceName_in] = pService;
			}
		}
		else
			pService = Iter->second;

		return pService;
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
	bool WindowerCore::Subscribe(const string_t& ServiceName_in, PluginFramework::IPlugin* pPlugin_in)
	{
		ModuleServices::iterator Iter = m_Services.find(ServiceName_in);

		if (Iter != m_Services.end())
		{
			ModuleService *pService = Iter->second;

			if (pService != NULL && pService->IsPluginCompatible(pPlugin_in))
			{
				bool bInstall = pService->GetSubscribers().empty();

				if (pService != NULL && pService->AddSubscriber(pPlugin_in))
				{
					// Hooks must be installed before creating the calling context!
					if (bInstall && m_HookManager.InstallHooks(pService->GetHooks()))
					{
						// Create the calling context for the hooks
						pService->CreateContext();
					}
					// callback
					OnSubscribe(pService, pPlugin_in);

					return true;
				}
			}
		}

		return false;
	}

	/*! \brief Removes a plugin subscription from the specified service
		\param[in] ServiceName_in : the name of the service
		\param[in] pPlugin_in : the plugin unsubscribing from the service
		\return true if successful; false otherwise
	*/
	bool WindowerCore::Unsubscribe(const string_t& ServiceName_in, PluginFramework::IPlugin* pPlugin_in)
	{
		ModuleServices::iterator Iter = m_Services.find(ServiceName_in);

		if (Iter != m_Services.end())
		{
			ModuleService *pService = Iter->second;

			if (pService != NULL && pService->RemoveSubscriber(pPlugin_in))
			{
				// uninstall the hooks on the last subscriber
				if (pService->GetSubscribers().empty() && m_HookManager.UninstallHooks(pService->GetHooks()))
				{
					pService->DestroyContext();
				}
				// callback
				OnUnsubscribe(pService, pPlugin_in);

				return true;
			}
		}

		return false;
	}

	/*! \brief Creates a service object given its name
		\param[in] ServiceName_in : the name of the service
		\param[in] Hooks_in : the hooks associated with the service
		\param[in] InvokePermission_in : flag specifying if the service can be invoked
		\return a pointer to the service object if successful; NULL otherwise
	*/
	ModuleService* WindowerCore::CreateService(const string_t& ServiceName_in, const HookPointers &Hooks_in, bool InvokePermission_in)
	{
		return new ModuleService(ServiceName_in, Hooks_in, InvokePermission_in);
	}
}
