#ifndef __SETTINGS_INI_FILE_H__
#define __SETTINGS_INI_FILE_H__

class SettingsIniFile : public SettingsInterface
{
public:
	SettingsIniFile(const TCHAR *pSourceFile);
	virtual ~SettingsIniFile();

	// Data source functions
	virtual void SetSource(const TCHAR *pDataSource);
	virtual bool Load();
	virtual bool Save();

	// Sections functions
	virtual bool CreateSection(const TCHAR *pSectionName);
	virtual bool DeleteSection(const TCHAR *pSectionName);
	virtual void getSections(CSimpleIni::TNamesDepend &Sections) const;

	// Settings accessors
	virtual LONG GetLong(const TCHAR *pSectionName, const TCHAR* pIntKey) const;
	virtual void SetLong(const TCHAR *pSectionName, const TCHAR* pIntKey, LONG NewValue);
	virtual const TCHAR* GetString(const TCHAR *pSectionName, const TCHAR* pStringKey) const;
	virtual void SetString(const TCHAR *pSectionName, const TCHAR* pStringKey, const TCHAR* pNewValue);
	CSimpleIni *m_pIni;
protected:
	TCHAR *m_pSourceFile;
};

#endif//__SETTINGS_INI_FILE_H__