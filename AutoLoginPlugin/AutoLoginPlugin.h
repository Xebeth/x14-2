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

// module name
#define SYSTEM_MODULE			"System"
// available services for the module
#define IE_SERVER_HWND_SERVICE	"IEServerHWND"

namespace Bootstrap
{
	class AutoLoginSettings;

	//! \brief Auto login plugin
	class AutoLoginPlugin : public PluginFramework::IPlugin
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

		bool CreateAutoLoginThread(HWND ParentHwnd_in);

		static bool Configure(PluginFramework::IPlugin *pInstance_in, const LPVOID pUserData_in);
		static PluginFramework::IPlugin* Create(PluginFramework::IPluginServices *pServices_in);
		static void Query(PluginFramework::PluginInfo& PluginInfo_out);		
		static void Destroy(PluginFramework::IPlugin *pInstance_in);

		AutoLoginSettings& GetSettings();

		bool Unsubscribe();
		bool Subscribe();
		
	protected:
		//! AutoLogin plugin settings
		AutoLoginSettings *m_pSettings;
	};
}

#endif//__AUTO_LOGIN_PLUGIN_H__