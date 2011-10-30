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

	//! \brief Plugin services parameters
	class ServiceParam
	{
	public:
		//! \brief ServiceParam default constructor
		ServiceParam() : DataType(_T("NULL")), pData(NULL) {}

		/*! \brief ServiceParam constructor
			\param[in] DataType_in : the type of the data
			\param[in] pData_in : a pointer to the data
		*/
		ServiceParam(string_t DataType_in, void *pData_in)
		{
			DataType = DataType_in;
			pData = pData_in;
		}
		//! the type of the data
		string_t	DataType;
		//! a pointer to the data
		void*		pData;
	};

	//! \brief Plugin services interface
	class IPluginServices
	{
	public:

		/*! \brief IPluginServices constructor
			\param[in] pVersion_in : a string containing the version of the services
		*/
		explicit IPluginServices(const TCHAR *pVersion_in) : m_Version(pVersion_in) {}
		//! \brief IPluginServices destructor
		virtual ~IPluginServices() {}

		/*! \brief Invokes a service by name in the specified module
			\param[in] ModuleName_in : the name of the module
			\param[in] ServiceName_in : the name of the service
			\param[in] Params_in : the parameters of the call
			\param[out] Results_out : the result of the call
			\return true if the service was invoked successfully; false otherwise
		*/
		virtual bool InvokeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
								   const ServiceParam &Params_in, ServiceParam &Results_out) const =0;
		/*! \brief Subscribes to a service by name in the specified module
			\param[in] ModuleName_in : the name of the module
			\param[in] ServiceName_in : the name of the service
			\param[in] pPlugin_in : the plugin subscribing to the service
			\return true if the subscribtion was successful; false otherwise
		*/
		virtual bool SubscribeService(const string_t &ModuleName_in,
									  const string_t &ServiceName_in,
									  IPlugin* pPlugin_in) const =0;
		/*! \brief Revokes the subscription from a service by name in the specified module
			\param[in] ModuleName_in : the name of the module
			\param[in] ServiceName_in : the name of the service
			\param[in] pPlugin_in : the plugin revoking its subscription from the service
			\return true if the subscribtion was revoked successfully; false otherwise
		*/
		virtual bool UnsubscribeService(const string_t &ModuleName_in,
										const string_t &ServiceName_in,
										IPlugin* pPlugin_in) const =0;

		//! the version of the services
		const PluginVersion m_Version;
	};
}

#endif//__I_PLUGIN_SERVICES_H__