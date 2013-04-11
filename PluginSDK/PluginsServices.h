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
		PluginServices(const PluginFramework::VersionInfo &Version_in, const CoreModules &Modules_in, const string_t &ConfigFile_in);

		virtual bool InvokeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
								   const PluginFramework::ServiceParam &Params_in) const;
		virtual bool SubscribeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
								      PluginFramework::IPlugin* pPlugin_in) const;
		virtual bool UnsubscribeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
										PluginFramework::IPlugin* pPlugin_in) const;
		/*! \brief Retrieves the configuration file path
			\return the configuration file path
		*/
		virtual const TCHAR* GetConfigFile() const { return m_ConfigFile.c_str(); }

	protected:
		//! hash map of modules available to the plugins
		const CoreModules &m_Modules;
		//! Full path to the configuration file
		string_t m_ConfigFile;
	};
}

#endif//__PLUGIN_SERVICES_H__