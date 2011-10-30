/**************************************************************************
	created		:	2011-05-29
	filename	: 	PluginManager.h
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin manager
**************************************************************************/
#ifndef __PLUGIN_MANAGER_H__
#define __PLUGIN_MANAGER_H__

namespace PluginFramework
{
	typedef stdext::hash_map<string_t, PluginInfo> RegisteredPlugins;
	typedef stdext::hash_map<string_t, RegisterParams*> LoadedPlugins;
	typedef stdext::hash_map<string_t, IPlugin*> PluginObjects;
	typedef std::vector<PluginUUID> Blacklist;

	class PluginManager
	{
	public:
		explicit PluginManager(const IPluginServices &Services_in);
		virtual ~PluginManager();

		IPlugin* LoadPlugin(const string_t &PluginName_in);
		bool UnloadPlugin(const string_t &PluginName_in);

		unsigned int ListPlugins(const string_t &Directory_in);
		bool IsPluginValid(const TCHAR *pPluginPath_in);
		string_t& GetVersionStr(string_t &Version_out) const;
		bool RegisterPlugin(PluginInfo &Info_in_out, string_t PluginPath_in);

	private:
		IPlugin* CreateObject(const string_t &PluginName_in);
		void DestroyObject(const string_t &PluginName_in);
		inline HMODULE LoadDLL(const TCHAR *pDLLPath_in) const;
		inline bool UnloadDLL(HMODULE hModule_in) const;
		bool CheckPluginInfo(fnQuery pQueryFunc_in, PluginInfo &Info_out) const;
		bool CheckDLLExports(HMODULE hModule_in, fnQuery *pQueryFunc_out) const;
		RegisterParams* InitializePlugin(HMODULE hModule_in);

		//! hash map of registered plugins
		RegisteredPlugins m_RegisteredPlugins;
		//! hash map of plugin instances
		PluginObjects m_PluginObjects;
		//! hash map of loaded plugins
		LoadedPlugins m_LoadedPlugins;
		//! hash map of blacklisted plugins
		Blacklist m_Blacklist;
		//! plugin services used to (un)subscribe to services and invoke them
		const IPluginServices &m_Services;
		//! plugin manager versions
		static const PluginVersion m_Version;
	};
}

#endif//__PLUGIN_MANAGER_H__