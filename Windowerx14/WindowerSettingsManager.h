/**************************************************************************
	created		:	2011-06-03
	filename	: 	WindowerSettingsManager.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower settings manager
**************************************************************************/
#ifndef __WINDOWER_SETTINGS_MANAGER_H__
#define __WINDOWER_SETTINGS_MANAGER_H__

#include <SimpleIni.h>
#include <hash_map>

namespace Windower
{
	#define DEFAULT_PROFILE_NAME	PROFILE_PREFIX _T("Default")
	#define DEFAULT_PLUGINS_DIR		_T("plugins")

	typedef stdext::hash_map<string_t, WindowerProfile*> WindowerSettings;
	typedef CSimpleIni::TNamesDepend::iterator SectionsIterator;

	typedef Settings::SettingsIniFile SettingsIniFile;

	//! \brief Windower settings manager
	class SettingsManager
	{
	public:
		explicit SettingsManager(const TCHAR *pIniFile_in);
		~SettingsManager();

		WindowerProfile* DuplicateProfile(const TCHAR *pProfileName_in, const WindowerProfile &Settings_in);
		bool RenameProfile(WindowerProfile *pProfile_in_out, string_t &NewName_in_out);
		bool RenameProfile(const TCHAR *pProfileName_in, string_t &NewName_in_out);
		bool LoadProfile(const TCHAR *pProfileName_in, WindowerProfile &Settings_in);
		bool CopyProfile(const TCHAR *pDstProfile_in, const WindowerProfile &Src_in);
		bool LoadDefaultProfile(WindowerProfile &Settings_out);
		bool DeleteProfile(const TCHAR *pProfileName_in);
		bool CreateDefaultProfile();

		bool CheckDuplicates(const string_t &CurrentName_in, string_t &Name_in_out);
		WindowerProfile* GetSettings(const TCHAR *pProfileName_in);		

		/*! \brief Retrieves the relative directory of the plugins
			\return the relative directory of the plugins
		*/
		const TCHAR* GetPluginsDir() const { return m_PluginsDir.c_str(); }
		/*! \brief Retrieves the absolute directory of the plugins
			\return the absolute directory of the plugins
		*/
		const string_t& GetPluginsAbsoluteDir()
		{
			TCHAR CurrentDir[_MAX_PATH];

			GetCurrentDirectory(_MAX_PATH, CurrentDir);
			format(m_PluginsAbsDir, _T("%s\\%s\\"), CurrentDir, m_PluginsDir.c_str());

			return m_PluginsAbsDir;
		}
		/*! \brief Sets the relative directory of the plugins
			\param[in] pPluginsDir_in : the relative directory of the plugins
		*/
		void SetPluginsDir(const TCHAR *pPluginsDir_in) { if (pPluginsDir_in != NULL) m_PluginsDir = pPluginsDir_in; }
		/*! \brief Sets the name of the default profile
			\param[in] pProfileName_in : the new default profile
		*/
		void SetDefaultProfile(const TCHAR *pProfileName_in) { m_DefaultProfile = pProfileName_in; }
		/*! \brief Retrieves the name of the default profile
			\return the name of the default profile
		*/
		const TCHAR* GetDefaultProfile() const { return m_DefaultProfile.c_str(); }

		/*! \brief Retrieves the profiles from the windower configuration file
			\return the profiles from the windower configuration file
		*/
		const WindowerSettings& GetProfiles() const { return m_Profiles; }

		bool Reload();
		bool Load();
		bool Save();

	protected:
		WindowerSettings::const_iterator GetSettingsPos(const TCHAR *pProfileName_in);

		//! the profiles from the windower configuration file
		WindowerSettings m_Profiles;
		//! the profiles to remove from the file
		std::set<string_t> m_DeletedProfiles;
		//! the windower configuration file
		SettingsIniFile *m_pSettingsFile;
		//! the name of the default profile
		string_t m_DefaultProfile;
		//! the absolute directory of the plugins
		string_t m_PluginsAbsDir;
		//! the relative directory of the plugins
		string_t m_PluginsDir;
	};
}

#endif//__WINDOWER_SETTINGS_MANAGER_H__