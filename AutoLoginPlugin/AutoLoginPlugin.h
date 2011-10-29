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

//! Plugin registration key
#define PLUGIN_REGKEY 0x18E5F530

namespace Windower
{
	class AutoLoginSettings;
	class WindowerCommand;	

	//! \brief Auto login plugin
	class AutoLoginPlugin : public PluginFramework::IPlugin
	{
	public:
		AutoLoginPlugin();
		virtual ~AutoLoginPlugin();

		static PluginFramework::IPlugin* Create();
		static void Destroy(PluginFramework::IPlugin *pInstance_in);
		static void Query(PluginInfo& Info_out);

		static int AutoLoginThread(const WindowerCommand *pCommand_in);

	protected:
		//! AutoLogin plugin settings
		static AutoLoginSettings *m_pSettings;
	};
}

#endif//__AUTO_LOGIN_PLUGIN_H__