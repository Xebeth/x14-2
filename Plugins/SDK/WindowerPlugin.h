/**************************************************************************
	created		:	2012-07-07
	filename	: 	WindowerPlugin.h
	author		:	Xebeth`
	copyright	:	North-Edge (2012)
	purpose		:	Windower plugin base class
**************************************************************************/
#ifndef _WINDOWER_PLUGIN_SDK_INCLUDE_
	#error Please include the global header file 'PluginSDK.h'
#endif

#ifndef __WINDOWER_PLUGIN_H__
#define __WINDOWER_PLUGIN_H__

namespace Windower
{
	class PluginPropertyPage;

	//! \brief Windower plugin base class
	class WindowerPlugin : public ConfigurablePlugin
	{
	public:
		explicit WindowerPlugin(PluginFramework::IPluginServices *pServices_in);

		virtual bool Update();

	protected:
		virtual PluginPropertyPage* GetPropertyPage();
		virtual void OnSettingsChanged();

		virtual bool UnregisterCommands();
		virtual bool RegisterCommands();

	private:
		void OnDestroy();
		void OnCreate();
	};
}

#endif//__WINDOWER_PLUGIN_H__
