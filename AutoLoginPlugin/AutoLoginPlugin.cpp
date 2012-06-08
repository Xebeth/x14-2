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

#include <PluginFramework.h>
#include <SettingsManager.h>

#include <PluginEngine.h>
#include <WindowerCommand.h>
#include <CommandHandler.h>

#include "AutoLoginConfigDlg.h"
#include "AutoLoginPlugin.h"
#include "AutoLoginSettings.h"
#include "AutoLogin.h"
#include "version.h"

namespace Windower
{
	//! \brief AutoLoginPlugin default constructor
	AutoLoginPlugin::AutoLoginPlugin(PluginFramework::IPluginServices *pServices_in)
		: CommandHandler(0x18E5F530, "AutoLogin"), PluginFramework::IPlugin(pServices_in),
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
	IPlugin* AutoLoginPlugin::Create(PluginFramework::IPluginServices *pServices_in)
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
		PluginInfo_out.SetCompatibilityFlags(PLUGIN_COMPATIBILITY_BOOTSTRAP);
		PluginInfo_out.SetVersion(PLUGIN_VERSION);
		PluginInfo_out.SetAuthor(_T("Xebeth`"));
		PluginInfo_out.SetName(_T("AutoLogin"));		
	}
	
	/*! \brief Opens the configuration screen of the plugin
		\param[out] pInstance_in : the instance of the plugin to configure
		\return true if the user validated the configuration screen; false otherwise
	*/
	bool AutoLoginPlugin::Configure(PluginFramework::IPlugin *pInstance_in)
	{
		AutoLoginConfigDlg ConfigDlg;

		return (ConfigDlg.DoModal() == IDOK);
	}

	/*! \brief Registers the commands of the plugin with the command dispatcher
		\return true if all the commands were registered successfully; false otherwise
	*/
	bool AutoLoginPlugin::RegisterCommands()
	{
		// register the command
		WindowerCommand *pCommand = RegisterCommand(CMD_CREATE_THREAD, "autologin::create_thread", "");

		if (pCommand != NULL && pCommand->AddPointerParam("hwnd") == false)
		{
			delete pCommand;
			pCommand = NULL;
		}

		return (pCommand != NULL);
	}

	/*! \brief Unregisters the commands of the plugin with the command dispatcher
		\return true if all the commands were unregistered successfully; false otherwise
	*/
	bool AutoLoginPlugin::UnregisterCommands()
	{
		return RevokeCommand(CMD_CREATE_THREAD);
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
	\return a pointer to the plugin registration parameters if successful; NULL otherwise
*/
extern "C" PLUGIN_API PluginFramework::RegisterParams* InitPlugin()
{
	return PluginFramework::IPlugin::Initialize(AutoLoginPlugin::Create, AutoLoginPlugin::Destroy,
												AutoLoginPlugin::Query, AutoLoginPlugin::Configure);
}