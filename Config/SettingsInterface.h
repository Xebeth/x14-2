#ifndef __SETTINGS_INTERFACE_H__
#define __SETTINGS_INTERFACE_H__

class SettingsInterface
{
public:
	// Data source functions
	virtual void SetSource(const TCHAR *pDataSource) = 0;
	virtual bool Load() = 0;
	virtual bool Save() = 0;

	// Sections functions
	virtual bool CreateSection(const TCHAR *pSectionName) = 0;
	virtual bool DeleteSection(const TCHAR *pSectionName) = 0;

	// Settings accessors
	virtual LONG GetLong(const TCHAR *pSectionName, const TCHAR* pIntKey) const = 0;
	virtual void SetLong(const TCHAR *pSectionName, const TCHAR* pIntKey, LONG NewValue) = 0;
	virtual const TCHAR* GetString(const TCHAR *pSectionName, const TCHAR* pStringKey) const = 0;
	virtual void SetString(const TCHAR *pSectionName, const TCHAR* pStringKey, const TCHAR* pNewValue) = 0;
};

#endif//__SETTINGS_INTERFACE_H__