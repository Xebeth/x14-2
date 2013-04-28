/**************************************************************************
	created		:	2011-05-28
	filename	: 	IPlugin.h
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin interface
**************************************************************************/
#ifndef __IPLUGIN_H__
#define __IPLUGIN_H__

#ifndef _PLUGIN_FRAMEWORK_SDK_INCLUDE_
	#error Please include the global header file 'PluginFramework.h'
#endif

#ifndef PLUGIN_EXPORTS
	#define PLUGIN_API __declspec(dllimport)
#else
	#define PLUGIN_API __declspec(dllexport)
#endif

// forward declarations
namespace PluginFramework
{
	class IPluginServices;
	class RegisterParams;
	class ServiceParam;
	class VersionInfo;
	class IPlugin;
}

//! a pointer to the 'Create' function of the plugin
typedef PluginFramework::IPlugin* (*fnCreate)(PluginFramework::IPluginServices *pServices_in);
//! a pointer to the 'Destroy' function of the plugin
typedef void (*fnDestroy)(PluginFramework::IPlugin*);
//! a function pointer to a Query function
typedef void (*fnQuery)(PluginFramework::PluginInfo&);
//! a function pointer to a Configure function
typedef bool (*fnConfigure)(PluginFramework::IPlugin*, const LPVOID);

namespace PluginFramework
{
	//! \brief Plugin registration structure
	class RegisterParams
	{
	public:
		RegisterParams();
		//! Plugin information
		PluginInfo  Info;
		//! plugin 'Query' function pointer
		fnQuery		QueryFunc;
		//! plugin 'Create' function pointer
		fnCreate	CreateFunc;
		//! plugin 'Destroy' function pointer
		fnDestroy	DestroyFunc;
		//! plugin 'Configure' function pointer
		fnConfigure ConfigureFunc;
	};

	/*! function pointer to an Initialize function
		\param[out] RegisterParams_out : Registration structure to be able to use the plugin
		\return true if the initialization succeeded; false otherwise
	*/
	typedef bool (*fnInitialize)(RegisterParams &RegisterParams_out);

	/*! \brief Base plugin interface */
	class IPlugin
	{
		friend class PluginManager;
	public:
		/*! \brief IPlugin constructor
			\param[in] pServices_in : a pointer to the plugin services
		*/
		explicit IPlugin(IPluginServices *pServices_in);
		//! \brief IPlugin destructor
		virtual ~IPlugin();

		/*! \brief Retrieves the framework version against which the plugin was compiled
			\return the framework version
		*/
		const VersionInfo& GetFrameworkVersion() const;

		/*! \brief Retrieves the version of the plugin
			\return the version of the plugin
		*/
		const VersionInfo& GetVersion() const;

		/*! \brief Retrieves the URL used to check for updates
			\return the URL used to check for updates
		*/
		const string_t& GetUpdateURL() const;

		/*! \brief Retrieves the path of the plugin DLL
			\return the path of the plugin DLL
		*/
		const string_t& GetDLLPath() const;

		/*! \brief Retrieves the author of the plugin
			\return the author of the plugin
		*/
		const string_t& GetAuthor() const;

		/*! \brief Retrieves the StringUtils::UUID of the plugin
			\return the StringUtils::UUID of the plugin
		*/
		const StringUtils::UUID& GetUUID() const;

		/*! \brief Retrieves the name of the plugin
			\return the name of the plugin
		*/
		const string_t& GetName() const;

		/*! \brief Retrieves the description of the plugin
			\return the description of the plugin
		*/
		const string_t& GetDesc() const;

		/*! \brief Retrieves the compatibility flags of the plugin
			\return the compatibility flags of the plugin
		*/
		DWORD GetCompatibilityFlags() const;

		/*! \brief Checks if the plugin has been initialized properly
			\return true if the plugin is initialized; false otherwise
		*/
		bool IsInitialized() const;

		/*! \brief Checks if the plugin has a configuration screen
			\return true if the plugin has a configuration screen; false otherwise
		*/
		bool IsConfigurable() const;

		/*! \brief Retrieves the handle of the plugin DLL once loaded
			\return the handle of the plugin
		*/
		HMODULE GetHandle() const;

		/*! \brief Converts the plugin information to a string
			\return a string representation of the plugin information
		*/
		string_t ToString() const;

		/*! \brief Sets the URL used to check for updates
			\param[in] UpdateURL_in : the URL used to check for updates
		*/
		void SetUpdateURL(const string_t &UpdateURL_in);

		/*! \brief Sets the description of the plugin
			\param[in] Description_in : the description of the plugin
		*/
		void SetDesc(const string_t &Description_in);

		/*! \brief Sets the author of the plugin
			\param[in] Author_in : the author of the plugin
		*/
		void SetAuthor(const string_t &Author_in);

		/*! \brief Sets the version of the plugin
			\param[in] Version_in : the version of the plugin
		*/
		void SetVersion(const string_t &Version_in);

		/*! \brief Sets the name of the plugin
			\param[in] Name_in : the name of the plugin
		*/
		void SetName(const string_t &Name_in);
		
		/*! \brief Displays the configuration screen of the plugin
			\param[in] pUserData_in : a pointer to the user data to pass to the plugin
			\return true if the user validated the screen; false otherwise
		*/
		bool Configure(const LPVOID pUserData_in);

		/*! \brief Cleans up the specified string to make it a valid plugin name
			\param[in,out] Name_in_out : the string to clean up
		*/
		static void CleanupName(string_t Name_in_out);
		/*! \brief Initializes the plugin
			\param[out] RegisterParams_out : Registration structure to be able to use the plugin
			\param[in] pfnCreateFunc_in : a pointer to the 'Create' function of the plugin
			\param[in] pfnDestroyFunc_in : a pointer to the 'Destroy' function of the plugin
			\param[in] pfnQueryFunc_in : a pointer to the 'Query' function of the plugin
			\param[in] pfnConfigureFunc_in : a pointer to the 'Configure' function of the plugin
			\return true if the initialization succeeded; false otherwise
		*/
		static bool Initialize(RegisterParams &RegisterParams_out, fnCreate pfnCreateFunc_in, 
							   fnDestroy pfnDestroyFunc_in, fnQuery pfnQueryFunc_in,
							   fnConfigure pfnConfigureFunc_in = NULL);

		//! \brief Fills a PluginInfo structure with the plugin information
		static void Query(PluginFramework::PluginInfo& PluginInfo_out);
		/*! \brief Invokes a command registered with the service in the specified module
			\param[in] ModuleName_in : the name of the module
			\param[in] ServiceName_in : the name of the service
			\param[in] Params_in : the input parameters
			\return true if the command was invoked successfully; false otherwise
		*/
		static bool InvokeService(const string_t &ModuleName_in, 
								  const string_t &ServiceName_in,
								  ServiceParam &Params_in);

		static const TCHAR* GetConfigFile();

	protected:
		/*! \brief Removes a plugin subscription from the service in the specified module
			\param[in] ModuleName_in : the name of the module
			\param[in] ServiceName_in : the name of the service
			\return true if successful; false otherwise
		*/
		bool UnsubscribeService(const string_t &ModuleName_in, const string_t &ServiceName_in);
		/*! \brief Adds a plugin subscription to the service in the specified module
			\param[in] ModuleName_in : the name of the module
			\param[in] ServiceName_in : the name of the service
			\param[in] pPlugin_in : the plugin subscribing to the service
			\return true if successful; false otherwise
		*/
		bool SubscribeService(const string_t &ModuleName_in, const string_t &ServiceName_in);

		/*! \brief Removes the plugin as a subscriber to plugin services
			\return true if the subscriptions were revoked successfully; false otherwise
		*/
		virtual bool Unsubscribe();
		/*! \brief Adds the plugin as a subscriber to plugin services
			\return true if the subscriptions succeeded; false otherwise
		*/
		virtual bool Subscribe();

		//! Callback function invoked when a new instance of the plugin has been created
		virtual void OnCreate();
		//! Callback function invoked when a new instance of the plugin is about to be destroyed
		virtual void OnDestroy();

		//! the plugin information (version, name, author, etc.)
		PluginInfo m_PluginInfo;

	private:
		//! plugin services used to (un)subscribe to services and invoke them
		static IPluginServices *m_pServices;
		//! register parameters
		RegisterParams *m_pRegisterParams;
	};
}

extern "C"
{
	/*! \brief Function exposed by the plugin DLL to initialize the plugin object
		\param[out] RegisterParams_out : Registration structure to be able to use the plugin
		\return true if the initialization succeeded; false otherwise
	*/
	PLUGIN_API bool InitPlugin(PluginFramework::RegisterParams &RegisterParams_out);
}

#endif//__IPLUGIN_H__