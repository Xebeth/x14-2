/**************************************************************************
	created		:	2011-05-29
	filename	: 	PluginManager.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin Manager
**************************************************************************/
#include "stdafx.h"
#include <FileIterator.h>
#include <hash_map>
#include <vector>

#include "FrameworkVersion.h"
#include "VersionInfo.h"
#include "PluginUUID.h"

#include "IPluginServices.h"
#include "IPlugin.h"

#include "PluginManager.h"
#include "PluginIterator.h"

namespace PluginFramework
{
	/*! \brief PluginManager constructor 
		\param[in] Services_in : plugin services
	*/
	PluginManager::PluginManager(IPluginServices *pServices_in)
		: m_pServices(pServices_in) {}

	//! \brief PluginManager destructor
	PluginManager::~PluginManager()
	{
		if (m_LoadedPlugins.empty() == false)
		{
			LoadedPlugins::iterator Iter;

			for (Iter = m_LoadedPlugins.begin(); Iter != m_LoadedPlugins.end(); ++Iter)
				UnloadPlugin(Iter->first);
		}
	}

	/*! \brief Lists the available plugins in the specified directory
		\param[in] pDirectory_in : the directory in which we're looking for plugins
		\param[in] CompatibilityFlags_in : flags specifying the plugin compatibility with the current manager
		\return the number of available plugins
	*/
	UINT PluginManager::ListPlugins(const string_t &Directory_in, DWORD CompatibilityFlags_in)
	{
		PluginIterator Iter(Directory_in, *this);
		UINT PluginCount = 0;
		HANDLE hFile;

		UINT PrevErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

		// iterating effectively loads every DLLs in the directory to check them (see PluginIterator::IsValid())
		for (hFile = Iter.Begin(); hFile != Iter.End(); hFile = ++Iter)
			++PluginCount;

		SetErrorMode(PrevErrorMode);

		return PluginCount;
	}

	/*! \brief Checks if a plugin DLL is valid
		\param[in] pPluginPath_in : the full path to the plugin
		\return true if the plugin is valid; false otherwise
	*/
	bool PluginManager::IsPluginValid(const TCHAR *pPluginPath_in, DWORD CompatibilityFlags_in)
	{
		bool bResult = false;
		HMODULE hPlugin;

		if ((hPlugin = LoadDLL(pPluginPath_in)) != NULL)
		{
			fnQuery QueryFunc = NULL;
			PluginInfo Info;

			if (CheckDLLExports(hPlugin, Info))
			{
				DWORD Flags = Info.GetCompatibilityFlags();
				Info.m_DLLPath = pPluginPath_in;

				if ((Flags & CompatibilityFlags_in) == Flags)
					bResult = RegisterPlugin(Info);

				UnloadDLL(hPlugin);
			}

			return bResult;
		}

		return false;
	}

	/*! \brief Loads a DLL library and returns a handle to it
		\param[in] pDLLPath_in : the full path to the DLL
	*/
	HMODULE PluginManager::LoadDLL(const TCHAR *pDLLPath_in) const
	{
		return ::LoadLibrary(pDLLPath_in);
	}

	/*! \brief Unloads a previous loaded DLL
		\param[in] hModule_in : a handle to the DLL
		\return true if the DLL was successfully unloaded; false otherwise
	*/
	bool PluginManager::UnloadDLL(HMODULE hModule_in) const
	{
		return (::FreeLibrary(hModule_in) != FALSE);
	}

	/*! \brief Checks if all the expected function exports are present in the DLL
		\param[in] hModule_in : a handle to the DLL
		\return true if the exports are correct; false otherwise
	*/
	bool PluginManager::CheckDLLExports(HMODULE hModule_in, PluginInfo &Info_out) const
	{
		if (hModule_in != NULL)
		{
			fnInitialize pInitialize = (fnInitialize)::GetProcAddress(hModule_in, "InitPlugin");

			if (pInitialize != NULL)
			{
				RegisterParams *pParams = pInitialize();

				if (pParams != NULL)
				{
					Info_out = pParams->Info;
					delete pParams;

					return CheckPluginInfo(Info_out);
				}
			}
		}

		return false;
	}

	/*! \brief Checks the plugin version and identifier
		\param[out] Info_in : structure containing the plugin information
		\return true if the plugin version matches the framework version
					 and the plugin identifier is not blacklisted
	*/
	bool PluginManager::CheckPluginInfo(const PluginInfo &Info_in) const
	{
		return (m_pServices != NULL && Info_in.m_FrameworkVersion == m_pServices->GetVersion());
	}

	/*! \brief Registers a plugin
		\param[in] Info_in : the plugin information
		\return true if the plugin was registered; false otherwise
	*/
	bool PluginManager::RegisterPlugin(const PluginInfo &Info_in)
	{
		if (m_RegisteredPlugins.find(Info_in.m_Name) == m_RegisteredPlugins.end())
		{
			// only register the plugin if it's not blacklisted
			if (m_Blacklist.find(Info_in.m_PluginIdentifier) == m_Blacklist.end())
				m_RegisteredPlugins[Info_in.m_Name] = Info_in;

			return true;
		}
		else
			return false;
	}

	/*! \brief Loads a plugin given its name
		\param[in] PluginName_in : the name of the plugin to load
		\return a pointer to the plugin instance if successful; NULL otherwise
	*/
	IPlugin* PluginManager::LoadPlugin(const string_t &PluginName_in)
	{
		RegisteredPlugins::iterator RegIter = m_RegisteredPlugins.find(PluginName_in);

		// is the plugin registered?
		if (RegIter != m_RegisteredPlugins.end())
		{
			LoadedPlugins::iterator LoadIter = m_LoadedPlugins.find(PluginName_in);
			RegisterParams *pParams = NULL;
			HMODULE Handle = NULL;

			// is the plugin already loaded?
			if (LoadIter == m_LoadedPlugins.end())
			{
				// the plugin has never been loaded
				Handle = LoadDLL(RegIter->second.m_DLLPath.c_str());
				pParams = InitializePlugin(Handle);
				// store the plugin with the loaded plugins
				m_LoadedPlugins[PluginName_in] = pParams;
			}
			else if (LoadIter->second->Info.m_hHandle == NULL)
			{
				// the plugin may have been loaded but has no handle
				Handle = LoadDLL(RegIter->second.m_DLLPath.c_str());
				LoadIter->second->Info.m_hHandle = Handle;
				pParams = LoadIter->second;
			}
			else
				pParams = LoadIter->second;

			if (pParams != NULL)
			{
				// update the DLL handle of the registered plugin
				RegIter->second.m_hHandle = Handle;

				return CreateObject(PluginName_in);
			}
		}

		// plugin is not registered
		return NULL;
	}

	/*! \brief Creates an instance of a loaded plugin given its name
		\param[in] PluginName_in : the name of the plugin to instantiate
		\return a pointer to the plugin instance if successful; NULL otherwise
	*/
	IPlugin* PluginManager::CreateObject(const string_t &PluginName_in)
	{
		PluginObjects::iterator ObjIter = m_PluginObjects.find(PluginName_in);

		if (ObjIter == m_PluginObjects.end())
		{
			LoadedPlugins::iterator LoadIter = m_LoadedPlugins.find(PluginName_in);
			IPlugin *pResult = NULL;

			if (LoadIter == m_LoadedPlugins.end())
				if (LoadPlugin(PluginName_in))
					LoadIter = m_LoadedPlugins.find(PluginName_in);

			if (LoadIter != m_LoadedPlugins.end())
			{
				// create a new instance
				pResult = LoadIter->second->CreateFunc(m_pServices);
				// if the object creation succeeded; store it
				if (pResult != NULL)
				{
					pResult->m_PluginInfo = LoadIter->second->Info;
					pResult->m_pRegisterParams = LoadIter->second;
					pResult->OnCreate();

					m_PluginObjects[PluginName_in] = pResult;
				}
			}

			return pResult;
		}
		else
			return ObjIter->second;
	}

	/*! \brief Unloads a plugin given its name
		\param[in] PluginName_in : the name of the plugin to unload
		\return true if successful; false otherwise
	*/
	bool PluginManager::UnloadPlugin(const string_t &PluginName_in)
	{
		LoadedPlugins::iterator LoadIter = m_LoadedPlugins.find(PluginName_in);

		// is the plugin already loaded?
		if (LoadIter != m_LoadedPlugins.end())
		{
			// retrieve the handle of the DLL
			HMODULE hDLL = LoadIter->second->Info.m_hHandle;
			// destroy the plugin instance
			DestroyObject(PluginName_in);
			delete LoadIter->second;
			// remove the plugin from the loaded plugins
			m_LoadedPlugins.erase(LoadIter);
			// unload the DLL
			return UnloadDLL(hDLL);
		}

		return false;
	}

	/*! \brief Destroys an instance of a loaded plugin given its name
		\param[in] PluginName_in : the name of the plugin to destroy
	*/
	void PluginManager::DestroyObject(const string_t &PluginName_in)
	{
		PluginObjects::iterator ObjIter = m_PluginObjects.find(PluginName_in);

		if (ObjIter != m_PluginObjects.end())
		{
			LoadedPlugins::iterator LoadIter = m_LoadedPlugins.find(PluginName_in);
			IPlugin *pPlugin = ObjIter->second;

			if (LoadIter != m_LoadedPlugins.end() && pPlugin != NULL)
			{
				// callback
				pPlugin->OnDestroy();
				// destroy the plugin instance
				pPlugin->m_pRegisterParams = NULL;
				LoadIter->second->DestroyFunc(ObjIter->second);
				// remove the object from the loaded objects
				m_PluginObjects.erase(ObjIter);
			}
		}
	}

	/*! \brief Initializes a plugin given its DLL handle
		\param[in] hModule_in : the DLL handle of the plugin to initialize
		\return a pointer to the plugin registration parameters if successful; NULL otherwise
	*/
	RegisterParams* PluginManager::InitializePlugin(HMODULE hModule_in)
	{
		fnInitialize pInitialize = (fnInitialize)::GetProcAddress(hModule_in, "InitPlugin");

		if (pInitialize != NULL)
		{
			RegisterParams *pResult = pInitialize();

			if (pResult != NULL)
			{
				pResult->Info.m_hHandle = hModule_in;

				return pResult;
			}
		}

		return NULL;
	}

	/*! \brief Checks if the specified plugin is loaded
		\param[in] PluginName_in : the name of the plugin
		\return true if the plugin is loaded; false otherwise
	*/
	bool PluginManager::IsPluginLoaded(const string_t &PluginName_in) const
	{
		PluginObjects::const_iterator ObjIter = m_PluginObjects.find(PluginName_in);

		return (ObjIter != m_PluginObjects.end());
	}

	bool PluginManager::IsPluginConfigurable(const string_t &PluginName_in) const
	{
		LoadedPlugins::const_iterator PluginIt = m_LoadedPlugins.find(PluginName_in);

		if (PluginIt != m_LoadedPlugins.end())
			return (PluginIt->second->ConfigureFunc != NULL);

		return false;
	}

	bool PluginManager::BlacklistPlugin(const string_t &UUID_in)
	{
		PluginUUID UUID(UUID_in.c_str());
		Blacklist::const_iterator Iter = m_Blacklist.find(UUID);

		if (Iter == m_Blacklist.end())
		{
			m_Blacklist.insert(UUID);

			return true;
		}

		return false;
	}

	bool PluginManager::WhitelistPlugin(const string_t &UUID_in)
	{
		PluginUUID UUID(UUID_in.c_str());
		Blacklist::const_iterator Iter = m_Blacklist.find(UUID);

		if (Iter != m_Blacklist.end())
		{
			m_Blacklist.erase(Iter);

			return true;
		}

		return false;
	}

	bool PluginManager::ConfigurePlugin(const string_t &PluginName_in, const LPVOID pUserData_in)
	{
		IPlugin *pPlugin = LoadPlugin(PluginName_in);

		if (pPlugin != NULL)
			return pPlugin->Configure(pUserData_in);

		return false;
	}
}