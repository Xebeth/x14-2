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
	class VersionInfo;
	class PluginInfo;
	class IPlugin;
}

//! a pointer to the 'Create' function of the plugin
typedef PluginFramework::IPlugin* (*fnCreate)();
//! a pointer to the 'Destroy' function of the plugin
typedef void (*fnDestroy)(PluginFramework::IPlugin*);
//! a function pointer to a Query function
typedef void (*fnQuery)(PluginFramework::PluginInfo&);

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
		void SetUpdateURL(const string_t &UpdateURL_in) { UpdateURL = UpdateURL_in; }
		/*! \brief Sets the description of the plugin
			\param[in] Descritpion_in : the description of the plugin
		*/
		void SetDesc(const string_t &Descritpion_in) { Descritpion = Descritpion_in; }
		/*! \brief Sets the author of the plugin
			\param[in] Author_in : the author of the plugin
		*/
		void SetAuthor(const string_t &Author_in) { Author = Author_in; }
		/*! \brief Sets the identifier of the plugin
			\param[in] UUID_in : the identifier of the plugin
		*/
		void SetIdentifier(const string_t &UUID_in) { PluginIdentifier.FromString(UUID_in.c_str()); }
		/*! \brief Sets the version of the plugin
			\param[in] Version_in : the version of the plugin
		*/
		void SetVersion(const string_t &Version_in) { Version.FromString(Version_in.c_str()); }
		/*! \brief Sets the name of the plugin
			\param[in] Name_in : the name of the plugin
		*/
		void SetName(const string_t &Name_in) { Name = Name_in; }
		/*! \brief Retrieves the name of the plugin
			\return the name of the plugin
		*/
		const string_t& GetName() const { return Name; }

		string_t ToString() const;

	protected:
		//! the name of the plugin
		string_t Name;
		//! the author of the plugin
		string_t Author;
		//! the description of the plugin
		string_t Descritpion;
		//! the URL of the update information
		string_t UpdateURL;
		//! the version of the plugin
		VersionInfo Version;

	private:
		//! flag specifying if the plugin information has been initialized
		bool Initialized;
		//! the plugin UUID
		PluginUUID PluginIdentifier;
		//! the compatible framework version
		VersionInfo FrameworkVersion;
		//! the handle to the DLL
		HMODULE hHandle;
		//! the path of the DLL
		string_t DLLPath;
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
	};

	//! function pointer to an Initialize function
	typedef RegisterParams* (*fnInitialize)(IPluginServices *pServices_in);

	/*! \brief Base plugin interface */
	class IPlugin
	{
		friend class PluginManager;
	public:
		//! \brief IPlugin default constructor
		IPlugin();
		//! \brief IPlugin destructor
		virtual ~IPlugin();

		/*! \brief Retrieves the instance of the plugin services
			\return the instance of the plugin services
		*/
		static const IPluginServices* Services() { return m_pServices; }
		/*! \brief Sets the services used to (un)subscribe to services and invoke them
			\param[in] pServices_in : the services
		*/
		static void SetServices(IPluginServices *pServices_in) { m_pServices = pServices_in; }
		/*! \brief Retrieves the framework version against which the plugin was compiled
			\return the framework version
		*/
		const VersionInfo& GetFrameworkVersion() const { return m_PluginInfo.FrameworkVersion; }

		/*! \brief Retrieves the version of the plugin
			\return the version of the plugin
		*/
		const VersionInfo& GetVersion() const { return m_PluginInfo.Version; }

		/*! \brief Retrieves the URL used to check for updates
			\return the URL used to check for updates
		*/
		const string_t& GetUpdateURL() const { return m_PluginInfo.UpdateURL; }

		/*! \brief Retrieves the path of the plugin DLL
			\return the path of the plugin DLL
		*/
		const string_t& GetDLLPath() const { return m_PluginInfo.DLLPath; }

		/*! \brief Retrieves the author of the plugin
			\return the author of the plugin
		*/
		const string_t& GetAuthor() const { return m_PluginInfo.Author; }

		/*! \brief Retrieves the UUID of the plugin
			\return the UUID of the plugin
		*/
		const PluginUUID& GetUUID() const { return m_PluginInfo.PluginIdentifier; }

		/*! \brief Retrieves the name of the plugin
			\return the name of the plugin
		*/
		const string_t& GetName() const { return m_PluginInfo.Name; }

		/*! \brief Retrieves the description of the plugin
			\return the description of the plugin
		*/
		const string_t& GetDesc() const { return m_PluginInfo.Descritpion; }

		/*! \brief Checks if the plugin has been initialized properly
			\return true if the plugin is initialized; false otherwise
		*/
		bool IsInitialized() const { return m_PluginInfo.Initialized; }

		/*! \brief Retrieves the handle of the plugin DLL once loaded
			\return the handle of the plugin
		*/
		HMODULE GetHandle() const { return m_PluginInfo.hHandle; }
		/*! \brief Converts the plugin information to a string
			\return a string representation of the plugin information
		*/
		string_t ToString() const;

		/*! \brief Sets the URL used to check for updates
			\param[in] UpdateURL_in : the URL used to check for updates
		*/
		void SetUpdateURL(const string_t &UpdateURL_in) { m_PluginInfo.UpdateURL = UpdateURL_in; }
		/*! \brief Sets the description of the plugin
			\param[in] Descritpion_in : the description of the plugin
		*/
		void SetDesc(const string_t &Descritpion_in) { m_PluginInfo.Descritpion = Descritpion_in; }
		/*! \brief Sets the author of the plugin
			\param[in] Author_in : the author of the plugin
		*/
		void SetAuthor(const string_t &Author_in) { m_PluginInfo.Author = Author_in; }
		/*! \brief Sets the version of the plugin
			\param[in] Version_in : the version of the plugin
		*/
		void SetVersion(const string_t &Version_in) { m_PluginInfo.Version.FromString(Version_in.c_str()); }
		/*! \brief Sets the name of the plugin
			\param[in] Name_in : the name of the plugin
		*/
		void SetName(const string_t &Name_in) { m_PluginInfo.Name = Name_in; }

		/*! \brief Initializes the plugin
			\param[in] pfnCreateFunc_in : a pointer to the 'Create' function of the plugin
			\param[in] pfnDestroyFunc_in : a pointer to the 'Destroy' function of the plugin
		*/
		static RegisterParams* Initialize(fnCreate pfnCreateFunc_in, fnDestroy pfnDestroyFunc_in, fnQuery pfnQueryFunc_in);

		//! Callback function invoked when a new instance of the plugin has been created
		void OnCreate();
		//! Callback function invoked when a new instance of the plugin is about to be destroyed
		void OnDestroy();

		//! \brief Fills a PluginInfo structure with the plugin information
		static void Query(PluginFramework::PluginInfo& PluginInfo_out);

	protected:
		virtual bool Unsubscribe();
		virtual bool Subscribe();

		//! the plugin information (version, name, author, etc.)
		PluginInfo m_PluginInfo;

	private:
		//! plugin services used to (un)subscribe to services and invoke them
		static IPluginServices *m_pServices;
	};
}

extern "C"
{
	/*! \brief Function exposed by the plugin DLL to initialize the plugin object
		\param[in] pServices_in : services used to (un)subscribe to services and invoke them
		\return a pointer to the plugin registration parameters if successful; NULL otherwise
	*/
	PLUGIN_API PluginFramework::RegisterParams* InitPlugin(PluginFramework::IPluginServices *pServices_in);
}

#endif//__IPLUGIN_H__