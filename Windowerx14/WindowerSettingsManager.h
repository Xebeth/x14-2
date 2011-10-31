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

namespace Windower
{
	#define DEFAULT_PROFILE_NAME _T("Default")
	#define DEFAULT_PLUGINS_DIR _T("plugins")

	typedef std::vector<WindowerProfile*> WindowerSettings;
	typedef WindowerSettings::const_iterator SettingsConstIterator;
	typedef WindowerSettings::iterator SettingsIterator;

	typedef CSimpleIni::TNamesDepend::iterator SectionsIterator;

	typedef Settings::SettingsIniFile SettingsIniFile;

	//! \brief Windower settings manager
	class SettingsManager
	{
	public:
		explicit SettingsManager(const TCHAR *pIniFile_in);
		~SettingsManager();

		WindowerProfile* CreateProfile(const TCHAR *pProfileName_in, const WindowerProfile &Settings_in);
		bool LoadDefaultProfile(WindowerProfile &Settings_out);
		bool LoadProfile(const TCHAR *pProfileName_in, WindowerProfile &Settings_in);
		bool CopyProfile(const TCHAR *pDstProfile_in, const WindowerProfile &Src_in);
		WindowerProfile* GetSettings(const TCHAR *pProfileName_in);
		bool DeleteProfile(const TCHAR *pProfileName_in);
		bool CreateDefaultProfile();

		/*! \brief Sets the name of the default profile
			\param[in] pProfileName_in : the new default profile
		*/
		void SetDefaultProfile(const TCHAR *pProfileName_in) { m_DefaultProfile = pProfileName_in; }
		/*! \brief Retrieves the name of the default profile
			\return the name of the default profile
		*/
		const string_t& GetDefaultProfile() const { return m_DefaultProfile; }
		/*! \brief Sets the flag specifying if the AutoLogin plugin is in use
			\param[in] AutoLogin_in : the new value of the flag
		*/
		void SetAutoLogin(bool AutoLogin_in) { m_AutoLogin = AutoLogin_in; }
		/*! \brief Retrieves the flag specifying if the AutoLogin plugin is in use
			\return the flag specifying if the AutoLogin plugin is in use
		*/
		bool GetAutoLogin() const { return m_AutoLogin; }

		bool Load();
		bool Save();
	protected:
		//! the profiles from the windower configuration file
		WindowerSettings	 m_Profiles;
		//! the windower configuration file
		SettingsIniFile		*m_pSettingsFile;
		//! the name of the default profile
		string_t			 m_DefaultProfile;
		//! flag specifying if the AutoLogin plugin is in use
		bool				 m_AutoLogin;
	};
}

#endif//__WINDOWER_SETTINGS_MANAGER_H__