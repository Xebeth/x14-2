/**************************************************************************
	created		:	2011-06-03
	filename	: 	WindowerSettingsManager.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower settings manager
**************************************************************************/
#include "stdafx.h"
#include <vector>

#include <SettingsManager.h>
#include "WindowerSettings.h"
#include "WindowerSettingsManager.h"

namespace Windower
{
	/*! \brief SettingsManager constructor
		\param[in] pIniFile_in : the path of the configuration file
	*/
	SettingsManager::SettingsManager(const TCHAR *pIniFile_in) : m_AutoLogin(false)
	{
		bool bEmpty = true;

		if (pIniFile_in != NULL)
		{
			m_pSettingsFile = new SettingsIniFile(pIniFile_in);

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

	//! \brief SettingsManager destructor
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

		if (m_pSettingsFile != NULL)
		{
			delete m_pSettingsFile;
			m_pSettingsFile = NULL;
		}
	}

	/*! \brief Creates a copy of the specified profile
		\param[in] pProfileName_in : the name of the profile
		\param[in] Settings_in : the settings being copied
		\return the newly created profile if successful; NULL otherwise
	*/
	WindowerProfile* SettingsManager::CreateProfile(const TCHAR *pProfileName_in, const WindowerProfile &Settings_in)
	{
		if (m_pSettingsFile != NULL && pProfileName_in != NULL)
		{
			WindowerProfile *pNewSettings = new WindowerProfile(Settings_in);

			m_Profiles.push_back(pNewSettings);

			return pNewSettings;
		}

		return NULL;
	}

	/*! \brief Deletes the profile specified by its name
		\param[in] pProfileName_in : the name of the profile
		\return true if the profile was deleted successfully; false otherwise
	*/
	bool SettingsManager::DeleteProfile(const TCHAR *pProfileName_in)
	{
		if (m_pSettingsFile != NULL && pProfileName_in != NULL)
			return m_pSettingsFile->DeleteSection(pProfileName_in);

		return false;
	}

	/*! \brief Saves the configuration
		\return true if successful; false otherwise
	*/
	bool SettingsManager::Save()
	{
		if (m_pSettingsFile != NULL)
		{
			SettingsIterator SettingsIter;

			m_pSettingsFile->SetString(_T("General"), _T("CurrentProfile"), m_DefaultProfile);
			m_pSettingsFile->SetLong(_T("General"), _T("AutoLogin"), m_AutoLogin);

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

	/*! \brief Loads profile specified by its name
		\param[in] pProfileName_in : the name of the profile to load
		\param[out] Settings_out : the settings being loaded
		\return true if the profile exits and was loaded successfully; false otherwise
	*/
	bool SettingsManager::LoadProfile(const TCHAR *pProfileName_in, WindowerProfile &Settings_out)
	{
		if (m_pSettingsFile != NULL && pProfileName_in != NULL)
		{
			Settings_out.SetVSync(m_pSettingsFile->GetLong(pProfileName_in, _T("VSync")));
			Settings_out.SetResolution(m_pSettingsFile->GetLong(pProfileName_in, _T("ResX")),
									   m_pSettingsFile->GetLong(pProfileName_in, _T("ResY")));
			Settings_out.SetPluginsDir(m_pSettingsFile->GetString(pProfileName_in, _T("PluginsDir")));
			Settings_out.SetName(pProfileName_in);

			return true;
		}

		return false;
	}

	/*! \brief Loads the default profile
		\param[in] Settings_out : the settings receiving the default profile
		\return true if the default profile was loaded successfully; false otherwise
	*/
	bool SettingsManager::LoadDefaultProfile(WindowerProfile &Settings_out)
	{
		return LoadProfile(GetDefaultProfile().c_str(), Settings_out);
	}

	/*! \brief Loads the configuration
		\return true if successful; false otherwise
	*/
	bool SettingsManager::Load()
	{
		if (m_pSettingsFile != NULL)
		{
			CSimpleIni::TNamesDepend Sections;
			SectionsIterator SectionIter;

			SetDefaultProfile(m_pSettingsFile->GetString(_T("General"), _T("CurrentProfile")));
			SetAutoLogin(m_pSettingsFile->GetLong(_T("General"), _T("AutoLogin")) == 1);

			m_pSettingsFile->getSections(Sections);

			for (SectionIter = Sections.begin(); SectionIter != Sections.end(); ++SectionIter)
			{
				if (_tcsicmp(SectionIter->pItem, _T("General")) != 0)
				{
					WindowerProfile *pSettings = new WindowerProfile();

					if (LoadProfile(SectionIter->pItem, *pSettings))
						m_Profiles.push_back(pSettings);
					else
						delete pSettings;
				}
			}

			return true;
		}

		return false;
	}

	/*! \brief Creates the default profile
		\return true if successful; false otherwise
	*/
	bool SettingsManager::CreateDefaultProfile()
	{
		if (m_pSettingsFile == NULL)
			return false;

		LONG ResX = GetSystemMetrics(SM_CXSCREEN);
		LONG ResY = GetSystemMetrics(SM_CYSCREEN);
		WindowerProfile Settings(ResX, ResY, TRUE, DEFAULT_PROFILE_NAME, DEFAULT_PLUGINS_DIR);

		SetDefaultProfile(DEFAULT_PROFILE_NAME);
		m_pSettingsFile->SetString(_T("General"), _T("CurrentProfile"), m_DefaultProfile);
		m_pSettingsFile->SetLong(_T("General"), _T("AutoLogin"), m_AutoLogin);

		return CreateProfile(DEFAULT_PROFILE_NAME, Settings) != NULL;
	}
	
	/*! \brief Retrieves the profile specified by its name
		\param[in] pProfileName_in : the name of the profile
		\return a pointer to the profile if found; NULL otherwise
	*/
	WindowerProfile* SettingsManager::GetSettings(const TCHAR *pProfileName_in)
	{
		SettingsIterator SettingsIter;

		for (SettingsIter = m_Profiles.begin(); SettingsIter != m_Profiles.end(); ++SettingsIter)
		{
			if (_tcsicmp((*SettingsIter)->GetName(), pProfileName_in) == 0)
			{
				return *SettingsIter;
			}
		}

		return NULL;
	}

	/*! \brief Copy a profile to another given its name
		\param[in] pDstProfile_in : the name of the destination profile
		\param[in] Src_in : the source profile
		\return true if the profile was copied successfully; false otherwise
	*/
	bool SettingsManager::CopyProfile(const TCHAR *pDstProfile_in, const WindowerProfile &Src_in)
	{
		WindowerProfile *pProfile = GetSettings(pDstProfile_in);

		if (pProfile != NULL)
		{
			pProfile->SetResolution(Src_in.GetResX(), Src_in.GetResY());
			pProfile->SetPluginsDir(Src_in.GetPluginsDir());
			pProfile->SetVSync(Src_in.GetVSync());
			pProfile->SetName(Src_in.GetName());
		}

		return (pProfile != NULL);
	}
}