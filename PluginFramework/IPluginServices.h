/**************************************************************************
	created		:	2011-07-03
	filename	: 	IPluginServices.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin services interface
**************************************************************************/
#ifndef __I_PLUGIN_SERVICES_H__
#define __I_PLUGIN_SERVICES_H__

namespace PluginFramework
{
	class IPlugin;

	class ServiceParam
	{
	public:
		ServiceParam() : DataType(_T("NULL")), pData(NULL) {}

		ServiceParam(string_t DataType_in, void *pData_in)
		{
			DataType = DataType_in;
			pData = pData_in;
		}

		string_t	DataType;
		void*		pData;
	};

	class IPluginServices
	{
	public:
		IPluginServices(const TCHAR *pVersion_in) : m_Version(pVersion_in) {}
		virtual ~IPluginServices() {}

		virtual bool InvokeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
								   const ServiceParam &Params_in, ServiceParam &Results_out) const =0;
		virtual bool SubscribeService(const string_t &ModuleName_in,
									  const string_t &ServiceName_in,
									  IPlugin* pPlugin_in) const =0;
		virtual bool UnsubscribeService(const string_t &ModuleName_in,
										const string_t &ServiceName_in,
										IPlugin* pPlugin_in) const =0;
		PluginVersion m_Version;
	};
}

#endif//__I_PLUGIN_SERVICES_H__