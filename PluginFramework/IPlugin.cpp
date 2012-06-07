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
		\param[in] pfnCreateFunc_in : a pointer to the 'Create' function of the plugin
		\param[in] pfnDestroyFunc_in : a pointer to the 'Destroy' function of the plugin
		\param[in] pfnQueryFunc_in : a pointer to the 'Query' function of the plugin
		\param[in] pfnConfigureFunc_in : a pointer to the 'Configure' function of the plugin
	*/
	RegisterParams* IPlugin::Initialize(fnCreate pfnCreateFunc_in, fnDestroy pfnDestroyFunc_in,
										fnQuery pfnQueryFunc_in, fnConfigure pfnConfigureFunc_in)
	{
		if (pfnCreateFunc_in != NULL && pfnDestroyFunc_in != NULL
		 && pfnQueryFunc_in != NULL && pfnConfigureFunc_in != NULL)
		{
			RegisterParams *pRegisterParams = new RegisterParams;

			pRegisterParams->ConfigureFunc = pfnConfigureFunc_in;
			pRegisterParams->DestroyFunc = pfnDestroyFunc_in;
			pRegisterParams->CreateFunc = pfnCreateFunc_in;
			pRegisterParams->QueryFunc = pfnQueryFunc_in;

			pfnQueryFunc_in(pRegisterParams->Info);
			Query(pRegisterParams->Info);

			return pRegisterParams;
		}

		return NULL;
	}

	//! \brief PluginInfo constructor
	PluginInfo::PluginInfo() : Initialized(false), hHandle(NULL) {}

	/*! \brief Retrieves the string representation of the plugin info
		\return the string representation of the plugin info
	*/
	string_t PluginInfo::ToString() const
	{
		return format(_T("Plugin '%s v%s':\n")
					  _T("\tAuthor:\t\t%s\n")
					  _T("\tDescritpion:\t%s\n"),
					  Name.c_str(), Version.ToString().c_str(),
					  Author.c_str(), Descritpion.c_str());
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
		PluginInfo_out.FrameworkVersion.FromString(__PLUGIN_FRAMEWORK_VERSION__);
		PluginInfo_out.Initialized = true;
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
}