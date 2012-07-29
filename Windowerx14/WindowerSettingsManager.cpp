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
	WindowerProfile* SettingsManager::DuplicateProfile(const TCHAR *pProfileName_in, const WindowerProfile &Settings_in)
	{
		WindowerSettings::const_iterator ProfileIt = m_Profiles.find(pProfileName_in);
		WindowerProfile *pNewSettings = NULL;

		if (m_pSettingsFile != NULL && pProfileName_in != NULL)
		{
			string_t NewName = pProfileName_in;

			// a plugin with the same name already exists
			if (ProfileIt != m_Profiles.end())
				CheckDuplicates(ProfileIt->first, NewName);

			m_Profiles[NewName] = pNewSettings = new WindowerProfile(Settings_in);
			// update the internal name to match the new one
			pNewSettings->SetName(NewName.c_str());
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

				// add the old profile to the delete profiles
				m_DeletedProfiles.insert(ProfileName);

				return true;
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
			std::set<string_t>::const_iterator DeleteIter;
			WindowerProfile *pProfile = NULL;
			const TCHAR *pName;

			for (SettingsIter = m_Profiles.begin(); SettingsIter != m_Profiles.end(); ++SettingsIter)
			{
				pProfile = SettingsIter->second;

				if (pProfile != NULL)
				{
					LONG ResX = pProfile->GetResX();
					LONG ResY = pProfile->GetResY();
					pName = pProfile->GetName();

					// the engine forces a minimal resolution of 1024x720
					if (ResX < 1024L)
						ResX = 1024L;
					if (ResY < 720L)
						ResY = 720L;

					m_pSettingsFile->SetLong(pName, _T("ResX"), ResX);
					m_pSettingsFile->SetLong(pName, _T("ResY"), ResY);
					m_pSettingsFile->SetLong(pName, _T("VSync"), pProfile->GetVSync());
					m_pSettingsFile->SetLong(pName, _T("Borderless"), pProfile->GetBorderless());

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

			// remove the sections of the delete profiles
			for (DeleteIter = m_DeletedProfiles.begin(); DeleteIter != m_DeletedProfiles.end(); ++DeleteIter)
				m_pSettingsFile->DeleteSection(*DeleteIter);
			// reset the delete profiles
			m_DeletedProfiles.clear();

			// only update the default profile if it exists
			if (GetSettings(m_DefaultProfile.c_str()) == NULL && m_Profiles.empty() == false)
				m_DefaultProfile = m_Profiles.begin()->first;

			m_pSettingsFile->SetString(_T("General"), _T("CurrentProfile"), m_DefaultProfile);

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
			LONG ResX = m_pSettingsFile->GetLong(pProfileName_in, _T("ResX"), GetSystemMetrics(SM_CXSCREEN));
			LONG ResY = m_pSettingsFile->GetLong(pProfileName_in, _T("ResY"), GetSystemMetrics(SM_CYSCREEN));
			bool ForceSave = false;

			if (ResX < 1024)
			{
				m_pSettingsFile->SetLong(pProfileName_in, _T("ResX"), 1024);
				ForceSave = true;
				ResX = 1024;
			}
			if (ResY < 720)
			{
				m_pSettingsFile->SetLong(pProfileName_in, _T("ResY"), 720);
				ForceSave = true;
				ResY = 720;
			}

			if (ForceSave)
				m_pSettingsFile->Save();

			Settings_out.SetBorderless(m_pSettingsFile->GetLong(pProfileName_in, _T("Borderless"), 1L));
			Settings_out.SetVSync(m_pSettingsFile->GetLong(pProfileName_in, _T("VSync"), 0L));
			Settings_out.SetResolution(ResX, ResY);
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

	/*! \brief Forces the configuration file to be reloaded
		\return true if successful; false otherwise
	*/
	bool SettingsManager::Reload()
	{
		if (m_pSettingsFile != NULL)
			return m_pSettingsFile->Reload();

		return false;
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
		WindowerProfile Settings(ResX, ResY, TRUE, TRUE, DEFAULT_PROFILE_NAME);

		SetDefaultProfile(DEFAULT_PROFILE_NAME);
		m_pSettingsFile->SetString(_T("General"), _T("CurrentProfile"), m_DefaultProfile);

		return DuplicateProfile(DEFAULT_PROFILE_NAME, Settings) != NULL;
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
			pProfile->SetBorderless(Src_in.GetBorderless());
			pProfile->SetVSync(Src_in.GetVSync());
			pProfile->SetName(Src_in.GetName());
		}

		return (pProfile != NULL);
	}

	
	/*! \brief Checks if the profile name is a duplicate of an existing profile
		\param[in] CurrentName_in : the current name of the profile
		\param[in,out] Name_in_out : the name to check for duplicates		
		\return true if the name was a duplicate; false otherwise
	*/
	bool SettingsManager::CheckDuplicates(const string_t &CurrentName_in, string_t &Name_in_out)
	{
		const WindowerProfile *pOtherProfile = GetSettings(Name_in_out.c_str());
		bool Result = (pOtherProfile != NULL);
		string_t NewName(Name_in_out);
		unsigned long Counter = 0UL;		

		// change the new name until it's unique
		while (pOtherProfile != NULL)
		{
			// create a new name
			format(NewName, _T("%s%ld"), Name_in_out.c_str(), ++Counter);
			// are we back to where we started?
			if (_tcscmp(NewName.c_str(), CurrentName_in.c_str()) == 0)
			{
				// keep the old name
				pOtherProfile = NULL;
				Result = false;
			}
			else
			{
				// check if the new name already exists
				pOtherProfile = GetSettings(NewName.c_str());
				Result = true;
			}
		}
		// update the name
		Name_in_out = NewName;

		return Result;
	}

	/*! \brief Renames a profile and ensures the name is not a duplicate of any current profile
		\param[in] pProfileName_in : the current name of the profile
		\param[in] pNewName_in_out : the new name of the profile
		\return true if the profile was found and renamed; false otherwise
	*/
	bool SettingsManager::RenameProfile(const TCHAR *pProfileName_in, string_t &NewName_in_out)
	{
		WindowerProfile *pProfile = GetSettings(pProfileName_in);

		if (pProfile != NULL)
			return RenameProfile(pProfile, NewName_in_out);

		return false;
	}

	/*! \brief Renames a profile and ensures the name is not a duplicate of any current profile
		\param[in] pProfileName_in : the current name of the profile
		\param[in] pNewName_in_out : the new name of the profile
		\return true if the profile was found and renamed; false otherwise
	*/
	bool SettingsManager::RenameProfile(WindowerProfile *pProfile_in_out, string_t &NewName_in_out)
	{
		if (pProfile_in_out != NULL)
		{
			const WindowerProfile *pOtherProfile = GetSettings(NewName_in_out.c_str());
			string_t CurrentName = pProfile_in_out->GetName();

			if (pProfile_in_out != pOtherProfile)
			{
				// check for duplicates
				CheckDuplicates(CurrentName, NewName_in_out);
				// check if the name has changed
				if (_tcscmp(CurrentName.c_str(), NewName_in_out.c_str()) != 0)
				{
					// move the profile in the map
					WindowerSettings::const_iterator Iter = GetSettingsPos(CurrentName.c_str());

					if (Iter != m_Profiles.end())
					{
						// rename the profile internally
						pProfile_in_out->SetName(NewName_in_out.c_str());
						// erase the current entry
						m_Profiles.erase(Iter);
						// insert the profile again under its new name
						m_Profiles[NewName_in_out] = pProfile_in_out;
						// add the old profile to the delete profiles
						m_DeletedProfiles.insert(CurrentName);

						return true;
					}
				}
			}
		}

		return false;
	}
}