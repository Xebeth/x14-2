/**************************************************************************
	created		:	2013-05-04
	filename	: 	ConfigurablePlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Plugin with configuration screen
**************************************************************************/
#ifndef _WINDOWER_PLUGIN_SDK_INCLUDE_
	#error Please include the global header file 'PluginSDK.h'
#endif

#ifndef __CONFIGURABLE_PLUGIN_H__
#define __CONFIGURABLE_PLUGIN_H__

namespace Windower
{
	class PluginPropertySheet;
	class PluginPropertyPage;
	class PluginSettings;

	//! \brief Plugin with configuration screen
	class ConfigurablePlugin : public PluginFramework::IPlugin
	{
	public:
		/*! \brief IPlugin constructor
			\param[in] pServices_in : a pointer to the plugin services
		*/
		explicit ConfigurablePlugin(PluginFramework::IPluginServices *pServices_in);
		~ConfigurablePlugin();

		static bool Configure(PluginFramework::IPlugin *pInstance_in, const LPVOID pUserData_in);

		/*! \brief Retrieves the property page used to configure the plugin
			\return a pointer to the property page of the plugin
		*/
		virtual PluginPropertyPage* GetPropertyPage() =0;
		//! \brief Callback function invoked when the settings have changed
		virtual void OnSettingsChanged() =0;

	protected:
		//! configuration page
		PluginPropertyPage *m_pConfigPage;
		//! configuration dialog
		PluginPropertySheet *m_pConfigDlg;
		//! settings of the plugin
		PluginSettings *m_pSettings;
	};
}

#endif//__CONFIGURABLEPLUGIN_H__
