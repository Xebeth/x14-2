/**************************************************************************
	created		:	2011-07-03
	filename	: 	IPluginServices.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin services interface
**************************************************************************/
#ifndef __I_PLUGIN_SERVICES_H__
#define __I_PLUGIN_SERVICES_H__

#ifndef _PLUGIN_FRAMEWORK_SDK_INCLUDE_
	#error Please include the global header file 'PluginFramework.h'
#endif

namespace PluginFramework
{
	class IUserSettings;
	class IPlugin;

	//! \brief Plugin services parameters structure
	class ServiceParam
	{
	public:
		//! \brief ServiceParam default constructor
		ServiceParam();

		/*! \brief ServiceParam constructor
			\param[in] DataType_in : the type of the data
			\param[in] pData_in : a pointer to the data
		*/
		ServiceParam(string_t DataType_in, LPVOID pData_in);

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
		explicit IPluginServices(const VersionInfo &Version_in);
		//! \brief IPluginServices destructor
		virtual ~IPluginServices();

		/*! \brief Invokes a service by name in the specified module
			\param[in] ModuleName_in : the name of the module
			\param[in] ServiceName_in : the name of the service
			\param[in] Params_in : the parameters of the call
			\return true if the service was invoked successfully; false otherwise
		*/
		virtual bool InvokeService(const string_t &ModuleName_in,
								   const string_t &ServiceName_in,
								   ServiceParam &Params_in) const =0;
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

		virtual bool LoadSettings(IUserSettings* pSettings_out) const =0;
		virtual bool SaveSettings(const IUserSettings *pSettings_in) =0;
		virtual const TCHAR* GetWorkingDir() const =0;

		/*! \brief Retrieves the framework version of the plugin services
			\return the framework version of the plugin services
		*/
		const VersionInfo& GetVersion() const;

	protected:
		//! the version of the services
		const VersionInfo m_Version;
	};
}

#endif//__I_PLUGIN_SERVICES_H__