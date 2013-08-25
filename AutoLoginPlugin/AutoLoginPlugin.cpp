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
#include "version.h"
#include <afxdlgs.h>

#include <PluginPropertyPage.h>

#include "AutoLogin.h"
#include "AutoLoginConfigDlg.h"
#include "AutoLoginPlugin.h"

namespace Bootstrap
{
	//! \brief AutoLoginPlugin constructor
	AutoLoginPlugin::AutoLoginPlugin(PluginFramework::IPluginServices *pServices_in)
		: Windower::ConfigurablePlugin(pServices_in)
	{
		// clear the thread data structure
		memset(&m_ThreadData, 0, sizeof(m_ThreadData));		
	}
	
	/*! \brief Creates the thread used to monitor the forms during the login process
		\param[in] ParentHwnd_in : the handle of the IE server window
		\return true if the thread was created successfully; false otherwise
	*/
	bool AutoLoginPlugin::CreateAutoLoginThread(HWND ParentHwnd_in)
	{
		if (ParentHwnd_in != NULL && m_pSettings != NULL)
		{
			m_ThreadData.m_hParentWnd = ParentHwnd_in;
			m_ThreadData.m_pSettings = m_pSettings;

			return (::CreateThread(NULL, 0, ::AutoLoginThread, (LPVOID)&m_ThreadData, 0, NULL) != NULL);
		}

		return false;
	}

	/*! \brief Creates an instance of AutoLoginPlugin
		\return a pointer to the new AutoLoginPlugin instance
	*/
	PluginFramework::IPlugin* AutoLoginPlugin::Create(PluginFramework::IPluginServices *pServices_in)
	{
		HWND hIEServerWnd = NULL;
		AutoLoginPlugin *pPlugin = new AutoLoginPlugin(pServices_in);
		PluginFramework::ServiceParam Param(_T("HWND*"), &hIEServerWnd);

		if (InvokeService(_T(SYSTEM_MODULE), _T(IE_SERVER_HWND_SERVICE), Param))
		{
			// start the thread
			pPlugin->CreateAutoLoginThread(hIEServerWnd);
		}

		return pPlugin;
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
	
	Windower::PluginPropertyPage* AutoLoginPlugin::GetPropertyPage()
	{
		return new AutoLoginConfigDlg(m_pSettings, m_PluginInfo.GetName());
	}

	void AutoLoginPlugin::OnSettingsChanged() {}

	/*! \brief Adds the plugin as a subscriber to the ie server handle service
		\return true if the subscription succeeded; false otherwise
	*/
	bool AutoLoginPlugin::Subscribe()
	{
		return SubscribeService(_T(SYSTEM_MODULE), _T(IE_SERVER_HWND_SERVICE));
	}

	/*! \brief Removes the plugin as a subscriber to the ie server handle service
		\return true if the subscription was revoked successfully; false otherwise
	*/
	bool AutoLoginPlugin::Unsubscribe()
	{
		return UnsubscribeService(_T(SYSTEM_MODULE), _T(IE_SERVER_HWND_SERVICE));
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
												AutoLoginPlugin::Query, Windower::ConfigurablePlugin::Configure);
}