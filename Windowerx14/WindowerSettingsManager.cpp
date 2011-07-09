/**************************************************************************
	created		:	2011-06-03
	filename	: 	WindowerSettingsManager.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower settings manager
**************************************************************************/
#include "stdafx.h"
#include <vector>

#include <SettingsInterface.h>
#include <SettingsIniFile.h>
#include "WindowerSettings.h"
#include "WindowerSettingsManager.h"

namespace Windower
{
	SettingsManager::SettingsManager(const TCHAR *pIniFile)
	{
		bool bEmpty = true;

		m_pDefaultProfile = NULL;

		if (pIniFile != NULL)
		{
			m_pSettingsFile = new SettingsIniFile(pIniFile);

			if (m_pSettingsFile->Load())
				bEmpty = !Load();
		}
		else
			m_pSettingsFile = NULL;

		if (bEmpty)
		{
			CreateDefaultProfile();
			Save();
		}
	}

	SettingsManager::~SettingsManager()
	{
		SettingsIterator SettingsIter;

		for (SettingsIter = m_Profiles.begin(); SettingsIter != m_Profiles.end(); ++SettingsIter)
		{
			if (*SettingsIter != NULL)
			{
				delete *SettingsIter;
				*SettingsIter = NULL;
			}
		}

		m_Profiles.clear();

		if (m_pDefaultProfile != NULL)
		{
			delete m_pDefaultProfile;
			m_pDefaultProfile = NULL;
		}

		if (m_pSettingsFile != NULL)
		{
			delete m_pSettingsFile;
			m_pSettingsFile = NULL;
		}
	}

	WindowerProfile* SettingsManager::CreateProfile(const TCHAR *pProfileName, const WindowerProfile *pSettings)
	{
		if (m_pSettingsFile != NULL && pProfileName != NULL && pSettings != NULL)
		{
			WindowerProfile *pNewSettings = new WindowerProfile(pSettings);

			m_Profiles.push_back(pNewSettings);

			return pNewSettings;
		}

		return NULL;
	}

	bool SettingsManager::DeleteProfile(const TCHAR *pProfileName)
	{
		if (m_pSettingsFile != NULL && pProfileName != NULL)
			return m_pSettingsFile->DeleteSection(pProfileName);

		return false;
	}

	bool SettingsManager::Save()
	{
		if (m_pSettingsFile != NULL)
		{
			SettingsIterator SettingsIter;

			m_pSettingsFile->SetString(_T("General"), _T("CurrentProfile"), m_pDefaultProfile);

			for (SettingsIter = m_Profiles.begin(); SettingsIter != m_Profiles.end(); ++SettingsIter)
			{
				const TCHAR *pName = (*SettingsIter)->GetName();

				m_pSettingsFile->SetLong(pName, _T("ResX"),  (*SettingsIter)->GetResX());
				m_pSettingsFile->SetLong(pName, _T("ResY"),  (*SettingsIter)->GetResY());
				m_pSettingsFile->SetLong(pName, _T("VSync"), (*SettingsIter)->GetVSync());
				m_pSettingsFile->SetString(pName, _T("PluginsDir"), (*SettingsIter)->GetPluginsDir());
			}

			return m_pSettingsFile->Save();
		}

		return false;
	}

	bool SettingsManager::LoadProfile(const TCHAR *pProfileName, WindowerProfile **pSettings)
	{
		if (m_pSettingsFile != NULL && pProfileName != NULL && pSettings != NULL)
		{
			(*pSettings)->SetVSync(m_pSettingsFile->GetLong(pProfileName, _T("VSync")));
			(*pSettings)->SetResolution(m_pSettingsFile->GetLong(pProfileName, _T("ResX")),
										m_pSettingsFile->GetLong(pProfileName, _T("ResY")));
			(*pSettings)->SetPluginsDir(m_pSettingsFile->GetString(pProfileName, _T("PluginsDir")));
			(*pSettings)->SetName(pProfileName);

			return true;
		}

		return false;
	}

	bool SettingsManager::LoadDefaultProfile(WindowerProfile **pSettings_out)
	{
		return LoadProfile(GetDefaultProfile(), pSettings_out);
	}

	bool SettingsManager::Load()
	{
		if (m_pSettingsFile != NULL)
		{
			const TCHAR *pCurrentProfile = _T("CurrentProfile");
			const TCHAR *pGeneralSection = _T("General");
			CSimpleIni::TNamesDepend Sections;
			SectionsIterator SectionIter;

			SetDefaultProfile(m_pSettingsFile->GetString(pGeneralSection, pCurrentProfile));
			m_pSettingsFile->getSections(Sections);

			for (SectionIter = Sections.begin(); SectionIter != Sections.end(); ++SectionIter)
			{
				if (_tcsicmp(SectionIter->pItem, pGeneralSection) != 0)
				{
					WindowerProfile *pSettings = new WindowerProfile();

					if (LoadProfile(SectionIter->pItem, &pSettings))
						m_Profiles.push_back(pSettings);
					else
						delete pSettings;
				}
			}

			return true;
		}

		return false;
	}

	bool SettingsManager::CreateDefaultProfile()
	{
		if (m_pSettingsFile == NULL)
			return false;

		LONG ResX = GetSystemMetrics(SM_CXSCREEN);
		LONG ResY = GetSystemMetrics(SM_CYSCREEN);
		WindowerProfile Settings(ResX, ResY, TRUE, DEFAULT_PROFILE_NAME, DEFAULT_PLUGINS_DIR);

		SetDefaultProfile(DEFAULT_PROFILE_NAME);
		m_pSettingsFile->SetString(_T("General"), _T("CurrentProfile"), m_pDefaultProfile);

		return CreateProfile(DEFAULT_PROFILE_NAME, &Settings) != NULL;
	}

	void SettingsManager::SetDefaultProfile(const TCHAR* pProfileName)
	{
		if (pProfileName != NULL)
		{
			if (m_pDefaultProfile != NULL)
				delete m_pDefaultProfile;

			m_pDefaultProfile = _tcsdup(pProfileName);
		}
	}

	const TCHAR* SettingsManager::GetDefaultProfile()
	{
		return m_pDefaultProfile;
	}

	WindowerProfile* SettingsManager::GetSettings(const TCHAR *pProfileName)
	{
		SettingsIterator SettingsIter;

		for (SettingsIter = m_Profiles.begin(); SettingsIter != m_Profiles.end(); ++SettingsIter)
		{
			if (_tcsicmp((*SettingsIter)->GetName(), pProfileName) == 0)
			{
				return *SettingsIter;
			}
		}

		return NULL;
	}

	bool SettingsManager::CopyProfile(const TCHAR *pDstProfile, const WindowerProfile &Src)
	{
		WindowerProfile *pProfile = GetSettings(pDstProfile);

		if (pProfile != NULL)
		{
			pProfile->SetResolution(Src.GetResX(), Src.GetResY());
			pProfile->SetPluginsDir(Src.GetPluginsDir());
			pProfile->SetVSync(Src.GetVSync());
			pProfile->SetName(Src.GetName());
		}

		return (pProfile != NULL);
	}
}