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
#include <PluginFramework.h>
#include <SettingsManager.h>

#include <WindowerCommand.h>
#include <CommandHandler.h>
#include "AutoLoginPlugin.h"
#include "AutoLoginSettings.h"
#include "AutoLogin.h"

namespace Windower
{
	//! \brief AutoLoginPlugin default constructor
	AutoLoginPlugin::AutoLoginPlugin() : CommandHandler(0x18E5F530, "AutoLogin"),
		m_hThread(NULL), m_pSettings(new AutoLoginSettings(_T("config.ini"))) {}

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
	IPlugin* AutoLoginPlugin::Create()
	{
		AutoLoginPlugin* pInstance = new AutoLoginPlugin;
		WindowerCommand *pCommand;

		// register the command
		pCommand = pInstance->RegisterCommand(CMD_CREATE_THREAD, "autologin::create_thread", "");

		if (pCommand != NULL)
			pCommand->AddPointerParam("hwnd");

		return pInstance;
	}

	/*! \brief Destroys an instance of AutoLoginPlugin
		\param[in] pInstance_in : an instance of AutoLoginPlugin
	*/
	void AutoLoginPlugin::Destroy(PluginFramework::IPlugin *pInstance_in)
	{
		if (pInstance_in != NULL)
		{
			// revoke the command
			static_cast<AutoLoginPlugin*>(pInstance_in)->RevokeCommand(CMD_CREATE_THREAD);

			delete pInstance_in;
			pInstance_in = NULL;
		}
	}

	//! \brief Fills a PluginInfo structure with the plugin information
	void AutoLoginPlugin::Query(PluginFramework::PluginInfo& PluginInfo_out)
	{
		PluginInfo_out.SetDesc(_T("This plugin will automatically fill the password field during the login process"));
		PluginInfo_out.SetIdentifier(_T("9D0E9E80-A5CB-11E0-8264-0800200C9A66"));
		PluginInfo_out.SetAuthor(_T("Xebeth`"));
		PluginInfo_out.SetName(_T("AutoLogin"));
		PluginInfo_out.SetVersion(_T("1.0.0"));

		IPlugin::Query(PluginInfo_out);
	}

	/*! \brief Executes the command specified by its ID
		\param[in] CmdID_in : the ID of the command to execute
		\param[in] Command_in : the command to execute
		\param[out] Feedback_out : the result of the execution
		\return true if the command was executed successfully; false otherwise
	*/
	bool AutoLoginPlugin::ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string &Feedback_out)
	{
		switch(CmdID_in)
		{
			case CMD_CREATE_THREAD:
				return CreateThread((HWND)Command_in.GetPointerValue("hwnd"));
		}

		return false;
	}

	/*! \brief Creates the thread used to monitor the forms during the login process
		\param[in] ParentHwnd_in : the handle of the IE server window
		\return true if the thread was created successfully; false otherwise
	*/
	bool AutoLoginPlugin::CreateThread(HWND ParentHwnd_in)
	{
		if (ParentHwnd_in != NULL)
		{
			m_pSettings->SetParentWnd(ParentHwnd_in);

			return (::CreateThread(NULL, 0, ::AutoLoginThread, (LPVOID)m_pSettings, 0, NULL) != NULL);
		}

		return false;
	}
}

using Windower::AutoLoginPlugin;

/*! \brief Function exposed by the plugin DLL to initialize the plugin object
	\param[in] pServices_in : services used to (un)subscribe to services and invoke them
	\return a pointer to the plugin registration parameters if successful; NULL otherwise
*/
extern "C" PLUGIN_API PluginFramework::RegisterParams* InitPlugin(PluginFramework::IPluginServices *pServices_in)
{
	PluginFramework::RegisterParams *pParams = new PluginFramework::RegisterParams;

	PluginFramework::IPlugin::SetServices(pServices_in);
	AutoLoginPlugin::Query(pParams->Info);	

	pParams->QueryFunc = AutoLoginPlugin::Query;
	pParams->CreateFunc = AutoLoginPlugin::Create;
	pParams->DestroyFunc = AutoLoginPlugin::Destroy;

	return pParams;
}