/**************************************************************************
	created		:	2011-07-03
	filename	: 	WindowerCore.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower core module base class
**************************************************************************/
#include "stdafx.h"

#include "WindowerCore.h"
#include "WindowerEngine.h"
#include "ModuleService.h"

namespace Windower
{
	HookEngine* WindowerCore::m_pHookManager = NULL;
	WindowerEngine* WindowerCore::m_pEngine = NULL;	

	/*! \brief WindowerCore constructor
		\param[in] ModuleName_in : the name of the module
		\param[in,out] Engine_in_out : the plugin engine
		\param[in,out] HookManager_in_out : the hook manager
	*/
	WindowerCore::WindowerCore(const string_t& ModuleName_in)
	{
		if (m_pEngine != NULL)
			m_pEngine->RegisterModule(ModuleName_in, this);
	}

	//! \brief WindowerCore destructor
	WindowerCore::~WindowerCore()
	{
		ModuleServices::const_iterator ServiceIt, EndIt = m_Services.cend();

		for (ServiceIt = m_Services.cbegin(); ServiceIt != EndIt; ++ServiceIt)
			delete ServiceIt->second;
	}

	/*! \brief Registers a service in the module
		\param[in] ServiceName_in : the name of the service
		\param[in] InvokePermission_in : flag specifying if the service can be invoked
		\return a pointer to the service if successful; NULL otherwise
	*/
	BaseModuleService* WindowerCore::RegisterService(const string_t& ServiceName_in, bool InvokePermission_in)
	{
		ModuleServices::const_iterator ServiceIt = m_Services.find(ServiceName_in);
		BaseModuleService *pBaseService = NULL;

		// the service doesn't exist
		if (ServiceIt == m_Services.cend())
		{
			// create a new service
			pBaseService = CreateService(ServiceName_in, InvokePermission_in);

			if (pBaseService != NULL)
			{
				if (pBaseService->CanSubscribe())
				{
					ModuleService *pService = static_cast<ModuleService*>(pBaseService);

					// register the hooks associated with the service
					RegisterHooks(pService);
				}
				// insert the service and its hooks
				m_Services[ServiceName_in] = pBaseService;
			}
		}
		else
			pBaseService = ServiceIt->second;

		return pBaseService;
	}

	/*! \brief (Un)installs a set of hooks given their names
		\param[in,out] pService_in_out : the service for which to update the hooks
		\param[in] Install_in : flag specifying if the hooks are to be installed or uninstalled
		\return true if all the hooks were (un)installed; false otherwise
	*/
	bool WindowerCore::UpdateServiceHooks(ModuleService *pService_in_out, bool Install_in) const
	{
		bool Result = false;

		if (pService_in_out != NULL && m_pHookManager != NULL)
		{
			const HookPointers &HookList = pService_in_out->GetHooks();
			HookPointers::const_iterator HookIt, EndIt = HookList.cend();			
			LPVOID pPointer = NULL;

			Result = true;

			for (HookIt = HookList.cbegin(); HookIt != EndIt; ++HookIt)
			{
				if (Install_in)
				{
					pPointer = m_pHookManager->InstallHook(HookIt->first.c_str());
					Result &= (pPointer != NULL);
				}
				else
				{
					// uninstall the hook
					Result &= m_pHookManager->UninstallHook(HookIt->first.c_str());
					pPointer = NULL;
				}
				// update the pointers (only create them when installing)
				pService_in_out->SetPointer(HookIt->first, pPointer, false);
			}
		}

		return Result;
	}

	/*! \brief Adds a plugin subscription to the specified service
		\param[in] ServiceName_in : the name of the service
		\param[in] pPlugin_in : the plugin subscribing to the service
		\return true if successful; false otherwise
	*/
	bool WindowerCore::Subscribe(const string_t& ServiceName_in, PluginFramework::IPlugin* pPlugin_in)
	{
		ModuleServices::const_iterator ServiceIt = m_Services.find(ServiceName_in);

		if (ServiceIt != m_Services.cend())
		{
			BaseModuleService *pBaseService = ServiceIt->second;

			if (pBaseService != NULL && pBaseService->CanSubscribe())
			{
				ModuleService *pService = static_cast<ModuleService*>(pBaseService);

				if (pService->IsSubscriberKeyValid(pPlugin_in))
				{
					bool bInstall = pService->GetSubscribers().empty();

					if (pService != NULL && pService->AddSubscriber(pPlugin_in))
					{
						// Hooks must be installed before creating the calling context!
						if (bInstall)
							UpdateServiceHooks(pService, true);
						// callback
						OnSubscribe(pService, pPlugin_in);

						return true;
					}
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
		ModuleServices::const_iterator ServiceIt = m_Services.find(ServiceName_in);

		if (ServiceIt != m_Services.cend())
		{
			BaseModuleService *pBaseService = ServiceIt->second;

			if (pBaseService != NULL && pBaseService->CanSubscribe())
			{
				ModuleService *pService = static_cast<ModuleService*>(pBaseService);

				if (pService->RemoveSubscriber(pPlugin_in))
				{
					// uninstall the hooks on the last subscriber
					if (pService->GetSubscribers().empty())
						UpdateServiceHooks(pService, false);
					// callback
					OnUnsubscribe(pService, pPlugin_in);

					return true;
				}
			}
		}

		return false;
	}



	/*! \brief Creates a service object given its name
		\param[in] ServiceName_in : the name of the service
		\param[in] InvokePermission_in : flag specifying if the service can be invoked
		\return a pointer to the service object if successful; NULL otherwise
	*/
	BaseModuleService* WindowerCore::CreateService(const string_t& ServiceName_in, bool InvokePermission_in)
	{
		return new BaseModuleService(ServiceName_in, InvokePermission_in);
	}
}
