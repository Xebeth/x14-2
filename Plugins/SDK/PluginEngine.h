/**************************************************************************
	created		:	2011-07-03
	filename	: 	PluginEngine.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin engine
**************************************************************************/
#ifndef __PLUGIN_ENGINE_H__
#define __PLUGIN_ENGINE_H__

#ifndef _WINDOWER_PLUGIN_SDK_INCLUDE_
	#error Please include the global header file 'PluginSDK.h'
#endif

namespace PluginFramework
{
	class PluginManager;
	class IUserSettings;
	class VersionInfo;
}

#ifdef _M_X64
	#define PLUGIN_DIRECTORY _T("plugins\\x64")
#else
	#define PLUGIN_DIRECTORY _T("plugins\\x86")
#endif // _M_X64

//! compatibility flags
enum eCompatibilityFlags
{
	PLUGIN_COMPATIBILITY_NONE		= 0x00000000U,
	PLUGIN_COMPATIBILITY_WINDOWER	= 0x00000001U,
	PLUGIN_COMPATIBILITY_BOOTSTRAP	= 0x00000002U,
	PLUGIN_COMPATIBILITY_NO_WIZARD	= 0x10000000U,
	PLUGIN_COMPATIBILITY_MASK		= 0x0FFFFFFFU,
	PLUGIN_COMPATIBILITY_ANY		= (PLUGIN_COMPATIBILITY_WINDOWER | PLUGIN_COMPATIBILITY_BOOTSTRAP),
};

namespace Windower
{
	class SettingsManager;
	class PluginServices;
	class ICoreModule;

	typedef hash_map<string_t, PluginFramework::IPlugin*> WindowerPlugins;
	typedef std::vector<PluginFramework::IPlugin*> ConfigurationQueue;
	typedef hash_map<string_t, ICoreModule*> CoreModules;
	typedef std::set<string_t> ActivePlugins;

	//! \brief Plugin engine
	class PluginEngine : public BaseEngine
	{
	public:
		PluginEngine(const string_t &WorkingDir_in, const TCHAR *pConfigFile_in);
		virtual ~PluginEngine();

		/*! \brief Retrieves the compatibility flags of the plugin
			\return the compatibility flags text
		*/
		static const TCHAR* GetCompatibilityFlagsText(DWORD Flags_in);

		bool Detach() override;

		PluginFramework::IPlugin* LoadPlugin(const string_t& PluginName_in, bool ForceReload_in = false);
		bool UnloadPlugin(const string_t& PluginName_in);
		bool ConfigurePlugin(string_t &PluginName_in);

		/*! \brief Loads a set of plugins
			\param[in] PluginSet_in : a list of plugins name to load
			\return the number of loaded plugins
		*/
		size_t LoadPlugins(const ActivePlugins& PluginSet_in);
		/*! \brief Lists all the available plugins and their state (loaded or not)
			\param[out] Feedback_out : the string receiving the result
			\return true if the list was retrieved successfully; false otherwise
		*/
		bool ListPlugins(std::string& Feedback_out) const;

		bool RegisterModule(const string_t& ModuleName_in, ICoreModule *pModule_in);
		bool UnregisterModule(const string_t& ModuleName_in);

		PluginFramework::IPlugin* GetPluginInstance(const string_t& PluginName_in);

		/*! \brief Retrieves the working directory of windower
			\return the working directory of windower
		*/
		const string_t& GetWorkingDir() const;
		const TCHAR* GetGameDirectory() const;

	protected:
		void PushPluginConfigure(PluginFramework::IPlugin *pPlugin_in);
		bool PopPluginConfigure();

		/*! \brief Sets the working directory of windower
			\param[in] pWorkingDir_in : the new working directory
		*/
		void SetWorkingDir(const TCHAR *pWorkingDir_in);

		//! plugin engine version
		static const PluginFramework::VersionInfo m_FrameworkVersion;
		//! the plugin services
		PluginServices *m_pPluginServices;
		//! settings
		SettingsManager *m_pSettingsManager;
		//! the plugin manager
		PluginFramework::PluginManager *m_pPluginManager;
		//! a hash map of plugin instances (name => instance)
		WindowerPlugins m_Plugins;	
		//! the working directory of windower
		string_t m_WorkingDir;
		//! the modules containing the services
		CoreModules m_Modules;
		//! queue of plugins waiting to be configured (must be ran in a thread)
		ConfigurationQueue m_ConfigQueue;
	};
}

#endif//__PLUGIN_ENGINE_H__