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

		virtual WindowerProfile* CreateProfile(const TCHAR *pProfileName, const WindowerProfile *pSettings);
		virtual bool LoadDefaultProfile(WindowerProfile **pSettings_out);
		virtual bool LoadProfile(const TCHAR *pProfileName, WindowerProfile **pSettings);
		virtual bool CopyProfile(const TCHAR *pDstProfile, const WindowerProfile &Src);
		virtual WindowerProfile* GetSettings(const TCHAR *pProfileName);
		virtual void SetDefaultProfile(const TCHAR *pProfileName);
		virtual bool DeleteProfile(const TCHAR *pProfileName);
		virtual const TCHAR* GetDefaultProfile();
		virtual bool CreateDefaultProfile();
		SettingsIterator Begin() { return m_Profiles.begin(); }
		const SettingsConstIterator End() { return m_Profiles.end(); }
		virtual bool Load();
		virtual bool Save();
	protected:
		WindowerSettings	 m_Profiles;
		SettingsIniFile		*m_pSettingsFile;
		TCHAR				*m_pDefaultProfile;
	};
}

#endif//__WINDOWER_SETTINGS_MANAGER_H__