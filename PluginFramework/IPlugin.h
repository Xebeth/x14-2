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
		virtual ~IPlugin() {}

		static void SetPluginServices(const PluginFramework::IPluginServices *pPluginServices_in)
			{ m_pPluginServices = pPluginServices_in; }

		const PluginInfo& GetInfo() { return m_BasePluginInfo; }
		void SetInfo(const PluginInfo &PluginInfo_in) { m_BasePluginInfo = PluginInfo_in; }

	protected:
		static const PluginFramework::IPluginServices *m_pPluginServices;
		PluginInfo m_BasePluginInfo;
	};
}

#endif//__IPLUGIN_H__