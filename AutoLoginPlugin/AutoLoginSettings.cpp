/**************************************************************************
	created		:	2011-07-10
	filename	: 	AutoLoginSettings.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	AutoLogin plugin settings
**************************************************************************/
#include "stdafx.h"

#include <SettingsManager.h>
#include "AutoLoginSettings.h"

namespace Windower
{
	/*! \brief AutoLoginSettings constructor
		\param[in] pSettingsFile_in : the path of the settings file
	*/
	AutoLoginSettings::AutoLoginSettings(const TCHAR *pSettingsFile_in)
		: SettingsIniFile(pSettingsFile_in), m_KeyHash(0L), m_hParentWnd(NULL)
	{
		if (pSettingsFile_in != NULL && Load() == false)
			Save();
	}

	/*! \brief Saves the settings to the INI file
		\return true if the settings were saved successfully; false otherwise
	*/
	bool AutoLoginSettings::Save()
	{
		SetString(_T("Plugin:AutoLogin"), _T("Password"), m_Password);
		SetHex(_T("Plugin:AutoLogin"), _T("KeyHash"), m_KeyHash);

		return SettingsIniFile::Save();
	}

	/*! \brief Loads the settings from the INI file
		\return true if the settings were loaded successfully; false otherwise
	*/
	bool AutoLoginSettings::Load()
	{
		if (SettingsIniFile::Load())
		{
			m_Password = GetString(_T("Plugin:AutoLogin"), _T("Password"));
			m_KeyHash = GetUnsignedLong(_T("Plugin:AutoLogin"), _T("KeyHash"));

			return true;
		}

		return false;
	}
}