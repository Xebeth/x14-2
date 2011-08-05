/**************************************************************************
	created		:	2011-07-10
	filename	: 	AutoLoginSettings.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
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
		: m_AutoValidate(false), m_hParentWnd(NULL)
	{
		bool bEmpty = true;

		if (pSettingsFile_in != NULL)
		{
			m_pSettingsFile = new SettingsIniFile(pSettingsFile_in);
			bEmpty = !Load();
		}
		else
			m_pSettingsFile = NULL;

		if (bEmpty)
			Save();
	}

	bool AutoLoginSettings::Save()
	{
		if (m_pSettingsFile != NULL)
		{
			m_pSettingsFile->SetLong(_T("AutoLogin"), _T("AutoValidate"), m_AutoValidate ? 1L : 0L);
			m_pSettingsFile->SetString(_T("AutoLogin"), _T("Password"), m_Password);
			m_pSettingsFile->SetHex(_T("AutoLogin"), _T("KeyHash"), m_KeyHash);

			return m_pSettingsFile->Save();
		}

		return false;
	}

	bool AutoLoginSettings::Load()
	{
		if (m_pSettingsFile != NULL && m_pSettingsFile->Load())
		{
			m_AutoValidate = (m_pSettingsFile->GetLong(_T("AutoLogin"), _T("AutoValidate")) == 1);
			m_Password = m_pSettingsFile->GetString(_T("AutoLogin"), _T("Password"));
			m_KeyHash = m_pSettingsFile->GetUnsignedLong(_T("AutoLogin"), _T("KeyHash"));

			return true;
		}

		return false;
	}
}