/**************************************************************************
	created		:	2011-06-04
	filename	: 	AutoLoginPlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Auto login plugin
					Monitors the forms during the login process
					and automatically fills the password field
**************************************************************************/
#ifndef __AUTO_LOGIN_PLUGIN_H__
#define __AUTO_LOGIN_PLUGIN_H__

namespace Windower
{
	class AutoLoginSettings;

	//! \brief Auto login plugin
	class AutoLoginPlugin : public PluginFramework::IPlugin, public CommandHandler
	{
		//! IDs of the commands registered with the plugin
		enum CommandMap
		{
			CMD_CREATE_THREAD = 0,	//!< creates the thread to monitor the forms
			CMD_COUNT				//!< number of registered commands
		};

	public:
		explicit AutoLoginPlugin(PluginFramework::IPluginServices *pServices_in);
		virtual ~AutoLoginPlugin();

		virtual bool ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string &Feedback_out);
		bool CreateThread(HWND ParentHwnd_in);

		static PluginFramework::IPlugin* Create(PluginFramework::IPluginServices *pServices_in);
		static void Query(PluginFramework::PluginInfo& PluginInfo_out);
		static bool Configure(PluginFramework::IPlugin *pInstance_in);
		static void Destroy(PluginFramework::IPlugin *pInstance_in);
		
	protected:
		bool UnregisterCommands();
		bool RegisterCommands();

		//! AutoLogin plugin settings
		AutoLoginSettings *m_pSettings;
		//! the handle of the thread used to monitor the forms during the login process
		HANDLE m_hThread;
	};
}

#endif//__AUTO_LOGIN_PLUGIN_H__