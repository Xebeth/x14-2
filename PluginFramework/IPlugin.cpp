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
	IPluginServices *IPlugin::m_pServices = NULL;

	//! \brief IPlugin default constructor
	IPlugin::IPlugin() {}
	//! \brief IPlugin destructor
	IPlugin::~IPlugin() {}

	//! \brief RegisterParams default constructor
	RegisterParams::RegisterParams() : CreateFunc(NULL), DestroyFunc(NULL), QueryFunc(NULL) {}

	//! Callback function invoked when a new instance of the plugin has been created
	void IPlugin::OnCreate() {}
	//! Callback function invoked when a new instance of the plugin is about to be destroyed
	void IPlugin::OnDestroy() {}

	/*! \brief Initializes the plugin
		\param[in] pfnCreateFunc_in : a pointer to the 'Create' function of the plugin
		\param[in] pfnDestroyFunc_in : a pointer to the 'Destroy' function of the plugin
		\param[in] pfnQueryFunc_in : a pointer to the 'Query' function of the plugin
	*/
	RegisterParams* IPlugin::Initialize(fnCreate pfnCreateFunc_in, fnDestroy pfnDestroyFunc_in, fnQuery pfnQueryFunc_in)
	{
		if (pfnCreateFunc_in != NULL && pfnDestroyFunc_in != NULL && pfnQueryFunc_in != NULL)
		{
			RegisterParams *pRegisterParams = new RegisterParams;

			pRegisterParams->DestroyFunc = pfnDestroyFunc_in;
			pRegisterParams->CreateFunc = pfnCreateFunc_in;
			pRegisterParams->QueryFunc = pfnQueryFunc_in;

			pfnQueryFunc_in(pRegisterParams->Info);

			return pRegisterParams;
		}

		return NULL;
	}

	//! \brief Fills a PluginInfo structure with the plugin information
	void IPlugin::Query(PluginFramework::PluginInfo& PluginInfo_out)
	{
		PluginInfo_out.FrameworkVersion.FromString(__PLUGIN_FRAMEWORK_VERSION__);
		PluginInfo_out.Initialized = true;		
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

	/*! \brief Adds the plugin as a subscriber to the game chat service
		\return true if the subscription succeeded; false otherwise
	*/
	bool IPlugin::Subscribe() { return true; }

	/*! \brief Removes the plugin as a subscriber to the game chat service
		\return true if the subscription was revoked successfully; false otherwise
	*/
	bool IPlugin::Unsubscribe() { return true; }
}