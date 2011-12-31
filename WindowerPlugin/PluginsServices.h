/**************************************************************************
	created		:	2011-07-03
	filename	: 	PluginServices.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin services used to (un)subscribe to services and invoke them
**************************************************************************/
#ifndef __PLUGIN_SERVICES_H__
#define __PLUGIN_SERVICES_H__

namespace Windower
{
	class ICoreModule;

	//! a hash map of core modules
	typedef stdext::hash_map<string_t, ICoreModule*> CoreModules;

	//! \brief Plugin services used to (un)subscribe to services and invoke them
	class PluginServices : public PluginFramework::IPluginServices
	{
	public:
		/*! \brief PluginServices constructor
			\param[in] pVersion_in : the version of the plugin services
			\param[in] Modules_in : hash map of modules available to the plugins
		*/
		PluginServices(const TCHAR *pVersion_in, CoreModules &Modules_in)
			: IPluginServices(pVersion_in), m_Modules(Modules_in) {}

		virtual bool InvokeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
								   const PluginFramework::ServiceParam &Params_in) const;
		virtual bool SubscribeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
								      PluginFramework::IPlugin* pPlugin_in) const;
		virtual bool UnsubscribeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
										PluginFramework::IPlugin* pPlugin_in) const;

	protected:
		//! hash map of modules available to the plugins
		const CoreModules &m_Modules;
	};
}

#endif//__PLUGIN_SERVICES_H__