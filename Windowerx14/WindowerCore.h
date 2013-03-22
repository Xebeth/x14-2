/**************************************************************************
	created		:	2011-07-03
	filename	: 	WindowerCore.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower core module base class
**************************************************************************/
#ifndef __WINDOWER_CORE_H__
#define __WINDOWER_CORE_H__

namespace Windower
{
	//! a hash map of module services
	typedef std::hash_map<string_t, ModuleService*> ModuleServices;
	//! the hooks of a service
	typedef stdext::hash_map<std::string, LPVOID> HookPointers;

	class PluginEngine;
		
	//! \brief Windower core module base class
	class WindowerCore : public ICoreModule
	{
	public:
		//! \brief WindowerCore constructor
		WindowerCore(PluginEngine &Engine_in_out, HookEngine &HookManager_in_out)
			: m_Engine(Engine_in_out), m_HookManager(HookManager_in_out) {}
		virtual ~WindowerCore();

		virtual void UnsubscribeAll(PluginFramework::IPlugin* pPlugin_in);
		virtual bool Subscribe(const string_t &ServiceName_in, PluginFramework::IPlugin* pPlugin_in);
		virtual bool Unsubscribe(const string_t &ServiceName_in, PluginFramework::IPlugin* pPlugin_in);
		virtual ModuleService* CreateService(const string_t &ServiceName_in, const HookPointers &Hooks_in,
											 bool InvokePermission_in = false);

		/*! \brief Register the hooks for this module
			\param[in] HookManager_in : the hook manager
		*/
		virtual void RegisterHooks(IHookManager &HookManager_in) {};
		/*! \brief Callback invoked when the hooks of the module are installed
			\param[in] HookManager_in : the hook manager
		*/
		virtual void OnHookInstall(IHookManager &HookManager_in) {};

		/*! \brief Register the hooks of the specified service with the hook manager
			\param[in] pService_in : the service for which to register hooks
			\param[out] Hooks_out : the list of hooks to register
			\return true if the hooks were registered; false otherwise
		*/
		virtual bool RegisterHooks(const string_t &ServiceName_in, HookPointers &Hooks_out) { return true; }
		/*! \brief Invokes a command registered with the specified service
			\param[in] ServiceName_in : the name of the service
			\param[in] Params_in : the input parameters
			\return true if the command was invoked successfully; false otherwise
		*/
		virtual bool Invoke(const string_t &ServiceName_in, const PluginFramework::ServiceParam &Params_in) { return false; }

	protected:
		bool RegisterService(const string_t &ServiceName_in, bool InvokePermission_in);
		/*! \brief Callback function invoked when a plugin subscribes to a service
			\param[in] pService_in_out : the service to which to subscribe
			\param[in] Subscribers_in : the subscribing plugin
		*/
		virtual void OnSubscribe(ModuleService *pService_in_out, PluginFramework::IPlugin* pPlugin_in) {}
		/*! \brief Callback function invoked when a plugin unsubscribes to a service
			\param[in] pService_in_out : the service from which to unsubscribe
			\param[in] Subscribers_in : the unsubscribing plugin
		*/
		virtual void OnUnsubscribe(ModuleService *pService_in_out, PluginFramework::IPlugin* pPlugin_in) {}

		//! a reference to the windower engine
		PluginEngine		&m_Engine;
		//! the module services
		ModuleServices		 m_Services;
		//! a reference to the hook manager
		HookEngine			&m_HookManager;
	};
}

#endif//__WINDOWER_CORE_H__