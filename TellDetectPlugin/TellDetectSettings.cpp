/**************************************************************************
	created		:	2011-07-10
	filename	: 	TellDetectSettings.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	TellDetect plugin settings
**************************************************************************/
#include "stdafx.h"

#include <SettingsManager.h>
#include "TellDetectSettings.h"

namespace Windower
{
	/*! \brief TellDetectSettings constructor
		\param[in] pSettingsFile_in : the path of the settings file
	*/
	TellDetectSettings::TellDetectSettings(const TCHAR *pSettingsFile_in, const TCHAR *pProfileName_in)
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
	bool TellDetectSettings::Save()
	{
		SetString(m_SectionName.c_str(), _T("TellSound"), m_SoundFilename);

		return SettingsIniFile::Save();
	}

	/*! \brief Loads the settings from the INI file
		\return true if the settings were loaded successfully; false otherwise
	*/
	bool TellDetectSettings::Load()
	{
		if (SettingsIniFile::Load())
		{
			if (m_SectionName.empty())
				m_SectionName = GetString(_T("General"), _T("CurrentProfile"), _T("Profile:Default"));

			m_SoundFilename = GetString(m_SectionName.c_str(), _T("TellSound"), _T("tell.wav"));

			return true;
		}

		return false;
	}
}