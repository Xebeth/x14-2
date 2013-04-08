/**************************************************************************
	created		:	2013-04-08
	filename	: 	IAutoLoginPlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Autologin plugin interface
**************************************************************************/
#ifndef __I_AUTOLOGIN_PLUGIN_H__
#define __I_AUTOLOGIN_PLUGIN_H__

namespace Bootstrap
{
	class IAutoLoginPlugin : public PluginFramework::IPlugin
	{
	public:
		/*! \brief IPlayerDataPlugin constructor
			\param[in] pServices_in : a pointer to the plugin services
		*/
		explicit IAutoLoginPlugin(PluginFramework::IPluginServices *pServices_in)
			: PluginFramework::IPlugin(pServices_in) {}

		virtual bool CreateAutoLoginThread(HWND hParentWnd_in) =0;

	};
}

#endif//__I_AUTOLOGIN_PLUGIN_H__
