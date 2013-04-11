/**************************************************************************
	created		:	2011-06-04
	filename	: 	AutoLoginPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Auto login plugin
					Monitors the forms during the login process
					and automatically fills the password field
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <SettingsManager.h>

#include "AutoLoginConfigDlg.h"
#include "IAutoLoginPlugin.h"
#include "AutoLoginPlugin.h"
#include "AutoLoginSettings.h"
#include "AutoLogin.h"
#include "version.h"

namespace Bootstrap
{
	//! \brief AutoLoginPlugin default constructor
	AutoLoginPlugin::AutoLoginPlugin(PluginFramework::IPluginServices *pServices_in)
		: IAutoLoginPlugin(pServices_in), m_hThread(NULL), 
		  m_pSettings(new AutoLoginSettings(PluginFramework::IPlugin::GetConfigFile(), NULL)) {}

	//! \brief AutoLoginPlugin destructor
	AutoLoginPlugin::~AutoLoginPlugin()
	{
		if (m_pSettings != NULL)
		{
			delete m_pSettings;
			m_pSettings = NULL;
		}
	}

	/*! \brief Creates an instance of AutoLoginPlugin
		\return a pointer to the new AutoLoginPlugin instance
	*/
	PluginFramework::IPlugin* AutoLoginPlugin::Create(PluginFramework::IPluginServices *pServices_in)
	{
		return new AutoLoginPlugin(pServices_in);
	}

	/*! \brief Destroys an instance of AutoLoginPlugin
		\param[in] pInstance_in : an instance of AutoLoginPlugin
	*/
	void AutoLoginPlugin::Destroy(PluginFramework::IPlugin *pInstance_in)
	{
		if (pInstance_in != NULL)
		{
			delete pInstance_in;
			pInstance_in = NULL;
		}
	}

	//! \brief Fills a PluginInfo structure with the plugin information
	void AutoLoginPlugin::Query(PluginFramework::PluginInfo& PluginInfo_out)
	{
		PluginInfo_out.SetDesc(_T("This plugin will automatically fill the password field during the login process"));
		PluginInfo_out.SetIdentifier(_T("9D0E9E80-A5CB-11E0-8264-0800200C9A66"));
		PluginInfo_out.SetCompatibilityFlags(0x2U);	// PLUGIN_COMPATIBILITY_BOOTSTRAP
		PluginInfo_out.SetVersion(PLUGIN_VERSION);		
		PluginInfo_out.SetAuthor(_T("Xebeth`"));
		PluginInfo_out.SetName(_T("AutoLogin"));		
	}
	
	/*! \brief Opens the configuration screen of the plugin
		\param[in] pInstance_in : the instance of the plugin to configure
		\param[in] pUserData_in : a pointer to the user data to pass to the plugin
		\return true if the user validated the configuration screen; false otherwise
	*/
	bool AutoLoginPlugin::Configure(PluginFramework::IPlugin *pInstance_in, const LPVOID pUserData_in)
	{
		AutoLoginConfigDlg ConfigDlg(PluginFramework::IPlugin::GetConfigFile(), reinterpret_cast<const TCHAR*>(pUserData_in));

		return (ConfigDlg.DoModal() == IDOK);
	}

	/*! \brief Creates the thread used to monitor the forms during the login process
		\param[in] ParentHwnd_in : the handle of the IE server window
		\return true if the thread was created successfully; false otherwise
	*/
	bool AutoLoginPlugin::CreateAutoLoginThread(HWND ParentHwnd_in)
	{
		if (ParentHwnd_in != NULL)
		{
			m_pSettings->SetParentWnd(ParentHwnd_in);

			return (::CreateThread(NULL, 0, ::AutoLoginThread, (LPVOID)m_pSettings, 0, NULL) != NULL);
		}

		return false;
	}
}

using Bootstrap::AutoLoginPlugin;

/*! \brief Function exposed by the plugin DLL to initialize the plugin object
	\param[out] RegisterParams_out : Registration structure to be able to use the plugin
	\return true if the initialization succeeded; false otherwise
*/
extern "C" PLUGIN_API bool InitPlugin(PluginFramework::RegisterParams &RegisterParams_out)
{
	return PluginFramework::IPlugin::Initialize(RegisterParams_out, AutoLoginPlugin::Create, AutoLoginPlugin::Destroy,
												AutoLoginPlugin::Query, AutoLoginPlugin::Configure);
}