/**************************************************************************
	created		:	2011-06-03
	filename	: 	WindowerSettingsManager.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower settings manager
**************************************************************************/
#include "stdafx.h"
#include <shlobj.h>

#include "WindowerSettings.h"
#include "WindowerSettingsManager.h"

namespace Windower
{
	/*! \brief SettingsManager constructor
		\param[in] pIniFile_in : the path of the configuration file
	*/
	SettingsManager::SettingsManager(const TCHAR *pWorkingDir_in, const TCHAR *pIniFile_in)
		: m_WorkingDir(pWorkingDir_in)
	{
		bool bEmpty = true;

		if (pIniFile_in != NULL && pWorkingDir_in != NULL)
		{
			if (m_WorkingDir.back() != '\\')
				m_WorkingDir += '\\';

			m_ConfigPath = m_WorkingDir + pIniFile_in;
			m_pSettingsFile = new Settings::SettingsIniFile(m_ConfigPath);

			if (m_pSettingsFile->Load())
				bEmpty = !Load();

			if (bEmpty)
			{
				CreateDefaultProfile(INI_DEFAULT_CURRENT_PROFILE);
				Save();
			}
		}
		else
			m_pSettingsFile = NULL;
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
					pName = pProfile->GetName();

					m_pSettingsFile->SetLong(pName, INI_KEY_VSYNC, pProfile->GetVSync());
					m_pSettingsFile->SetLong(pName, INI_KEY_LNG, pProfile->GetLanguage(), INI_COMMENT_LNG);

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
						m_pSettingsFile->SetString(pName, INI_KEY_PLUGINS, PluginList);
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

			m_pSettingsFile->SetString(INI_SECTION_GENERAL, INI_KEY_CURRENT_PROFILE, m_DefaultProfile);
			m_pSettingsFile->SetLong(INI_SECTION_GENERAL, INI_AUTO_UPDATE, m_bAutoUpdate ? 1L : 0L);
			m_pSettingsFile->SetString(INI_SECTION_GENERAL, INI_KEY_GAME_PATH, m_GamePath);

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
		if (pProfileName_in != NULL && m_pSettingsFile != NULL && m_pSettingsFile->SectionExists(pProfileName_in))
		{
			Settings_out.SetVSync(m_pSettingsFile->GetLong(pProfileName_in, INI_KEY_VSYNC, INI_DEFAULT_VSYNC) == 1L);
			Settings_out.SetLanguage(m_pSettingsFile->GetLong(pProfileName_in, INI_KEY_LNG, INI_DEFAULT_LNG));
			Settings_out.SetName(pProfileName_in);

			string_t Plugins = m_pSettingsFile->GetString(pProfileName_in, INI_KEY_PLUGINS, INI_DEFAULT_PLUGINS);
			std::list<string_t>::const_iterator PluginIt, EndIt;
			std::list<string_t> PluginList;

			tokenize<wchar_t>(Plugins, PluginList, _T("|"), _T("\0"));

			for (PluginIt = PluginList.cbegin(), EndIt = PluginList.cend(); PluginIt != EndIt; ++PluginIt)
				Settings_out.ActivatePlugin(*PluginIt);

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
		return (m_pSettingsFile != NULL) ? m_pSettingsFile->Reload() : false;
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

			// verify the configuration
			VerifyConfig();
			// get the general parameters
			SetDefaultProfile(m_pSettingsFile->GetString(INI_SECTION_GENERAL, INI_KEY_CURRENT_PROFILE, INI_DEFAULT_CURRENT_PROFILE));
			SetAutoUpdate(m_pSettingsFile->GetLong(INI_SECTION_GENERAL, INI_AUTO_UPDATE, INI_DEFAULT_AUTO_UPDATE) != 0L);
			SetGamePath(m_pSettingsFile->GetString(INI_SECTION_GENERAL, INI_KEY_GAME_PATH, INI_DEFAULT_GAME_PATH));			
			// load the sections
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
	bool SettingsManager::CreateDefaultProfile(const TCHAR *pProfileName_in)
	{
		if (m_pSettingsFile != NULL && pProfileName_in != NULL)
		{
			WindowerProfile Settings(pProfileName_in, INI_DEFAULT_LNG, INI_DEFAULT_VSYNC);

			m_pSettingsFile->SetString(INI_SECTION_GENERAL, INI_KEY_CURRENT_PROFILE, pProfileName_in);

			SetDefaultProfile(pProfileName_in);

			return (DuplicateProfile(pProfileName_in, Settings) != NULL);
		}

		return false;
	}

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
	bool SettingsManager::CheckDuplicates(const string_t& CurrentName_in, string_t& Name_in_out)
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
	bool SettingsManager::RenameProfile(const TCHAR *pProfileName_in, string_t& NewName_in_out)
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
	bool SettingsManager::RenameProfile(WindowerProfile *pProfile_in_out, string_t& NewName_in_out)
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

	/*! \brief Asks the user to select a folder
		\param[out] SelectedDir_out : a string receiving the path to the selected folder
		\return true if the path was selected; false otherwise
	*/
	bool SettingsManager::SelectDirectory(string_t& SelectedDir_out) const
	{
		LPCITEMIDLIST pSelectedPIDL = NULL;
		LPITEMIDLIST PIDL = NULL;
		BROWSEINFO BrowseInfo;
		bool Result = false;

		SecureZeroMemory(&BrowseInfo, sizeof(BrowseInfo));

		CoInitialize(NULL);

		if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, &PIDL)))
		{
			BrowseInfo.hwndOwner = NULL;
			BrowseInfo.pidlRoot = PIDL;
			BrowseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NONEWFOLDERBUTTON | BIF_USENEWUI | BIF_RETURNFSANCESTORS;
			BrowseInfo.lpszTitle = _T("Please select the Final Fantasy XIV installation folder or type the complete path in the edit box below:");

			// select the directory manually
			pSelectedPIDL = SHBrowseForFolder(&BrowseInfo);

			if (pSelectedPIDL != NULL)
			{
				LPTSTR pPathBuffer = new TCHAR[_MAX_PATH];

				if (SHGetPathFromIDList(pSelectedPIDL, pPathBuffer))
				{
					SelectedDir_out.assign(pPathBuffer);

					if (SelectedDir_out.back() != '\\')
						SelectedDir_out += '\\';

					Result = true;				
				}

				delete[] pPathBuffer;
			}
		}

		CoUninitialize();

		return Result;
	}

	/*! \brief Checks that the specified game path contains a 'boot' and 'game' directory
		\return true if the path is valid; false otherwise
	*/
	bool SettingsManager::CheckGamePath(const string_t& GamePath_in) const
	{
		string_t BootPath = GamePath_in + BOOT_SUBFOLDER;
		string_t GamePath = GamePath_in + GAME_SUBFOLDER;
		DWORD Attributes;		

		Attributes = GetFileAttributes(BootPath.c_str());

		if (Attributes == INVALID_FILE_ATTRIBUTES || (Attributes & FILE_ATTRIBUTE_DIRECTORY) == 0UL)
			return false;

		Attributes = GetFileAttributes(GamePath.c_str());

		if (Attributes == INVALID_FILE_ATTRIBUTES || (Attributes & FILE_ATTRIBUTE_DIRECTORY) == 0UL)
			return false;

		return true;
	}

	bool SettingsManager::VerifyConfig()
	{
		if (m_pSettingsFile != NULL)
		{
			const TCHAR *pFirstProfile = NULL;
			bool bSave = false;

			// 1. Check that the general section exists
			if (m_pSettingsFile->SectionExists(INI_SECTION_GENERAL) == false)
			{
				m_pSettingsFile->SetString(INI_SECTION_GENERAL, INI_KEY_CURRENT_PROFILE, INI_DEFAULT_CURRENT_PROFILE);
				m_pSettingsFile->SetString(INI_SECTION_GENERAL, INI_KEY_GAME_PATH, INI_DEFAULT_GAME_PATH);
				bSave = true;
			}

			// 2. Remove outdated entries
			const TCHAR *pOutdatedKeys[3] = { _T("ResX"), _T("ResY"), _T("Borderless") };
			CSimpleIni::TNamesDepend Sections;
			SectionsIterator SectionIter;

			// load the sections
			m_pSettingsFile->GetSections(Sections);

			for (SectionIter = Sections.begin(); SectionIter != Sections.end(); ++SectionIter)
			{
				if (_tcsstr(SectionIter->pItem, PROFILE_PREFIX) == SectionIter->pItem)
				{
					if (pFirstProfile == NULL)
						pFirstProfile = SectionIter->pItem;

					for (int i = 0; i < 3; ++i)
						bSave |= m_pSettingsFile->DeleteKey(SectionIter->pItem, pOutdatedKeys[i]);
				}
			}

			// 3. Check if the default profile exists
			string_t DefaultProfile = m_pSettingsFile->GetString(INI_SECTION_GENERAL, INI_KEY_CURRENT_PROFILE, INI_DEFAULT_CURRENT_PROFILE);

			if (DefaultProfile.empty())
			{
				// set the first profile as default
				if (pFirstProfile == NULL)
					pFirstProfile = INI_DEFAULT_CURRENT_PROFILE;

				SetDefaultProfile(pFirstProfile);
				DefaultProfile = pFirstProfile;				
				bSave = true;
			}

			if (m_pSettingsFile->SectionExists(DefaultProfile) == false)
				bSave |= CreateDefaultProfile(DefaultProfile.c_str());

			if (bSave)
				Save();

			// 4. Check if the game path is correct
			return CheckGamePath(m_pSettingsFile->GetString(INI_SECTION_GENERAL, INI_KEY_GAME_PATH, INI_DEFAULT_GAME_PATH));
		}

		return false;
	}

	void SettingsManager::SetDefaultProfile(const TCHAR *pProfileName_in)
	{ m_DefaultProfile = pProfileName_in; }
}