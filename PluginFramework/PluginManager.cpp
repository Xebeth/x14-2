/**************************************************************************
	created		:	2011-05-29
	filename	: 	PluginManager.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin Manager
**************************************************************************/
#include "stdafx.h"
#include <hash_map>
#include <vector>

#include "PluginUUID.h"
#include "PluginVersion.h"
#include "IPluginServices.h"
#include "PluginSupport.h"
#include "IPlugin.h"

#include <FileIterator.h>
#include "PluginManager.h"
#include "PluginIterator.h"

namespace PluginFramework
{
	PluginVersion PluginManager::m_Version(_T("1.0.0"));

	/*! \brief PluginManager default constructor */
	PluginManager::PluginManager(const IPluginServices *pServices_in)
		: m_pServices(pServices_in) {}

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
		\return the number of available plugins
	*/
	unsigned int PluginManager::ListPlugins(const string_t &Directory_in)
	{
		PluginIterator Iter(Directory_in, *this);
		unsigned int PluginCount = 0;
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
	bool PluginManager::IsPluginValid(const TCHAR *pPluginPath_in)
	{
		bool bResult = false;
		HMODULE hPlugin;

		if ((hPlugin = LoadDLL(pPluginPath_in)) != NULL)
		{
			fnQuery QueryFunc = NULL;
			PluginInfo Info;

			bResult  = CheckDLLExports(hPlugin, &QueryFunc);
			bResult &= CheckPluginInfo(QueryFunc, Info);
			bResult &= RegisterPlugin(Info, pPluginPath_in);
			bResult &= UnloadDLL(hPlugin);

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
	bool PluginManager::CheckDLLExports(HMODULE hModule_in, fnQuery *pQueryFunc_out) const
	{
		if (hModule_in != NULL && pQueryFunc_out != NULL && *pQueryFunc_out == NULL)
		{
			fnInitialize pInitialize = (fnInitialize)::GetProcAddress(hModule_in, "InitPlugin");
			fnTerminate pTerminate = (fnTerminate)::GetProcAddress(hModule_in, "TerminatePlugin");
			*pQueryFunc_out = (fnQuery)::GetProcAddress(hModule_in, "QueryPlugin");

			return (pTerminate != NULL && pInitialize != NULL && *pQueryFunc_out != NULL);
		}

		return false;
	}

	/*! \brief Checks the plugin version and identifier
		\param[in] pQueryFunc_in : a function pointer to QueryPlugin(...)
		\param[out Info_out : structure receiving the plugin information
		\return true if the plugin version matches the framework version
					 and the plugin identifier is not blacklisted
	*/
	bool PluginManager::CheckPluginInfo(const fnQuery pQueryFunc_in, PluginInfo &Info_out) const
	{
		if (pQueryFunc_in != NULL)
		{
			pQueryFunc_in(Info_out);

			if (Info_out.FrameworkVersion == m_Version)
			{
				if (m_Blacklist.empty() == false)
				{
					Blacklist::const_iterator Iter;

					for (Iter = m_Blacklist.begin(); Iter != m_Blacklist.end(); ++Iter)
						if (*Iter == Info_out.PluginIdentifier)
							return false;
				}
			}

			return true;
		}

		return false;
	}

	/*! \brief Registers a plugin
		\param[in] Identifier_in : a plugin identifier
		\param[in] PluginPath_in : the path of the plugin
		\return true if the plugin was registered; false otherwise
	*/
	bool PluginManager::RegisterPlugin(PluginInfo &Info_in_out, string_t PluginPath_in)
	{
		if (m_RegisteredPlugins.find(Info_in_out.Name) == m_RegisteredPlugins.end())
		{
			Info_in_out.DLLPath = PluginPath_in;
			m_RegisteredPlugins[Info_in_out.Name] = Info_in_out;

			return true;
		}
		else
			return false;
	}

	/*! \brief Returns the version of the framework as a string
		\return a string containing the framework version
	*/
	string_t& PluginManager::GetVersionStr(string_t &Version_out) const
	{
		return (Version_out = m_Version.ToString());
	}

	/*! \brief 
	\param[] PluginName_in : 
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
				Handle = LoadDLL(RegIter->second.DLLPath.c_str());
				pParams = InitializePlugin(Handle);
				// store the plugin with the loaded plugins
				m_LoadedPlugins[PluginName_in] = pParams;
			}
			else if (LoadIter->second->Info.Handle == NULL)
			{
				// the plugin may have been loaded but has no handle
				Handle = LoadDLL(RegIter->second.DLLPath.c_str());
				LoadIter->second->Info.Handle = Handle;
				pParams = LoadIter->second;
			}
			else
				pParams = LoadIter->second;

			if (pParams != NULL)
			{
				// update the DLL handle of the registered plugin
				RegIter->second.Handle = Handle;

				return CreateObject(PluginName_in);
			}
		}

		// plugin is not registered
		return NULL;
	}

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
				pResult = LoadIter->second->CreateFunc();
				// if the object creation succeeded; store it
				if (pResult != NULL)
				{
					pResult->SetInfo(LoadIter->second->Info);
					m_PluginObjects[PluginName_in] = pResult;
				}
			}

			return pResult;
		}
		else
			return ObjIter->second;
	}

	bool PluginManager::UnloadPlugin(const string_t &PluginName_in)
	{
		LoadedPlugins::iterator LoadIter = m_LoadedPlugins.find(PluginName_in);

		// is the plugin already loaded?
		if (LoadIter != m_LoadedPlugins.end())
		{
			// retrieve the handle of the DLL
			HMODULE hDLL = LoadIter->second->Info.Handle;
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

	void PluginManager::DestroyObject(const string_t &PluginName_in)
	{
		PluginObjects::iterator ObjIter = m_PluginObjects.find(PluginName_in);

		if (ObjIter != m_PluginObjects.end())
		{
			LoadedPlugins::iterator LoadIter = m_LoadedPlugins.find(PluginName_in);

			if (LoadIter != m_LoadedPlugins.end())
			{
				// destroy the plugin instance
				LoadIter->second->DestroyFunc(ObjIter->second);
				// remove the object from the loaded objects
				m_PluginObjects.erase(ObjIter);
			}
		}
	}

	RegisterParams* PluginManager::InitializePlugin(HMODULE hModule_in)
	{
		fnInitialize pInitFunc = (fnInitialize)::GetProcAddress(hModule_in, "InitPlugin");

		if (pInitFunc != NULL)
		{
			RegisterParams *pResult = pInitFunc(m_pServices);

			if (pResult != NULL)
			{
				pResult->Info.Handle = hModule_in;

				return pResult;
			}
		}

		return NULL;
	}
}