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
	//! \brief Windower core module base class
	class WindowerCore : public ICoreModule
	{
	public:
		//! \brief WindowerCore constructor
		explicit WindowerCore(PluginEngine &Engine_in_out) : m_Engine(Engine_in_out) {}
		virtual ~WindowerCore();

		virtual void UnsubscribeAll(PluginFramework::IPlugin* pPlugin_in);
		virtual bool IsPluginCompatible(PluginFramework::IPlugin* pPlugin_in);
		virtual bool Subscribe(const string_t &ServiceName_in, PluginFramework::IPlugin* pPlugin_in);
		virtual bool Unsubscribe(const string_t &ServiceName_in, PluginFramework::IPlugin* pPlugin_in);
		/*! \brief Invokes a command registered with the specified service
			\param[in] ServiceName_in : the name of the service
			\param[in] Params_in : the input parameters
			\param[out] Results_out : the results from the invocation
			\return true if the command was invoked successfully; false otherwise
		*/
		virtual bool Invoke(const string_t &ServiceName_in, const PluginFramework::ServiceParam &Params_in,
							PluginFramework::ServiceParam &Results_out) { return false; }

	protected:
		virtual void OnSubscribe(const string_t &ServiceName_in,
								 const PluginSet &Subscribers_in) {};
		virtual void OnUnsubscribe(const string_t &ServiceName_in,
								   const PluginSet &Subscribers_in) {};
		ModuleService* RegisterService(const string_t &ServiceName_in, bool InvokePermission_in);

		//! a reference to the windower engine
		PluginEngine		&m_Engine;
		//! a set of compatible plugins
		CompatiblePlugins	 m_CompatiblePlugins;
		//! the module services
		ModuleServices		 m_Services;
	};
}

#endif//__WINDOWER_CORE_H__