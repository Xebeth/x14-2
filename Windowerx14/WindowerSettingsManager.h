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
#include <shlobj.h>

namespace Windower
{
	////////////////////////////////////////////////////////////////////////////
	// INI sections
	#define INI_SECTION_GENERAL				_T("General")
	////////////////////////////////////////////////////////////////////////////
	// INI keys

	// general section
	#define INI_KEY_CURRENT_PROFILE			_T("CurrentProfile")
	#define INI_DEFAULT_CURRENT_PROFILE		PROFILE_PREFIX _T("Default")

	#define INI_KEY_PLUGINS					_T("Plugins")
	#define INI_DEFAULT_PLUGINS				_T("")

	#define INI_KEY_GAME_PATH				_T("GamePath")
	#define INI_DEFAULT_GAME_PATH			_T("")

	// profile sections
	#define INI_KEY_VSYNC					_T("VSync")
	#define INI_DEFAULT_VSYNC				0L
	////////////////////////////////////////////////////////////////////////////
	// sub-folders to check in the game	root
	#define GAME_SUBFOLDER					_T("game")
	#define BOOT_SUBFOLDER					_T("boot")

	typedef stdext::hash_map<string_t, WindowerProfile*> WindowerSettings;
	typedef CSimpleIni::TNamesDepend::iterator SectionsIterator;

	typedef Settings::SettingsIniFile SettingsIniFile;

	//! \brief Windower settings manager
	class SettingsManager
	{
	public:
		SettingsManager(const TCHAR *pWorkingDir_in, const TCHAR *pIniFile_in);
		~SettingsManager();

		WindowerProfile* DuplicateProfile(const TCHAR *pProfileName_in, const WindowerProfile &Settings_in);
		bool CreateDefaultProfile(const TCHAR *pProfileName_in = INI_DEFAULT_CURRENT_PROFILE);
		bool RenameProfile(WindowerProfile *pProfile_in_out, string_t &NewName_in_out);
		bool RenameProfile(const TCHAR *pProfileName_in, string_t &NewName_in_out);
		bool LoadProfile(const TCHAR *pProfileName_in, WindowerProfile &Settings_in);
		bool CopyProfile(const TCHAR *pDstProfile_in, const WindowerProfile &Src_in);		
		bool LoadDefaultProfile(WindowerProfile &Settings_out);
		bool DeleteProfile(const TCHAR *pProfileName_in);		

		bool CheckDuplicates(const string_t &CurrentName_in, string_t &Name_in_out);
		WindowerProfile* GetSettings(const TCHAR *pProfileName_in);

		bool SelectDirectory(string_t &SelectedDir_out) const;
		bool CheckGamePath(const string_t &GamePath_in) const;

		/*! \brief Sets the name of the default profile
			\param[in] pProfileName_in : the new default profile
		*/
		void SetDefaultProfile(const TCHAR *pProfileName_in) { m_DefaultProfile = pProfileName_in; }
		/*! \brief Retrieves the name of the default profile
			\return the name of the default profile
		*/
		const TCHAR* GetDefaultProfile() const { return m_DefaultProfile.c_str(); }
		/*! \brief Sets the directory of the game
			\param[in] pProfileName_in : the game path
		*/
		void SetGamePath(const TCHAR *pGamePath_in) { m_GamePath = pGamePath_in; }
		/*! \brief Sets the working directory of windower
			\param[in] pProfileName_in : the new working directory
		*/
		void SetWorkingDir(const TCHAR *pWorkingDir_in) { m_WorkingDir = pWorkingDir_in; }
		/*! \brief Retrieves the path of the game
			\return the path of the game
		*/
		const TCHAR* GetGamePath() const { return m_GamePath.c_str(); }

		/*! \brief Retrieves the working directory of windower
			\return the working directory of windower
		*/
		const string_t& GetWorkingDir() const { return m_WorkingDir; }

		/*! \brief Retrieves the configuration file path
			\return the configuration file path
		*/
		const string_t& GetConfigFile() const { return m_ConfigPath; }

		/*! \brief Retrieves the profiles from the windower configuration file
			\return the profiles from the windower configuration file
		*/
		const WindowerSettings& GetProfiles() const { return m_Profiles; }

		/*! \brief Checks if the game path is valid
			\return true if the game path is valid; false otherwise
		*/
		bool IsGamePathValid() const { return CheckGamePath(m_GamePath); }

		bool Reload();
		bool Load();
		bool Save();

	protected:
		WindowerSettings::const_iterator GetSettingsPos(const TCHAR *pProfileName_in);
		bool VerifyConfig();

		//! the profiles from the windower configuration file
		WindowerSettings m_Profiles;
		//! the profiles to remove from the file
		std::set<string_t> m_DeletedProfiles;
		//! the windower configuration file
		SettingsIniFile *m_pSettingsFile;
		//! the name of the default profile
		string_t m_DefaultProfile;
		//! the installation path of the game
		string_t m_GamePath;
		//! the working directory of the manager
		string_t m_WorkingDir;
		//! absolute path to the config file
		string_t m_ConfigPath;
	};
}

#endif//__WINDOWER_SETTINGS_MANAGER_H__