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
	typedef hash_map<string_t, BaseModuleService*> ModuleServices;
	//! the hooks of a service
	typedef hash_map<std::string, LPVOID> HookPointers;
	typedef std::set<UUID> Blacklist;

	class WindowerEngine;
		
	//! \brief Windower core module base class
	class WindowerCore : public ICoreModule, public NonCopyable
	{
	public:
		//! \brief WindowerCore constructor
		WindowerCore(const string_t& ModuleName_in);
		virtual ~WindowerCore();

		virtual bool Subscribe(const string_t& ServiceName_in, PluginFramework::IPlugin* pPlugin_in);
		virtual bool Unsubscribe(const string_t& ServiceName_in, PluginFramework::IPlugin* pPlugin_in);
		virtual BaseModuleService* CreateService(const string_t& ServiceName_in, bool InvokePermission_in = false);

		static void Initialize(WindowerEngine *pEngine_in, HookEngine *pHookManager_in)
		{ m_pEngine = pEngine_in; m_pHookManager = pHookManager_in; }

		/*! \brief Register the hooks for this module
			\param[in] HookManager_in : the hook manager
		*/
		virtual void RegisterHooks(HookEngineLib::IHookManager &HookManager_in) {};
		/*! \brief Callback invoked when the hooks of the module are installed
			\param[in] HookManager_in : the hook manager
		*/
		virtual void OnHookInstall(HookEngineLib::IHookManager &HookManager_in) {};
		
		/*! \brief Registers the services of the module
			\return true if the services were registered; false otherwise
		*/
		virtual bool RegisterServices() { return true; }
		/*! \brief Register the hooks of the specified service with the hook manager
			\param[in] pService_in : the service for which to register hooks
			\return true if the hooks were registered; false otherwise
		*/
		virtual bool RegisterHooks(ModuleService *pService_in_out) { return true; }
		/*! \brief Invokes a command registered with the specified service
			\param[in] ServiceName_in : the name of the service
			\param[in] Params_in : the input parameters
			\return true if the command was invoked successfully; false otherwise
		*/
		virtual bool Invoke(const string_t& ServiceName_in, PluginFramework::ServiceParam &Params_in) { return false; }

	protected:
		template <typename T> class CallingContext
		{
		public:
			CallingContext() : m_pThis(NULL) {}
			
			bool IsSet() const { return (m_pThis != NULL); }
			void Set(T *pThis_in) { m_pThis = pThis_in; }
			T* operator->() const { return m_pThis; }
			operator T*() { return m_pThis; }

		private:
			T *m_pThis;
		};

		bool UpdateServiceHooks(ModuleService *pService_in_out, bool Install_in) const;

		BaseModuleService* RegisterService(const string_t& ServiceName_in, bool InvokePermission_in);
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
		static WindowerEngine *m_pEngine;
		//! a reference to the hook manager
		static HookEngine *m_pHookManager;
		//! the module services
		ModuleServices m_Services;
	};
}

#endif//__WINDOWER_CORE_H__