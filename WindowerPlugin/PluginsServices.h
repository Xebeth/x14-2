/**************************************************************************
	created		:	2011-07-03
	filename	: 	PluginServices.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin services used to (un)subscribe to services and invoke them
**************************************************************************/
#ifndef __PLUGIN_SERVICES_H__
#define __PLUGIN_SERVICES_H__

namespace PluginFramework
{
	class VersionInfo;
}

namespace Windower
{
	class ICoreModule;

	//! a hash map of core modules
	typedef stdext::hash_map<string_t, ICoreModule*> CoreModules;

	//! \brief Plugin services used to (un)subscribe to services and invoke them
	class PluginServices : public PluginFramework::IPluginServices
	{
	public:
		PluginServices(const PluginFramework::VersionInfo &Version_in, CoreModules &Modules_in);

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