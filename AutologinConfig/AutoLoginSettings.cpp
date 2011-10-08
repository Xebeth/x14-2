/**************************************************************************
	created		:	2011-07-10
	filename	: 	AutoLoginSettings.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "SettingsManager.h"
#include "AutoLoginSettings.h"

/*! \brief AutoLoginSettings constructor
	\param[in] pSettingsFile_in : the path of the settings file
*/
AutoLoginSettings::AutoLoginSettings(const TCHAR *pSettingsFile_in)
	: SettingsIniFile(pSettingsFile_in), m_KeyHash(0L), m_AutoValidate(false), m_hParentWnd(NULL)
{
	if (pSettingsFile_in != NULL && Load() == false)
		Save();
}

bool AutoLoginSettings::Save()
{
	SetLong(_T("AutoLogin"), _T("AutoValidate"), m_AutoValidate ? 1L : 0L);
	SetString(_T("AutoLogin"), _T("Password"), m_Password);
	SetHex(_T("AutoLogin"), _T("KeyHash"), m_KeyHash);

	return SettingsIniFile::Save();
}

bool AutoLoginSettings::Load()
{
	if (SettingsIniFile::Load())
	{
		m_AutoValidate = (GetLong(_T("AutoLogin"), _T("AutoValidate")) == 1);
		m_Password = GetString(_T("AutoLogin"), _T("Password"));
		m_KeyHash = GetUnsignedLong(_T("AutoLogin"), _T("KeyHash"));

		return true;
	}

	return false;
}
