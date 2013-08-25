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

namespace Windower
{
	class PluginPropertyPage;
	class WindowerProfile;
}

namespace Bootstrap
{
	//! \brief Auto login plugin
	class AutoLoginPlugin : public Windower::ConfigurablePlugin
	{
		//! IDs of the commands registered with the plugin
		enum CommandMap
		{
			CMD_CREATE_THREAD = 0,	//!< creates the thread to monitor the forms
			CMD_COUNT				//!< number of registered commands
		};

	public:
		explicit AutoLoginPlugin(PluginFramework::IPluginServices *pServices_in);

		bool CreateAutoLoginThread(HWND ParentHwnd_in);

		static PluginFramework::IPlugin* Create(PluginFramework::IPluginServices *pServices_in);
		static void Query(PluginFramework::PluginInfo& PluginInfo_out);		
		static void Destroy(PluginFramework::IPlugin *pInstance_in);

		bool Unsubscribe();
		bool Subscribe();

	protected:
		Windower::PluginPropertyPage* GetPropertyPage();
		void OnSettingsChanged();

		ThreadData m_ThreadData;
	};
}

#endif//__AUTO_LOGIN_PLUGIN_H__