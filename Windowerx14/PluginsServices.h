/**************************************************************************
	created		:	2011-07-03
	filename	: 	PluginServices.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#ifndef __PLUGIN_SERVICES_H__
#define __PLUGIN_SERVICES_H__

namespace Windower
{
	class ICoreModule;

	typedef stdext::hash_map<string_t, ICoreModule*> CoreModules;

	class PluginServices : public PluginFramework::IPluginServices
	{
	public:
		PluginServices(const TCHAR *pVersion_in, CoreModules &Modules_in)
			: IPluginServices(pVersion_in), m_Modules(Modules_in) {}

		virtual bool InvokeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
								   const PluginFramework::ServiceParam &Params_in,
								   PluginFramework::ServiceParam &Results_out) const;
		virtual bool SubscribeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
								      PluginFramework::IPlugin* pPlugin_in) const;
		virtual bool UnsubscribeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
										PluginFramework::IPlugin* pPlugin_in) const;

	protected:
		const CoreModules &m_Modules;
	};
}

#endif//__PLUGIN_SERVICES_H__