/**************************************************************************
	created		:	2011-05-28
	filename	: 	IPlugin.h
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin interface
**************************************************************************/
#ifndef __IPLUGIN_H__
#define __IPLUGIN_H__

namespace PluginFramework
{
	/*! \brief Base plugin interface */
	class IPlugin
	{
	public:
		//! \brief IPlugin destructor
		virtual ~IPlugin() {}

		/*! \brief Sets the services for all instances of the plugin
			\param[in] pPluginServices_in : plugin services
		*/
		static void SetPluginServices(const PluginFramework::IPluginServices &PluginServices_in)
			{ m_pPluginServices = &PluginServices_in; }

		/*! \brief Retrieves the plugin information (version, etc.)
			\return the plugin information
		*/
		const PluginInfo& GetInfo() { return m_PluginInfo; }
		/*! \brief Sets the plugin information (version, etc.)
			\param[in] PluginInfo_in : the plugin information
		*/
		void SetInfo(const PluginInfo &PluginInfo_in) { m_PluginInfo = PluginInfo_in; }

	protected:
		//! the plugin services
		static const PluginFramework::IPluginServices *m_pPluginServices;
		//! the plugin information (version, etc.)
		PluginInfo m_PluginInfo;
	};
}

#endif//__IPLUGIN_H__