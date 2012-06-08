/**************************************************************************
	created		:	2011-05-29
	filename	: 	PluginManager.h
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin manager
**************************************************************************/
#ifndef __PLUGIN_MANAGER_H__
#define __PLUGIN_MANAGER_H__

#include <FileIterator.h>
#include <set>

namespace PluginFramework
{
	typedef stdext::hash_map<string_t, PluginInfo> RegisteredPlugins;
	typedef stdext::hash_map<string_t, RegisterParams*> LoadedPlugins;
	typedef stdext::hash_map<string_t, IPlugin*> PluginObjects;
	typedef std::set<PluginUUID> Blacklist;

	class PluginManager
	{
	public:
		explicit PluginManager(IPluginServices *pServices_in);
		virtual ~PluginManager();

		UINT ListPlugins(const string_t &Directory_in, DWORD CompatibilityFlags_in = 0xFFFFFFFF);
		bool ConfigurePlugin(const string_t &PluginName_in);
		IPlugin* LoadPlugin(const string_t &PluginName_in);
		bool UnloadPlugin(const string_t &PluginName_in);		

		bool IsPluginValid(const TCHAR *pPluginPath_in, DWORD CompatibilityFlags_in = 0xFFFFFFFF);
		bool IsPluginLoaded(const string_t &PluginName_in) const;
		string_t& GetVersionStr(string_t &Version_out) const;		
		bool RegisterPlugin(const PluginInfo &Info_in);

		/*! \brief Retrieves the list of registered plugins
			\return the list of registered plugins
		*/
		const RegisteredPlugins& GetRegisteredPlugins() const { return m_RegisteredPlugins; }

		bool BlacklistPlugin(const string_t &UUID_in);
		bool WhitelistPlugin(const string_t &UUID_in);

	private:
		IPlugin* CreateObject(const string_t &PluginName_in);
		void DestroyObject(const string_t &PluginName_in);
		inline HMODULE LoadDLL(const TCHAR *pDLLPath_in) const;
		inline bool UnloadDLL(HMODULE hModule_in) const;
		bool CheckDLLExports(HMODULE hModule_in, PluginInfo &Info_out) const;
		bool CheckPluginInfo(const PluginInfo &Info_in) const;
		RegisterParams* InitializePlugin(HMODULE hModule_in);

		//! plugin services used to (un)subscribe to services and invoke them
		IPluginServices *m_pServices;
		//! hash map of registered plugins
		RegisteredPlugins m_RegisteredPlugins;
		//! hash map of plugin instances
		PluginObjects m_PluginObjects;
		//! hash map of loaded plugins
		LoadedPlugins m_LoadedPlugins;
		//! hash map of blacklisted plugins
		Blacklist m_Blacklist;
	};
}

#endif//__PLUGIN_MANAGER_H__