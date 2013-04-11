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

namespace Bootstrap
{
	/*! \brief AutoLoginSettings constructor
		\param[in] pSettingsFile_in : the path of the settings file
	*/
	AutoLoginSettings::AutoLoginSettings(const TCHAR *pSettingsFile_in, const TCHAR *pProfileName_in)
		: Settings::SettingsIniFile(pSettingsFile_in), m_KeyHash(0L), m_Language(1L), 
		  m_hParentWnd(NULL), m_ConfigFile(pSettingsFile_in), m_AutoSubmit(false)
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
		SetLong(m_SectionName.c_str(), _T("AutoSubmit"), m_AutoSubmit ? 1L : 0L);
		SetString(m_SectionName.c_str(), _T("Password"), m_Password);
		SetString(m_SectionName.c_str(), _T("Username"), m_Username);
		SetLong(m_SectionName.c_str(), _T("Language"), m_Language);		
		SetHex(m_SectionName.c_str(), _T("KeyHash"), m_KeyHash);

		return Settings::SettingsIniFile::Save();
	}

	/*! \brief Loads the settings from the INI file
		\return true if the settings were loaded successfully; false otherwise
	*/
	bool AutoLoginSettings::Load()
	{
		if (Settings::SettingsIniFile::Load())
		{
			if (m_SectionName.empty())
				m_SectionName = GetString(_T("General"), _T("CurrentProfile"), _T("Profile:Default"));

			m_Username = GetString(m_SectionName.c_str(), _T("Username"));
			m_Password = GetString(m_SectionName.c_str(), _T("Password"));
			m_Language = GetLong(m_SectionName.c_str(), _T("Language"), 1L);
			m_KeyHash = GetUnsignedLong(m_SectionName.c_str(), _T("KeyHash"));
			m_AutoSubmit = (GetLong(m_SectionName.c_str(), _T("AutoSubmit")) == 1L);

			return true;
		}

		return false;
	}
}