/**************************************************************************
	created		:	2010-10-23
	filename	: 	ICoreModule.h
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Base class for the windower modules
**************************************************************************/
#ifndef __WINDOWER_MODULE_H__
#define __WINDOWER_MODULE_H__

namespace Windower
{
	//! a set of UUID
	typedef std::set<PluginFramework::PluginUUID> CompatiblePlugins;
	//! a set of plugins
	typedef std::set<PluginFramework::IPlugin*> PluginSet;

	//! Module service
	class ModuleService
	{
	public:
		ModuleService(bool InvokePermission_in = false)
			: InvokePermission(InvokePermission_in) {}

		/*! \brief Checks if the service can be invoked
			\return true if the service can be invoked; false otherwise
		*/
		bool CanInvoke() const { return InvokePermission; }

		//! flag specifying if the service can be invoked
		bool InvokePermission;
		//! the service subscribers
		PluginSet Subscribers;
	};

	//! a hash map of module services
	typedef std::hash_map<string_t, ModuleService*> ModuleServices;
	typedef PluginFramework::VersionInfo VersionInfo;
	typedef HookEngineLib::IHookManager IHookManager;

	//! \brief Base class for the windower modules
	class ICoreModule
	{
	public:
		//! ICoreModule destructor
		virtual ~ICoreModule() {}

		/*! \brief Register the hooks for this module
			\param[in] HookManager_in : the hook manager
		*/
		virtual void RegisterHooks(IHookManager &HookManager_in) =0;
		/*! \brief Callback invoked when the hooks of the module are installed
			\param[in] HookManager_in : the hook manager
		*/
		virtual void OnHookInstall(IHookManager &HookManager_in) =0;
		/*! \brief Revokes all the subscriptions of the specified plugin
			\param[in] pPlugin_in : the plugin revoking its subscriptions
		*/
		virtual void UnsubscribeAll(PluginFramework::IPlugin* pPlugin_in) =0;
		/*! \brief Checks if the specified plugin is compatible with the module
			\param[in] pPlugin_in : the plugin to check for compatibility
			\return true if the plugin is compatible; false otherwise
		*/
		virtual bool IsPluginCompatible(PluginFramework::IPlugin* pPlugin_in) =0;
		/*! \brief Adds a plugin subscription to the specified service
			\param[in] ServiceName_in : the name of the service
			\param[in] pPlugin_in : the plugin subscribing to the service
			\return true if successful; false otherwise
		*/
		virtual bool Subscribe(const string_t &ServiceName_in, PluginFramework::IPlugin* pPlugin_in) =0;
		/*! \brief Removes a plugin subscription from the specified service
			\param[in] ServiceName_in : the name of the service
			\param[in] pPlugin_in : the plugin unsubscribing from the service
			\return true if successful; false otherwise
		*/
		virtual bool Unsubscribe(const string_t &ServiceName_in, PluginFramework::IPlugin* pPlugin_in) =0;
		/*! \brief Invokes a command registered with the specified module
			\param[in] ServiceName_in : the name of the service
			\param[in] Params_in : the input parameters
			\return true if the command was invoked successfully; false otherwise
		*/
		virtual bool Invoke(const string_t &ServiceName_in, const PluginFramework::ServiceParam &Params_in) =0;

		/*! \brief Sets the pointer to the plugin manager
			\param[in,out] Manager_in_out : the plugin manager
		*/
		static void SetPluginManager(PluginFramework::PluginManager &Manager_in_out) { m_pPluginManager = &Manager_in_out; }

	protected:
		/*! \brief Callback function invoked when a plugin subscribes to a service
			\param[in] ServiceName_in : the name of the service
			\param[in] Subscribers_in : the service subscribers
		*/
		virtual void OnSubscribe(const string_t &ServiceName_in,
								 const PluginSet &Subscribers_in) =0;
		/*! \brief Callback function invoked when a plugin subscription to a service is revoked
			\param[in] ServiceName_in : the name of the service
			\param[in] Subscribers_in : the service subscribers
		*/
		virtual void OnUnsubscribe(const string_t &ServiceName_in,
								   const PluginSet &Subscribers_in) =0;
		/*! \brief Registers a service in the module
			\param[in] ServiceName_in : the name of the service
			\param[in] InvokePermission_in : flag specifying if the service can be invoked
			\return a pointer to the newly created service if successful; NULL otherwise
		*/
		virtual ModuleService* RegisterService(const string_t &ServiceName_in, bool InvokePermission_in) =0;
		//! the plugin manager
		static PluginFramework::PluginManager *m_pPluginManager;
	};
}

#endif//__WINDOWER_MODULE_H__