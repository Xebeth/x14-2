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

	//! \brief Plugin services parameters structure
	class ServiceParam
	{
	public:
		//! \brief ServiceParam default constructor
		ServiceParam() : DataType(_T("NULL")), pData(NULL) {}

		/*! \brief ServiceParam constructor
			\param[in] DataType_in : the type of the data
			\param[in] pData_in : a pointer to the data
		*/
		ServiceParam(string_t DataType_in, LPVOID pData_in)
			: DataType(DataType_in), pData(pData_in) {}

		//! the type of the data
		string_t	DataType;
		//! a pointer to the data
		LPVOID		pData;
	};

	//! \brief Plugin services interface
	class IPluginServices
	{
	public:

		/*! \brief IPluginServices constructor
			\param[in] Version_in : the version of the services
		*/
		explicit IPluginServices(const VersionInfo &Version_in) : m_Version(Version_in) {}
		//! \brief IPluginServices destructor
		virtual ~IPluginServices() {}

		/*! \brief Invokes a service by name in the specified module
			\param[in] ModuleName_in : the name of the module
			\param[in] ServiceName_in : the name of the service
			\param[in] Params_in : the parameters of the call
			\return true if the service was invoked successfully; false otherwise
		*/
		virtual bool InvokeService(const string_t &ModuleName_in, const string_t &ServiceName_in,
								   const ServiceParam &Params_in) const =0;
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
		/*! \brief Retrieves the configuration file path
			\return the configuration file path
		*/
		virtual const TCHAR* GetConfigFile() const =0;


		/*! \brief Retrieves the framework version of the plugin services
			\return the framework version of the plugin services
		*/
		const VersionInfo& GetVersion() const { return m_Version; }

		//! the version of the services
		const VersionInfo m_Version;
	};

	//! function pointer to an InvokeService function
	typedef bool (PluginFramework::IPluginServices::*fnInvokeService)(const string_t&, const string_t&,
																	  const PluginFramework::ServiceParam&,
																	  PluginFramework::ServiceParam&) const;
	//! function pointer to a SubscribeService function
	typedef bool (PluginFramework::IPluginServices::*fnSubscribeService)(const string_t&, const string_t&, PluginFramework::IPlugin*) const;
	//! function pointer to an UnsubscribeService function
	typedef bool (PluginFramework::IPluginServices::*fnUnsubscribeService)(const string_t&, const string_t&, PluginFramework::IPlugin*) const;
}

#endif//__I_PLUGIN_SERVICES_H__