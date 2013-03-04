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
	AutoLoginSettings::AutoLoginSettings(const TCHAR *pSettingsFile_in, const TCHAR *pProfileName_in)
		: SettingsIniFile(pSettingsFile_in), m_KeyHash(0L), m_hParentWnd(NULL), m_ConfigFile(pSettingsFile_in)
	{
		if (pProfileName_in != NULL)
			m_SectionName = pProfileName_in;

		if (pSettingsFile_in != NULL && Load() == false)
			Save();
	}

	/*! \brief Saves the settings to the INI file
		\return true if the settings were saved successfully; false otherwise
	*/
	bool AutoLoginSettings::Save()
	{
		SetString(m_SectionName.c_str(), _T("Password"), m_Password);
		SetString(m_SectionName.c_str(), _T("Username"), m_Username);
		SetHex(m_SectionName.c_str(), _T("KeyHash"), m_KeyHash);

		return SettingsIniFile::Save();
	}

	/*! \brief Loads the settings from the INI file
		\return true if the settings were loaded successfully; false otherwise
	*/
	bool AutoLoginSettings::Load()
	{
		if (SettingsIniFile::Load())
		{
			if (m_SectionName.empty())
				m_SectionName = GetString(_T("General"), _T("CurrentProfile"), _T("Profile:Default"));

			m_Username = GetString(m_SectionName.c_str(), _T("Username"));
			m_Password = GetString(m_SectionName.c_str(), _T("Password"));
			m_KeyHash = GetUnsignedLong(m_SectionName.c_str(), _T("KeyHash"));

			return true;
		}

		return false;
	}
}