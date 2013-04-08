/**************************************************************************
	created		:	2012-07-07
	filename	: 	WindowerPlugin.h
	author		:	Xebeth`
	copyright	:	North-Edge (2012)
	purpose		:	
**************************************************************************/
#ifndef __WINDOWER_PLUGIN_H__
#define __WINDOWER_PLUGIN_H__

#ifndef _WINDOWER_PLUGIN_SDK_INCLUDE_
	#error Please include the global header file 'PluginSDK.h'
#endif

namespace Windower
{
	class WindowerPlugin : public PluginFramework::IPlugin
	{
	public:
		/*! \brief WindowerPlugin constructor
			\param[in] pServices_in : a pointer to the plugin services
		*/
		explicit WindowerPlugin(PluginFramework::IPluginServices *pServices_in)
			: PluginFramework::IPlugin(pServices_in) {}

		/*! \brief Update method to be overloaded by plugins
				   This method will be called in the main loop of the windower engine
			\return true if any computation was performed; false otherwise
		*/
		virtual bool Update() { return false; }
	};
}

#endif//__WINDOWER_PLUGIN_H__
