/**************************************************************************
	created		:	2011-07-03
	filename	: 	PluginEngine.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin engine
**************************************************************************/
#ifndef __PLUGIN_ENGINE_H__
#define __PLUGIN_ENGINE_H__

namespace PluginFramework
{
	class PluginManager;
}

#include "BaseEngine.h"
#include "PluginsServices.h"

namespace Windower
{
	class PluginServices;
	class ICoreModule;

	typedef stdext::hash_map<string_t, PluginFramework::IPlugin*> WindowerPlugins;
	typedef stdext::hash_map<string_t, ICoreModule*> CoreModules;
	typedef PluginFramework::PluginManager PluginManager;

	//! \brief Plugin engine
	class PluginEngine : public BaseEngine
	{
	public:
		explicit PluginEngine(const TCHAR *pConfigFile_in);
		virtual ~PluginEngine();

		virtual bool Detach();

		bool LoadPlugin(const string_t &PluginName_in, bool ForceReload_in = false);
		bool UnloadPlugin(const string_t &PluginName_in);

		bool RegisterModule(const string_t ModuleName_in, ICoreModule *pModule_in);
		bool UnregisterModule(const string_t ModuleName_in);

		PluginFramework::IPlugin* GetPluginInstance(const string_t &PluginName_in);

	protected:
		//! the plugin services
		PluginServices *m_pPluginServices;
		//! the plugin manager
		PluginManager *m_pPluginManager;
		//! a hash map of plugin instances (name => instance)
		WindowerPlugins m_Plugins;	
		//! the modules containing the services
		CoreModules m_Modules;
	};
}

#endif//__PLUGIN_ENGINE_H__