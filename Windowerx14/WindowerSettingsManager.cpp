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
	SettingsManager::SettingsManager(const TCHAR *pIniFile_in)
		: m_PluginsDir(DEFAULT_PLUGINS_DIR)
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
		WindowerSettings::const_iterator SettingsIter;

		for (SettingsIter = m_Profiles.begin(); SettingsIter != m_Profiles.end(); ++SettingsIter)
			delete SettingsIter->second;

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
		WindowerSettings::const_iterator ProfileIt = m_Profiles.find(pProfileName_in);
		WindowerProfile *pNewSettings = NULL;

		if (m_pSettingsFile != NULL && pProfileName_in != NULL)
		{
			if (ProfileIt == m_Profiles.end())
				m_Profiles[pProfileName_in] = pNewSettings = new WindowerProfile(Settings_in);
			else
				pNewSettings = ProfileIt->second;
		}

		return pNewSettings;
	}

	/*! \brief Deletes the profile specified by its name
		\param[in] pProfileName_in : the name of the profile
		\return true if the profile was deleted successfully; false otherwise
	*/
	bool SettingsManager::DeleteProfile(const TCHAR *pProfileName_in)
	{
		if (m_pSettingsFile != NULL && pProfileName_in != NULL)
		{
			WindowerSettings::const_iterator ProfileIt = m_Profiles.find(pProfileName_in);

			if (ProfileIt != m_Profiles.end())
			{
				string_t ProfileName(pProfileName_in);

				delete ProfileIt->second;
				m_Profiles.erase(ProfileIt);

				return m_pSettingsFile->DeleteSection(ProfileName);
			}
		}

		return false;
	}

	/*! \brief Saves the configuration
		\return true if successful; false otherwise
	*/
	bool SettingsManager::Save()
	{
		if (m_pSettingsFile != NULL)
		{
			WindowerSettings::const_iterator SettingsIter;
			WindowerProfile *pProfile;
			const TCHAR *pName;

			m_pSettingsFile->SetString(_T("General"), _T("CurrentProfile"), m_DefaultProfile);

			for (SettingsIter = m_Profiles.begin(); SettingsIter != m_Profiles.end(); ++SettingsIter)
			{
				pProfile = SettingsIter->second;

				if (pProfile != NULL)
				{
					pName = pProfile->GetName();

					m_pSettingsFile->SetLong(pName, _T("ResX"),  pProfile->GetResX());
					m_pSettingsFile->SetLong(pName, _T("ResY"),  pProfile->GetResY());
					m_pSettingsFile->SetLong(pName, _T("VSync"), pProfile->GetVSync());

					const ActivePlugins &Plugins = pProfile->GetActivePlugins();
					ActivePlugins::size_type Count = Plugins.size(), Index = 0;
					ActivePlugins::const_iterator PluginIt;
					string_t PluginList;
					
					for (PluginIt = Plugins.begin(); PluginIt != Plugins.end(); ++PluginIt, ++Index)
					{
						if (Index > 0 && Index < Count)
							PluginList += '|';

						PluginList += *PluginIt;
					}

					if (PluginList.empty() == false)
						m_pSettingsFile->SetString(pName, _T("Plugins"), PluginList);
				}
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
			Settings_out.SetName(pProfileName_in);

			string_t Plugins = m_pSettingsFile->GetString(pProfileName_in, _T("Plugins"));
			string_t::size_type Pos, Offset = 0;
			string_t Current;

			Pos = Plugins.find_first_of('|', Offset);

			while (Pos != string_t::npos)
			{
				Current = Plugins.substr(Offset, Pos - Offset);

				if (Current.empty() == false)
					Settings_out.ActivatePlugin(Current);

				Offset = Pos + 1;
				Pos = Plugins.find_first_of('|', Offset);
			}

			Current = Plugins.substr(Offset, Pos);
			// add the last item in the list
			if (Current.empty() == false)
				Settings_out.ActivatePlugin(Current);

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
		return LoadProfile(GetDefaultProfile(), Settings_out);
	}

	/*! \brief Loads the configuration
		\return true if successful; false otherwise
	*/
	bool SettingsManager::Load()
	{
		if (m_pSettingsFile != NULL)
		{
			WindowerSettings::iterator SettingsIt;
			CSimpleIni::TNamesDepend Sections;
			SectionsIterator SectionIter;
			bool Exists, Result = false;
			WindowerProfile *pSettings;
			

			SetDefaultProfile(m_pSettingsFile->GetString(_T("General"), _T("CurrentProfile"), DEFAULT_PROFILE_NAME));
			SetPluginsDir(m_pSettingsFile->GetString(_T("General"), _T("PluginsDir"), DEFAULT_PLUGINS_DIR));

			m_pSettingsFile->GetSections(Sections);

			for (SectionIter = Sections.begin(); SectionIter != Sections.end(); ++SectionIter)
			{
				if (_tcsstr(SectionIter->pItem, PROFILE_PREFIX) == SectionIter->pItem)
				{
					SettingsIt = m_Profiles.find(SectionIter->pItem);
					Exists = (SettingsIt != m_Profiles.end());

					if (Exists)
						pSettings = SettingsIt->second;
					else
						pSettings = new WindowerProfile();

					if (LoadProfile(SectionIter->pItem, *pSettings))
					{
						//SectionIter->

						m_Profiles[SectionIter->pItem] = pSettings;
						Result = true;
					}
					else
					{
						delete pSettings;

						if (Exists)
							m_Profiles.erase(SettingsIt);
					}
				}
			}

			return Result;
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
		WindowerProfile Settings(ResX, ResY, TRUE, DEFAULT_PROFILE_NAME);

		SetDefaultProfile(DEFAULT_PROFILE_NAME);
		m_pSettingsFile->SetString(_T("General"), _T("CurrentProfile"), m_DefaultProfile);

		return CreateProfile(DEFAULT_PROFILE_NAME, Settings) != NULL;
	}

	/*! \brief Retrieves the position of the profile specified by its name
		\param[in] pProfileName_in : the name of the profile
		\return the position of the profile
	*/
	WindowerSettings::const_iterator SettingsManager::GetSettingsPos(const TCHAR *pProfileName_in)
	{
		return m_Profiles.find(pProfileName_in);
	}

	/*! \brief Retrieves the profile specified by its name
		\param[in] pProfileName_in : the name of the profile
		\return a pointer to the profile if found; NULL otherwise
	*/
	WindowerProfile* SettingsManager::GetSettings(const TCHAR *pProfileName_in)
	{
		WindowerSettings::const_iterator ProfileIt = GetSettingsPos(pProfileName_in);

		if (ProfileIt != m_Profiles.end())
			return ProfileIt->second;

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
			pProfile->SetVSync(Src_in.GetVSync());
			pProfile->SetName(Src_in.GetName());
		}

		return (pProfile != NULL);
	}
}