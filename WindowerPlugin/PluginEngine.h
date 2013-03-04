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
	class VersionInfo;
}

#include "BaseEngine.h"
#include "PluginsServices.h"

//! compatibility flags
enum eCompatibilityFlags
{
	PLUGIN_COMPATIBILITY_NONE		= 0x00000000U,
	PLUGIN_COMPATIBILITY_WINDOWER	= 0x00000001U,
	PLUGIN_COMPATIBILITY_BOOTSTRAP	= 0x00000002U,
	PLUGIN_COMPATIBILITY_ANY		= (PLUGIN_COMPATIBILITY_WINDOWER | PLUGIN_COMPATIBILITY_BOOTSTRAP)
};

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
		PluginEngine(HMODULE hModule_in, const TCHAR *pConfigFile_in);
		virtual ~PluginEngine();

		static const TCHAR* GetCompatibilityFlagsText(DWORD Flags_in);

		virtual bool Detach();

		PluginFramework::IPlugin* LoadPlugin(const string_t &PluginName_in, bool ForceReload_in = false);
		bool UnloadPlugin(const string_t &PluginName_in);

		bool RegisterModule(const string_t ModuleName_in, ICoreModule *pModule_in);
		bool UnregisterModule(const string_t ModuleName_in);

		PluginFramework::IPlugin* GetPluginInstance(const string_t &PluginName_in);

		/*! \brief Retrieves the working directory of windower
			\return the working directory of windower
		*/
		const string_t& GetWorkingDir() const { return m_WorkingDir; }

	protected:
		void SetWorkingDir(HMODULE hModule_in);

		//! plugin engine version
		static const PluginFramework::VersionInfo m_FrameworkVersion;
		//! the plugin services
		static PluginServices *m_pPluginServices;
		//! the plugin manager
		PluginManager *m_pPluginManager;
		//! a hash map of plugin instances (name => instance)
		WindowerPlugins m_Plugins;	
		//! the working directory of windower
		string_t m_WorkingDir;
		//! the modules containing the services
		CoreModules m_Modules;
	};
}

#endif//__PLUGIN_ENGINE_H__