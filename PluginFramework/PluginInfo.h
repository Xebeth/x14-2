/**************************************************************************
	created		:	2011-05-28
	filename	: 	PluginInfo.h
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin information class
**************************************************************************/
#ifndef __PLUGIN_INFO_H__
#define __PLUGIN_INFO_H__

#ifndef _PLUGIN_FRAMEWORK_SDK_INCLUDE_
	#error Please include the global header file 'PluginFramework.h'
#endif

namespace PluginFramework
{
	//! \brief Plugin information class
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
		void SetUpdateURL(const string_t &UpdateURL_in);
		/*! \brief Retrieves the URL used to check for updates
			\return the URL used to check for updates
		*/
		const string_t& GetUpdateURL() const;
		/*! \brief Sets the description of the plugin
			\param[in] Description_in : the description of the plugin
		*/
		void SetDesc(const string_t &Description_in);
		/*! \brief Retrieves the description of the plugin
			\return the description of the plugin
		*/
		const string_t& GetDesc() const;
		/*! \brief Sets the author of the plugin
			\param[in] Author_in : the author of the plugin
		*/
		void SetAuthor(const string_t &Author_in);
		/*! \brief Retrieves the author of the plugin
			\return the author of the plugin
		*/
		const string_t& GetAuthor() const;
		/*! \brief Sets the identifier of the plugin
			\param[in] UUID_in : the identifier of the plugin
		*/
		void SetIdentifier(const string_t &UUID_in);
		/*! \brief Retrieves the identifier of the plugin
			\return the identifier of the plugin
		*/
		string_t GetIndentifier() const;
		/*! \brief Retrieves the version of the plugin
			\return the version of the plugin
		*/
		string_t GetVersion() const;
		/*! \brief Sets the version of the plugin
			\param[in] Version_in : the version of the plugin
		*/
		void SetVersion(const string_t &Version_in);
		/*! \brief Sets the name of the plugin
			\param[in] Name_in : the name of the plugin
		*/
		void SetName(const string_t &Name_in);
		/*! \brief Retrieves the name of the plugin
			\return the name of the plugin
		*/
		const string_t& GetName() const;
		/*! \brief Sets the compatibility flags of the plugin
			\param[in] Name_in : the compatibility flags of the plugin
		*/
		void SetCompatibilityFlags(DWORD CompatibilityFlags_in);
		/*! \brief Retrieves the compatibility flags of the plugin
			\return the compatibility flags of the plugin
		*/
		DWORD GetCompatibilityFlags() const;
		/*! \brief Retrieves the compatible framework version
			\return the compatible framework version
		*/
		string_t GetFrameworkVersion() const;
		/*! \brief Retrieves the path of the DLL
			\return the path of the DLL
		*/
		const string_t& GetDLLPath() const;
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
		string_t m_Description;
		//! the URL of the update information
		string_t m_UpdateURL;
		//! the compatibility flags of the plugin
		DWORD m_CompatibilityFlags;
		//! the version of the plugin
		VersionInfo m_Version;

	private:
		//! flag specifying if the plugin information has been initialized
		bool m_Initialized;
		//! the plugin StringUtils::UUID
		StringUtils::UUID m_PluginIdentifier;
		//! the compatible framework version
		VersionInfo m_FrameworkVersion;
		//! the handle to the DLL
		HMODULE m_hHandle;
		//! the path of the DLL
		string_t m_DLLPath;
	};
}

#endif//__PLUGIN_INFO_H__