/**************************************************************************
	created		:	2011-11-06
	filename	: 	IPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin interface
**************************************************************************/
#include "stdafx.h"

#include "FrameworkVersion.h"
#include "VersionInfo.h"
#include "PluginUUID.h"
#include "IPlugin.h"

#include "IPluginServices.h"
#include "PluginManager.h"
#include "PluginIterator.h"

namespace PluginFramework
{
	IPluginServices * IPlugin::m_pServices = NULL;

	/*! \brief IPlugin constructor
		\param[in] pServices_in : a pointer to the plugin services
	*/
	IPlugin::IPlugin(IPluginServices *pServices_in)
		: m_pRegisterParams(NULL)
	{
		m_pServices = pServices_in;
	}
	//! \brief IPlugin destructor
	IPlugin::~IPlugin() {}

	//! \brief RegisterParams default constructor
	RegisterParams::RegisterParams()
		: CreateFunc(NULL), DestroyFunc(NULL), 
		  QueryFunc(NULL), ConfigureFunc(NULL) {}

	/*! \brief Initializes the plugin
		\param[out] RegisterParams_out : Registration structure to be able to use the plugin
		\param[in] pfnCreateFunc_in : a pointer to the 'Create' function of the plugin
		\param[in] pfnDestroyFunc_in : a pointer to the 'Destroy' function of the plugin
		\param[in] pfnQueryFunc_in : a pointer to the 'Query' function of the plugin
		\param[in] pfnConfigureFunc_in : a pointer to the 'Configure' function of the plugin
		\return true if the initialization succeeded; false otherwise
	*/
	bool IPlugin::Initialize(RegisterParams &RegisterParams_out, fnCreate pfnCreateFunc_in, 
							 fnDestroy pfnDestroyFunc_in, fnQuery pfnQueryFunc_in,
							 fnConfigure pfnConfigureFunc_in)
	{
		if (pfnCreateFunc_in != NULL && pfnDestroyFunc_in != NULL && pfnQueryFunc_in != NULL)
		{
			RegisterParams_out.ConfigureFunc = pfnConfigureFunc_in;
			RegisterParams_out.DestroyFunc = pfnDestroyFunc_in;
			RegisterParams_out.CreateFunc = pfnCreateFunc_in;
			RegisterParams_out.QueryFunc = pfnQueryFunc_in;

			pfnQueryFunc_in(RegisterParams_out.Info);
			Query(RegisterParams_out.Info);

			return true;
		}

		return false;
	}
	
	/*! \brief Displays the configuration screen of the plugin
		\param[in] pUserData_in : a pointer to the user data to pass to the plugin
		\return true if the user validated the screen; false otherwise
	*/
	bool IPlugin::Configure(const LPVOID pUserData_in)
	{
		if (IsConfigurable() == false)
		{
			MessageBox(NULL, _T("This plugin has no configuration."),
					   m_PluginInfo.GetName().c_str(),
					   MB_OK | MB_ICONINFORMATION);

			return false;
		}
		else
			return m_pRegisterParams->ConfigureFunc(this, pUserData_in);
	}

	//! \brief PluginInfo constructor
	PluginInfo::PluginInfo() : m_Initialized(false), m_hHandle(NULL),
		m_CompatibilityFlags(0U) {}

	/*! \brief Retrieves the string representation of the plugin info
		\return the string representation of the plugin info
	*/
	string_t PluginInfo::ToString() const
	{
		return format(_T("Plugin '%s v%s':\n")
					  _T("  Author:      %s\n")
					  _T("  Description: %s\n"),
					  m_Name.c_str(), m_Version.ToString().c_str(),
					  m_Author.c_str(), m_Description.c_str());
	}

	/*! \brief Converts the plugin information to a string
		\return a string representation of the plugin information
	*/
	string_t IPlugin::ToString() const
	{
		return m_PluginInfo.ToString();
	}

	/*! \brief Adds a plugin subscription to the service in the specified module
		\param[in] ModuleName_in : the name of the module
		\param[in] ServiceName_in : the name of the service
		\return true if successful; false otherwise
	*/
	bool IPlugin::SubscribeService(const string_t &ModuleName_in, const string_t &ServiceName_in)
	{
		return (m_pServices != NULL && m_pServices->SubscribeService(ModuleName_in, ServiceName_in, this));
	}

	/*! \brief Removes a plugin subscription from the service in the specified module
		\param[in] ModuleName_in : the name of the module
		\param[in] ServiceName_in : the name of the service
		\return true if successful; false otherwise
	*/
	bool IPlugin::UnsubscribeService(const string_t &ModuleName_in, const string_t &ServiceName_in)
	{
		return (m_pServices != NULL && m_pServices->UnsubscribeService(ModuleName_in, ServiceName_in, this));
	}

	//! Callback function invoked when a new instance of the plugin has been created
	void IPlugin::OnCreate()
	{
		Subscribe();
		RegisterCommands();
	}

	//! Callback function invoked when a new instance of the plugin is about to be destroyed
	void IPlugin::OnDestroy()
	{
		UnregisterCommands();
		Unsubscribe();
	}

	//! \brief Fills a PluginInfo structure with the plugin information
	void IPlugin::Query(PluginFramework::PluginInfo& PluginInfo_out)
	{
		PluginInfo_out.m_FrameworkVersion.FromString(__PLUGIN_FRAMEWORK_VERSION__);
		PluginInfo_out.m_Initialized = true;

		CleanupName(PluginInfo_out.m_Name);
	}

	/*! \brief Cleans up the specified string to make it a valid plugin name
		\param[in,out] Name_in_out : the string to clean up
	*/
	void IPlugin::CleanupName( string_t Name_in_out )
	{
		string_t::size_type Pos, Offset = 0;

		Pos = Name_in_out.find_first_of('|', Offset);

		while(Pos != string_t::npos)
		{
			Name_in_out.erase(Pos, 1);

			Offset = Pos + 1;
			Pos = Name_in_out.find_first_of('|', Offset);
		}
	}

	/*! \brief Invokes a command registered with the service in the specified module
		\param[in] ModuleName_in : the name of the module
		\param[in] ServiceName_in : the name of the service
		\param[in] Params_in : the input parameters
		\return true if the command was invoked successfully; false otherwise
	*/
	bool IPlugin::InvokeService(const string_t &ModuleName_in,
								const string_t &ServiceName_in,
								const ServiceParam &Params_in)
	{
		return (m_pServices != NULL && m_pServices->InvokeService(ModuleName_in, ServiceName_in, Params_in));
	}

	/*! \brief Retrieves the configuration file path
		\return the configuration file path
	*/
	const TCHAR* IPlugin::GetConfigFile()
	{
		return m_pServices->GetConfigFile();
	}
}