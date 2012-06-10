/**************************************************************************
	created		:	2011-07-10
	filename	: 	TimestampSettings.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Timestamp plugin settings
**************************************************************************/
#include "stdafx.h"

#include <SettingsManager.h>
#include "TimestampSettings.h"

namespace Windower
{
	/*! \brief TimestampSettings constructor
		\param[in] pSettingsFile_in : the path of the settings file
	*/
	TimestampSettings::TimestampSettings(const TCHAR *pSettingsFile_in, const TCHAR *pProfileName_in)
		: SettingsIniFile(pSettingsFile_in)
	{
		if (pProfileName_in != NULL)
			m_SectionName = pProfileName_in;

		if (pSettingsFile_in != NULL && Load() == false)
			Save();
	}

	/*! \brief Saves the settings to the INI file
		\return true if the settings were saved successfully; false otherwise
	*/
	bool TimestampSettings::Save()
	{
		SetString(m_SectionName.c_str(), _T("TimestampFormat"), m_Format);

		return SettingsIniFile::Save();
	}

	/*! \brief Loads the settings from the INI file
		\return true if the settings were loaded successfully; false otherwise
	*/
	bool TimestampSettings::Load()
	{
		if (SettingsIniFile::Load())
		{
			if (m_SectionName.empty())
				m_SectionName = GetString(_T("General"), _T("CurrentProfile"), _T("Profile:Default"));

			m_Format = GetString(m_SectionName.c_str(), _T("TimestampFormat"), _T("[HH:mm:ss] "));

			return true;
		}

		return false;
	}
}