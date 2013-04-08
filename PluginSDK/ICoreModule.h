/**************************************************************************
	created		:	2010-10-23
	filename	: 	ICoreModule.h
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Base class for the windower modules
**************************************************************************/
#ifndef __WINDOWER_MODULE_H__
#define __WINDOWER_MODULE_H__

#ifndef _WINDOWER_PLUGIN_SDK_INCLUDE_
	#error Please include the global header file 'PluginSDK.h'
#endif

namespace HookEngineLib
{
	class IHookManager;
}

namespace Windower
{
	class ModuleService;

	//! \brief Base class for the windower modules
	class ICoreModule
	{
	public:
		//! ICoreModule destructor
		virtual ~ICoreModule() {}

		/*! \brief Register the hooks for this module
			\param[in] HookManager_in : the hook manager
		*/
		virtual void RegisterHooks(HookEngineLib::IHookManager &HookManager_in) =0;
		/*! \brief Registers the services of the module
			\return true if the services were registered; false otherwise
		*/
		virtual bool RegisterServices() =0;
		/*! \brief Callback invoked when the hooks of the module are installed
			\param[in] HookManager_in : the hook manager
		*/
		virtual void OnHookInstall(HookEngineLib::IHookManager &HookManager_in) =0;
		/*! \brief Revokes all the subscriptions of the specified plugin
			\param[in] pPlugin_in : the plugin revoking its subscriptions
		*/
		virtual void UnsubscribeAll(PluginFramework::IPlugin* pPlugin_in) =0;
		/*! \brief Adds a plugin subscription to the specified service
			\param[in] ServiceName_in : the name of the service
			\param[in] pPlugin_in : the plugin subscribing to the service
			\return true if successful; false otherwise
		*/
		virtual bool Subscribe(const string_t& ServiceName_in, PluginFramework::IPlugin* pPlugin_in) =0;
		/*! \brief Removes a plugin subscription from the specified service
			\param[in] ServiceName_in : the name of the service
			\param[in] pPlugin_in : the plugin unsubscribing from the service
			\return true if successful; false otherwise
		*/
		virtual bool Unsubscribe(const string_t& ServiceName_in, PluginFramework::IPlugin* pPlugin_in) =0;
		/*! \brief Invokes a command registered with the specified module
			\param[in] ServiceName_in : the name of the service
			\param[in] Params_in : the input parameters
			\return true if the command was invoked successfully; false otherwise
		*/
		virtual bool Invoke(const string_t& ServiceName_in, const PluginFramework::ServiceParam &Params_in) =0;

		/*! \brief Sets the pointer to the plugin manager
			\param[in,out] Manager_in_out : the plugin manager
		*/
		static void SetPluginManager(PluginFramework::PluginManager &Manager_in_out) { m_pPluginManager = &Manager_in_out; }

	protected:
		/*! \brief Callback function invoked when a plugin subscribes to a service
			\param[in] pService_in_out : the service to which to subscribe
			\param[in] Subscribers_in : the subscribing plugin
		*/
		virtual void OnSubscribe(ModuleService *pService_in_out, PluginFramework::IPlugin* pPlugin_in) =0;
		/*! \brief Callback function invoked when a plugin unsubscribes to a service
			\param[in] pService_in_out : the service from which to unsubscribe
			\param[in] Subscribers_in : the unsubscribing plugin
		*/
		virtual void OnUnsubscribe(ModuleService *pService_in_out, PluginFramework::IPlugin* pPlugin_in) =0;
		/*! \brief Registers a service in the module
			\param[in] ServiceName_in : the name of the service
			\param[in] InvokePermission_in : flag specifying if the service can be invoked
			\return true if the service is registered; false otherwise
		*/
		virtual ModuleService* RegisterService(const string_t& ServiceName_in, bool InvokePermission_in) =0;
		//! the plugin manager
		static PluginFramework::PluginManager *m_pPluginManager;
	};
}

#endif//__WINDOWER_MODULE_H__