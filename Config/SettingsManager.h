#ifndef __SETTINGS_MANAGER_H__
#define __SETTINGS_MANAGER_H__

#define DEFAULT_PROFILE_NAME L"Default"

typedef std::vector<WindowerSettings*>::const_iterator SettingsConstIterator;
typedef std::vector<WindowerSettings*>::iterator SettingsIterator;
typedef CSimpleIni::TNamesDepend::iterator SectionsIterator;

class SettingsIniFile;

class SettingsManager
{
public:
	SettingsManager(const TCHAR *pIniFile);
	~SettingsManager();

	virtual WindowerSettings* CreateProfile(const TCHAR *pProfileName, const WindowerSettings *pSettings);
	virtual bool LoadProfile(const TCHAR *pProfileName, WindowerSettings **pSettings);
	virtual bool CopyProfile(const TCHAR *pDstProfile, WindowerSettings Src);
	virtual WindowerSettings* GetSettings(const TCHAR *pProfileName);
	virtual void SetDefaultProfile(const TCHAR *pProfileName);
	virtual bool DeleteProfile(const TCHAR *pProfileName);
	virtual const TCHAR* GetDefaultProfile();
	virtual bool CreateDefaultProfile();
	SettingsIterator Begin() { return m_Profiles.begin(); }
	const SettingsConstIterator End() { return m_Profiles.end(); }
	virtual bool Load();
	virtual bool Save();
protected:
	SettingsIniFile *m_pSettingsFile;
	TCHAR			*m_pDefaultProfile;
	std::vector<WindowerSettings*> m_Profiles;
};

#endif//__SETTINGS_MANAGER_H__