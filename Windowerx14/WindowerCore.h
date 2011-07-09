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
	class WindowerCore : public ICoreModule
	{
	public:
		WindowerCore(PluginEngine &Engine_in) : m_Engine(Engine_in) {}

		virtual void UnsubscribeAll(PluginFramework::IPlugin* pPlugin_in);
		virtual bool IsPluginCompatible(PluginFramework::IPlugin* pPlugin_in);
		virtual bool Subscribe(const string_t &ServiceName_in, PluginFramework::IPlugin* pPlugin_in);
		virtual bool Unsubscribe(const string_t &ServiceName_in, PluginFramework::IPlugin* pPlugin_in);
		virtual bool Invoke(const string_t &ServiceName_in, const PluginFramework::ServiceParam &Params_in,
							PluginFramework::ServiceParam &Results_out) { return true; }

	protected:
		virtual void OnSubscribe(const string_t &ServiceName_in,
								 const PluginSet &Subscribers_in) {};
		virtual void OnUnsubscribe(const string_t &ServiceName_in,
								   const PluginSet &Subscribers_in) {};
		void RegisterService(const string_t &ServiceName_in, bool InvokePermission_in);

		PluginEngine		&m_Engine;
		CompatiblePlugins	 m_CompatiblePlugins;
		ModuleServices		 m_Services;
	};
}

#endif//__WINDOWER_CORE_H__