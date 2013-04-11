/**************************************************************************
	created		:	2012-07-07
	filename	: 	WindowerPlugin.h
	author		:	Xebeth`
	copyright	:	North-Edge (2012)
	purpose		:	Windower plugin base class
**************************************************************************/
#ifndef __WINDOWER_PLUGIN_H__
#define __WINDOWER_PLUGIN_H__

#ifndef _WINDOWER_PLUGIN_SDK_INCLUDE_
	#error Please include the global header file 'PluginSDK.h'
#endif

namespace Windower
{
	//! \brief Windower plugin base class
	class WindowerPlugin : public PluginFramework::IPlugin
	{
	public:
		explicit WindowerPlugin(PluginFramework::IPluginServices *pServices_in);

		virtual bool Update();
	};
}

#endif//__WINDOWER_PLUGIN_H__
