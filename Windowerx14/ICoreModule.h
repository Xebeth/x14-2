/**************************************************************************
	created		:	2010-10-23
	filename	: 	ICoreModule.h
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Base class for the windower modules
**************************************************************************/
#ifndef __WINDOWER_MODULE_H__
#define __WINDOWER_MODULE_H__

#include <set>

namespace Windower
{
	typedef std::set<PluginFramework::PluginUUID> CompatiblePlugins;
	typedef std::set<PluginFramework::IPlugin*> PluginSet;

	class ModuleService
	{
	public:
		ModuleService() : InvokePermission(false) {}
		ModuleService(bool InvokePermission_in, const PluginSet &Subscribers_in)
		{
			InvokePermission = InvokePermission_in;
			Subscribers = Subscribers_in;
		}

		bool InvokePermission;
		PluginSet Subscribers;
	};

	typedef std::hash_map<string_t, ModuleService> ModuleServices;
	typedef PluginFramework::PluginVersion PluginVersion;
	typedef HookEngineLib::IHookManager IHookManager;

	class ICoreModule
	{
	public:
		ICoreModule() {}

		virtual void RegisterHooks(IHookManager *pHookManager) =0;
		virtual void OnHookInstall(IHookManager *pHookManager) =0;

		virtual void UnsubscribeAll(PluginFramework::IPlugin* pPlugin_in) =0;
		virtual bool IsPluginCompatible(PluginFramework::IPlugin* pPlugin_in) =0;
		virtual bool Subscribe(const string_t &ServiceName_in, PluginFramework::IPlugin* pPlugin_in) =0;
		virtual bool Unsubscribe(const string_t &ServiceName_in, PluginFramework::IPlugin* pPlugin_in) =0;
		virtual bool Invoke(const string_t &ServiceName_in, const PluginFramework::ServiceParam &Params_in,
							PluginFramework::ServiceParam &Results_out) =0;

		static void SetPluginManager(PluginFramework::PluginManager *pManager_in) { m_pPluginManager = pManager_in; }

	protected:
		virtual void OnSubscribe(const string_t &ServiceName_in,
								 const PluginSet &Subscribers_in) =0;
		virtual void OnUnsubscribe(const string_t &ServiceName_in,
								   const PluginSet &Subscribers_in) =0;

		virtual void RegisterService(const string_t &ServiceName_in, bool InvokePermission_in) =0;
		static PluginFramework::PluginManager	*m_pPluginManager;
	};
}

#endif//__WINDOWER_MODULE_H__