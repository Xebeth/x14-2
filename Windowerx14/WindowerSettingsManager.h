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

	class SettingsManager
	{
	public:
		SettingsManager(const TCHAR *pIniFile);
		~SettingsManager();

		WindowerProfile* CreateProfile(const TCHAR *pProfileName, const WindowerProfile *pSettings);
		bool LoadDefaultProfile(WindowerProfile **pSettings_out);
		bool LoadProfile(const TCHAR *pProfileName, WindowerProfile *pSettings);
		bool CopyProfile(const TCHAR *pDstProfile, const WindowerProfile &Src);
		WindowerProfile* GetSettings(const TCHAR *pProfileName);
		bool DeleteProfile(const TCHAR *pProfileName);
		bool CreateDefaultProfile();
		SettingsIterator Begin() { return m_Profiles.begin(); }
		const SettingsConstIterator End() { return m_Profiles.end(); }

		void SetDefaultProfile(const TCHAR *pProfileName)
			{ m_DefaultProfile = pProfileName; }
		const string_t& GetDefaultProfile() const { return m_DefaultProfile; }
		void SetAutoLogin(bool AutoLogin_in)
			{ m_AutoLogin = AutoLogin_in; }
		bool GetAutoLogin() const { return m_AutoLogin; }


		bool Load();
		bool Save();
	protected:
		WindowerSettings	 m_Profiles;
		SettingsIniFile		*m_pSettingsFile;
		string_t			 m_DefaultProfile;
		bool				 m_AutoLogin;
	};
}

#endif//__WINDOWER_SETTINGS_MANAGER_H__