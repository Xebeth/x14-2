/**************************************************************************
	created		:	2011-06-03
	filename	: 	WindowerSettingsManager.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower settings manager
**************************************************************************/
#ifndef __WINDOWER_SETTINGS_MANAGER_H__
#define __WINDOWER_SETTINGS_MANAGER_H__

#ifndef _WINDOWER_PLUGIN_SDK_INCLUDE_
	#error Please include the global header file 'PluginSDK.h'
#endif

namespace Windower
{
	////////////////////////////////////////////////////////////////////////////
	// INI sections
	#define INI_SECTION_GENERAL				_T("General")
	#define INI_SECTION_AUTO_BLACKLIST		_T("AutoBlacklist")
	
	////////////////////////////////////////////////////////////////////////////
	// sub-folders to check in the game	root
	#define GAME_SUBFOLDER					_T("game")
	#define BOOT_SUBFOLDER					_T("boot")

	// general section
	#define INI_KEY_CURRENT_PROFILE			_T("CurrentProfile")
	#define INI_DEFAULT_CURRENT_PROFILE		PROFILE_PREFIX _T("Default")

	#define INI_KEY_GAME_PATH				_T("GamePath")
	#define INI_DEFAULT_GAME_PATH			_T("")

	#define INI_AUTO_UPDATE					_T("AutoUpdate")
	#define INI_DEFAULT_AUTO_UPDATE			FALSE

	typedef hash_map<string_t, WindowerProfile*> WindowerSettings;
	typedef CSimpleIni::TNamesDepend::const_iterator SectionsConstIterator;

	//! \brief Windower settings manager
	class SettingsManager
	{
	public:
		SettingsManager(const TCHAR *pWorkingDir_in, const TCHAR *pIniFile_in);
		~SettingsManager();

		bool SelectDirectory(string_t& SelectedDir_out) const;
		bool CheckGamePath(const string_t& GamePath_in) const;

		bool LoadSettings(WindowerProfile* pSettings_out) const;
		bool CopySettings(const WindowerProfile *pSettings_in);
		bool SaveSettings(WindowerProfile *pSettings_in);		

		WindowerProfile* DuplicateProfile(const TCHAR *pProfileName_in, const WindowerProfile &Settings_in);
		bool RenameProfile(WindowerProfile *pProfile_in_out, string_t& NewName_in_out);
		bool LoadDefaultProfile(WindowerProfile &Settings_out);
		bool DeleteProfile(const TCHAR *pProfileName_in);

		WindowerProfile* GetSettings(const TCHAR *pProfileName_in) const;

		/*! \brief Sets the name of the default profile
			\param[in] pProfileName_in : the new default profile
		*/
		void SetDefaultProfile(const TCHAR *pProfileName_in);
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
		void SetWorkingDir(const TCHAR *pWorkingDir_in) { initialize_path(pWorkingDir_in, m_WorkingDir); }

		/*! \brief Sets the flag specifying if windower auto-updates
			\param[in] AutoUpdate_in : the new value of the flag
		*/
		void SetAutoUpdate(bool AutoUpdate_in) { m_bAutoUpdate = AutoUpdate_in; }

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

		/*! \brief Checks if windower auto-updates
			\return true if windower auto-updates; false otherwise
		*/
		bool IsAutoUpdated() const { return m_bAutoUpdate; }

		bool IsConfigLoaded() const;
		bool Reload();
		bool Load();
		bool Save();

	protected:
		bool CreateDefault(const TCHAR *pProfileName_in = INI_DEFAULT_CURRENT_PROFILE);
		bool LoadProfile(const TCHAR *pProfileName_in, WindowerProfile &Settings_in);
		bool CopyProfile(const TCHAR *pDstProfile_in, const WindowerProfile &Src_in);
		bool RenameProfile(const TCHAR *pProfileName_in, string_t& NewName_in_out);

		void LoadLabels(WindowerProfile &Settings_out);
		void SaveLabels(WindowerProfile &Settings_out);

		const TCHAR* GetString(const TCHAR *pProfileName_in, eIniKeys Key_in) const;
		ULONG GetUnsignedLong(const TCHAR *pProfileName_in, eIniKeys Key_in) const;
		LONG GetLong(const TCHAR *pProfileName_in, eIniKeys Key_in) const;		
		bool GetBool(const TCHAR *pProfileName_in, eIniKeys Key_in) const;

		void SetString(const TCHAR *pProfileName_in, eIniKeys Key_in, const TCHAR *pValue_in);
		void SetString(const TCHAR *pProfileName_in, const TCHAR *Key_in, const TCHAR *pValue_in);
		void SetLong(const TCHAR *pProfileName_in, eIniKeys Key_in, const LONG Value_in);
		void SetHex(const TCHAR *pProfileName_in, eIniKeys Key_in, ULONG Value_in);
		void SetBool(const TCHAR *pProfileName_in, eIniKeys Key_in, bool Value_in);

		bool CheckDuplicates(const string_t& CurrentName_in, string_t& Name_in_out);

		/*! \brief Retrieves the position of the profile specified by its name
			\param[in] pProfileName_in : the name of the profile
			\return the position of the profile
		*/
		WindowerSettings::const_iterator GetSettingsPos(const TCHAR *pProfileName_in) const;
		/*! \brief Checks the configuration file and attempts to fix errors
			\return true if the configuration file is valid; false otherwise
		*/
		bool VerifyConfig();
		bool CreateDefaultScoredWords();
		const ScoredWords::const_iterator& UpdateScoredWord(ScoredWords::const_iterator &WordIt_in);

		//! the profiles from the windower configuration file
		WindowerSettings m_Profiles;
		//! the profiles to remove from the file
		std::set<string_t> m_DeletedProfiles;
		//! the windower configuration file
		Settings::SettingsIniFile *m_pSettingsFile;
		//! flag specifying if windower auto-updates
		bool m_bAutoUpdate;
		//! the name of the default profile
		string_t m_DefaultProfile;
		//! the installation path of the game
		string_t m_GamePath;
		//! the working directory of the manager
		string_t m_WorkingDir;
		//! absolute path to the config file
		string_t m_ConfigPath;
		//! flag specifying if the configuration is loaded
		bool m_bIsLoaded;
		//! scored words for the auto blacklist plugin
		ScoredWords m_ScoredWords;
	};
}

#endif//__WINDOWER_SETTINGS_MANAGER_H__