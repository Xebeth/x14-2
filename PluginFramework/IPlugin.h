/**************************************************************************
	created		:	2011-05-28
	filename	: 	IPlugin.h
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin interface
**************************************************************************/
#ifndef __IPLUGIN_H__
#define __IPLUGIN_H__

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
	class PluginInfo;
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
	//! \brief Plugin information structure
	class PluginInfo
	{
		friend class PluginManager;
		friend class IPlugin;
	public:
		//! \brief PluginInfo constructor
		PluginInfo();

		/*! \brief Sets the URL used to check for updates
			\param[in] UpdateURL_in : the URL used to check for updates
		*/
		void SetUpdateURL(const string_t &UpdateURL_in) { m_UpdateURL = UpdateURL_in; }
		/*! \brief Retrieves the URL used to check for updates
			\return the URL used to check for updates
		*/
		const string_t& GetUpdateURL() const { return m_UpdateURL; }
		/*! \brief Sets the description of the plugin
			\param[in] Descritpion_in : the description of the plugin
		*/
		void SetDesc(const string_t &Descritpion_in) { m_Descritpion = Descritpion_in; }
		/*! \brief Retrieves the description of the plugin
			\return the description of the plugin
		*/
		const string_t& GetDesc() const { return m_Descritpion; }
		/*! \brief Sets the author of the plugin
			\param[in] Author_in : the author of the plugin
		*/
		void SetAuthor(const string_t &Author_in) { m_Author = Author_in; }
		/*! \brief Retrieves the author of the plugin
			\return the author of the plugin
		*/
		const string_t& GetAuthor() const { return m_Author; }
		/*! \brief Sets the identifier of the plugin
			\param[in] UUID_in : the identifier of the plugin
		*/
		void SetIdentifier(const string_t &UUID_in) { m_PluginIdentifier.FromString(UUID_in.c_str()); }
		/*! \brief Retrieves the identifier of the plugin
			\return the identifier of the plugin
		*/
		string_t GetIndentifier() const { return m_PluginIdentifier.ToString(); }
		/*! \brief Retrieves the version of the plugin
			\return the version of the plugin
		*/
		string_t GetVersion() const { return m_Version.ToString(); }
		/*! \brief Sets the version of the plugin
			\param[in] Version_in : the version of the plugin
		*/
		void SetVersion(const string_t &Version_in) { m_Version.FromString(Version_in.c_str()); }
		/*! \brief Sets the name of the plugin
			\param[in] Name_in : the name of the plugin
		*/
		void SetName(const string_t &Name_in) { m_Name = Name_in; }
		/*! \brief Retrieves the name of the plugin
			\return the name of the plugin
		*/
		const string_t& GetName() const { return m_Name; }
		/*! \brief Sets the compatibility flags of the plugin
			\param[in] Name_in : the compatibility flags of the plugin
		*/
		void SetCompatibilityFlags(DWORD CompatibilityFlags_in) { m_CompatibilityFlags = CompatibilityFlags_in; }
		/*! \brief Retrieves the compatibility flags of the plugin
			\return the compatibility flags of the plugin
		*/
		DWORD GetCompatibilityFlags() const { return m_CompatibilityFlags; }

		/*! \brief Retrieves the compatible framework version
			\return the compatible framework version
		*/
		string_t GetFrameworkVersion() const { return m_FrameworkVersion.ToString(); }
		/*! \brief Retrieves the path of the DLL
			\return the path of the DLL
		*/
		const string_t& GetDLLPath() const { return m_DLLPath; }

		/*! \brief Retrieves the string representation of the plugin info
			\return the string representation of the plugin info
		*/
		string_t ToString() const;

	protected:
		//! the name of the plugin
		string_t m_Name;
		//! the author of the plugin
		string_t m_Author;
		//! the description of the plugin
		string_t m_Descritpion;
		//! the URL of the update information
		string_t m_UpdateURL;
		//! the compatibility flags of the plugin
		DWORD m_CompatibilityFlags;
		//! the version of the plugin
		VersionInfo m_Version;

	private:
		//! flag specifying if the plugin information has been initialized
		bool m_Initialized;
		//! the plugin UUID
		PluginUUID m_PluginIdentifier;
		//! the compatible framework version
		VersionInfo m_FrameworkVersion;
		//! the handle to the DLL
		HMODULE m_hHandle;
		//! the path of the DLL
		string_t m_DLLPath;
	};

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
		const VersionInfo& GetFrameworkVersion() const { return m_PluginInfo.m_FrameworkVersion; }

		/*! \brief Retrieves the version of the plugin
			\return the version of the plugin
		*/
		const VersionInfo& GetVersion() const { return m_PluginInfo.m_Version; }

		/*! \brief Retrieves the URL used to check for updates
			\return the URL used to check for updates
		*/
		const string_t& GetUpdateURL() const { return m_PluginInfo.m_UpdateURL; }

		/*! \brief Retrieves the path of the plugin DLL
			\return the path of the plugin DLL
		*/
		const string_t& GetDLLPath() const { return m_PluginInfo.m_DLLPath; }

		/*! \brief Retrieves the author of the plugin
			\return the author of the plugin
		*/
		const string_t& GetAuthor() const { return m_PluginInfo.m_Author; }

		/*! \brief Retrieves the UUID of the plugin
			\return the UUID of the plugin
		*/
		const PluginUUID& GetUUID() const { return m_PluginInfo.m_PluginIdentifier; }

		/*! \brief Retrieves the name of the plugin
			\return the name of the plugin
		*/
		const string_t& GetName() const { return m_PluginInfo.m_Name; }

		/*! \brief Retrieves the description of the plugin
			\return the description of the plugin
		*/
		const string_t& GetDesc() const { return m_PluginInfo.m_Descritpion; }

		/*! \brief Retrieves the compatibility flags of the plugin
			\return the compatibility flags of the plugin
		*/
		DWORD GetCompatibilityFlags() const { return m_PluginInfo.m_CompatibilityFlags; }

		/*! \brief Checks if the plugin has been initialized properly
			\return true if the plugin is initialized; false otherwise
		*/
		bool IsInitialized() const { return m_PluginInfo.m_Initialized; }

		/*! \brief Checks if the plugin has a configuration screen
			\return true if the plugin has a configuration screen; false otherwise
		*/
		bool IsConfigurable() const { return (m_pRegisterParams != NULL && m_pRegisterParams->ConfigureFunc != NULL); }

		/*! \brief Retrieves the handle of the plugin DLL once loaded
			\return the handle of the plugin
		*/
		HMODULE GetHandle() const { return m_PluginInfo.m_hHandle; }

		/*! \brief Converts the plugin information to a string
			\return a string representation of the plugin information
		*/
		string_t ToString() const;

		/*! \brief Sets the URL used to check for updates
			\param[in] UpdateURL_in : the URL used to check for updates
		*/
		void SetUpdateURL(const string_t &UpdateURL_in) { m_PluginInfo.m_UpdateURL = UpdateURL_in; }

		/*! \brief Sets the description of the plugin
			\param[in] Descritpion_in : the description of the plugin
		*/
		void SetDesc(const string_t &Descritpion_in) { m_PluginInfo.m_Descritpion = Descritpion_in; }

		/*! \brief Sets the author of the plugin
			\param[in] Author_in : the author of the plugin
		*/
		void SetAuthor(const string_t &Author_in) { m_PluginInfo.m_Author = Author_in; }

		/*! \brief Sets the version of the plugin
			\param[in] Version_in : the version of the plugin
		*/
		void SetVersion(const string_t &Version_in) { m_PluginInfo.m_Version.FromString(Version_in.c_str()); }

		/*! \brief Sets the name of the plugin
			\param[in] Name_in : the name of the plugin
		*/
		void SetName(const string_t &Name_in) { m_PluginInfo.m_Name = Name_in; }
		
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
								  const ServiceParam &Params_in);

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
		virtual bool Unsubscribe() { return true; }
		/*! \brief Adds the plugin as a subscriber to plugin services
			\return true if the subscriptions succeeded; false otherwise
		*/
		virtual bool Subscribe() { return true; }
		/*! \brief Unregisters the commands of the plugin with the command dispatcher
			\return true if all the commands were unregistered successfully; false otherwise
		*/
		virtual bool UnregisterCommands() { return true; }
		/*! \brief Registers the commands of the plugin with the command dispatcher
			\return true if all the commands were registered successfully; false otherwise
		*/
		virtual bool RegisterCommands() { return true; }

		//! the plugin information (version, name, author, etc.)
		PluginInfo m_PluginInfo;

	private:
		//! Callback function invoked when a new instance of the plugin has been created
		void OnCreate();
		//! Callback function invoked when a new instance of the plugin is about to be destroyed
		void OnDestroy();

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