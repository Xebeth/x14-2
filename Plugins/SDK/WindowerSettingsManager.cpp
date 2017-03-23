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
		: m_bAutoUpdate(false), m_bIsLoaded(false)
	{
		bool bEmpty = true;

		if (pIniFile_in != nullptr && pWorkingDir_in != nullptr)
		{
			initialize_path(pWorkingDir_in, m_WorkingDir);
			m_ConfigPath = m_WorkingDir + pIniFile_in;

			m_pSettingsFile = new Settings::SettingsIniFile(m_ConfigPath);

			if (m_pSettingsFile->Load())
				bEmpty = !Load();

			if (bEmpty)
			{
				CreateDefault(INI_DEFAULT_CURRENT_PROFILE);
				Save();
			}
		}
		else
			m_pSettingsFile = nullptr;
	}

	//! \brief SettingsManager destructor
	SettingsManager::~SettingsManager()
	{
		WindowerSettings::const_iterator SettingsIter, EndIt = m_Profiles.cend();

		for (SettingsIter = m_Profiles.cbegin(); SettingsIter != EndIt; ++SettingsIter)
			delete SettingsIter->second;

		m_Profiles.clear();

		if (m_pSettingsFile != nullptr)
		{
			delete m_pSettingsFile;
			m_pSettingsFile = nullptr;
		}
	}

	/*! \brief Creates a copy of the specified profile
		\param[in] pProfileName_in : the name of the profile
		\param[in] Settings_in : the settings being copied
		\return the newly created profile if successful; nullptr otherwise
	*/
	WindowerProfile* SettingsManager::DuplicateProfile(const TCHAR *pProfileName_in, const WindowerProfile &Settings_in)
	{
		WindowerSettings::const_iterator ProfileIt = m_Profiles.find(pProfileName_in);
		WindowerProfile *pNewSettings = nullptr;

		if (m_pSettingsFile != nullptr && pProfileName_in != nullptr)
		{
			string_t NewName = pProfileName_in;

			// a plugin with the same name already exists
			if (ProfileIt != m_Profiles.cend())
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
		if (m_pSettingsFile != nullptr && pProfileName_in != nullptr)
		{
			WindowerSettings::const_iterator ProfileIt = m_Profiles.find(pProfileName_in);

			if (ProfileIt != m_Profiles.cend())
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
		if (m_pSettingsFile != nullptr)
		{
			std::set<string_t>::const_iterator DeleteIter, DeleteEndIt = m_DeletedProfiles.cend();
			WindowerSettings::const_iterator SettingsIter, EndIt = m_Profiles.cend();
			WindowerProfile *pProfile = nullptr;


			for (SettingsIter = m_Profiles.cbegin(); SettingsIter != EndIt; ++SettingsIter)
			{
				pProfile = SettingsIter->second;

				if (pProfile != nullptr)
					SaveSettings(pProfile);
			}

			// remove the sections of the delete profiles
			for (DeleteIter = m_DeletedProfiles.cbegin(); DeleteIter != DeleteEndIt; ++DeleteIter)
				m_pSettingsFile->DeleteSection(*DeleteIter);
			// reset the delete profiles
			m_DeletedProfiles.clear();

			// only update the default profile if it exists
			if (GetSettings(m_DefaultProfile.c_str()) == nullptr && m_Profiles.empty() == false)
				m_DefaultProfile = m_Profiles.cbegin()->first;

			m_pSettingsFile->SetString(INI_SECTION_GENERAL, INI_KEY_CURRENT_PROFILE, m_DefaultProfile);
			m_pSettingsFile->SetLong(INI_SECTION_GENERAL, INI_AUTO_UPDATE, m_bAutoUpdate ? 1L : 0L);
			m_pSettingsFile->SetString(INI_SECTION_GENERAL, INI_KEY_GAME_PATH, normalize_path(m_GamePath));

			if (m_ScoredWords.empty() == false)
			{
				ScoredWords::const_iterator WordIt, WordEndIt = m_ScoredWords.cbegin();

				for (WordIt = --m_ScoredWords.cend(); WordIt != WordEndIt; --WordIt)
					WordIt = UpdateScoredWord(WordIt);

				if (WordIt == WordEndIt)
					UpdateScoredWord(WordIt);
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
		if (pProfileName_in != nullptr && m_pSettingsFile != nullptr && m_pSettingsFile->SectionExists(pProfileName_in))
		{
			Settings_out.SetBlacklistThreshold(GetLong(pProfileName_in, INI_KEY_BLACKLIST_THRESHOLD));
			Settings_out.SetBlacklistCount(GetLong(pProfileName_in, INI_KEY_BLACKLIST_COUNT));
			Settings_out.SetTimestampFormat(GetString(pProfileName_in, INI_KEY_TIMESTAMP));
			Settings_out.SetCryptedPassword(GetString(pProfileName_in, INI_KEY_PASSWORD));
			Settings_out.SetTellSound(GetString(pProfileName_in, INI_KEY_TELL_SOUND));
			Settings_out.SetAutoSubmit(GetBool(pProfileName_in, INI_KEY_AUTO_SUBMIT));
			Settings_out.SetPluginList(GetString(pProfileName_in, INI_KEY_PLUGINS));
			Settings_out.SetKeyHash(GetUnsignedLong(pProfileName_in, INI_KEY_HASH));
			Settings_out.SetUsername(GetString(pProfileName_in, INI_KEY_USERNAME));
			Settings_out.SetVSync(GetBool(pProfileName_in, INI_KEY_VSYNC));
			Settings_out.SetLanguage(GetLong(pProfileName_in, INI_KEY_LNG));
			Settings_out.SetVSync(GetBool(pProfileName_in, INI_KEY_VSYNC));
			Settings_out.SetScoredWords(&m_ScoredWords);
			Settings_out.SetName(pProfileName_in);

			LoadLabels(Settings_out);

			return true;
		}

		return false;
	}

	void SettingsManager::LoadLabels(WindowerProfile &Settings_out)
	{
		string_t SectionName = LABELS_PREFIX;

		SectionName += Settings_out.GetName() + _tcslen(PROFILE_PREFIX);

		if (m_pSettingsFile->SectionExists(SectionName))
		{
			SectionsConstIterator SectionIt, EndIt;
			CSimpleIni::TNamesDepend Values;

			if (m_pSettingsFile->GetAllKeys(SectionName, Values))
			{
				SectionsConstIterator LabelIt, LabelEndIt = Values.cend();

				for (LabelIt = Values.cbegin(); LabelIt != LabelEndIt; ++LabelIt)
 					Settings_out.LoadLabel(LabelIt->pItem, m_pSettingsFile->GetString(SectionName, LabelIt->pItem));
			}
		}
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
		return (m_pSettingsFile != nullptr) ? m_pSettingsFile->Reload() : false;
	}

	/*! \brief Loads the configuration
		\return true if successful; false otherwise
	*/
	bool SettingsManager::Load()
	{
		if (m_pSettingsFile != nullptr)
		{
			WindowerSettings::const_iterator SettingsIt;
			CSimpleIni::TNamesDepend Sections;
			WindowerProfile *pSettings;
			bool Exists;

			// get the general parameters
			SetDefaultProfile(m_pSettingsFile->GetString(INI_SECTION_GENERAL, INI_KEY_CURRENT_PROFILE, INI_KEY_CURRENT_PROFILE));
			SetAutoUpdate(m_pSettingsFile->GetLong(INI_SECTION_GENERAL, INI_AUTO_UPDATE, INI_DEFAULT_AUTO_UPDATE) == 1L);
			SetGamePath(m_pSettingsFile->GetString(INI_SECTION_GENERAL, INI_KEY_GAME_PATH, INI_KEY_GAME_PATH));			
			// load the sections
			m_pSettingsFile->GetSections(Sections);

			SectionsConstIterator SectionIt, EndIt;
			CSimpleIni::TNamesDepend Values;

			if (m_pSettingsFile->GetAllKeys(INI_SECTION_AUTO_BLACKLIST, Values))
			{
				std::string Word;

				EndIt = Values.cend();

				for (SectionIt = Values.cbegin(); SectionIt != EndIt; ++SectionIt)
				{
					convert_ansi(SectionIt->pItem, Word, CP_UTF8);
					m_ScoredWords[Word] = m_pSettingsFile->GetLong(INI_SECTION_AUTO_BLACKLIST, SectionIt->pItem);
				}
			}

			EndIt = Sections.cend();

			for (SectionIt = Sections.cbegin(); SectionIt !=  EndIt; ++SectionIt)
			{
				if (_tcsstr(SectionIt->pItem, PROFILE_PREFIX) == SectionIt->pItem)
				{
					SettingsIt = m_Profiles.find(SectionIt->pItem);
					Exists = (SettingsIt != m_Profiles.cend());

					if (Exists)
						pSettings = SettingsIt->second;
					else
						pSettings = new WindowerProfile();

					if (LoadProfile(SectionIt->pItem, *pSettings))
					{
						m_Profiles[SectionIt->pItem] = pSettings;
						m_bIsLoaded = true;
					}
					else
					{
						delete pSettings;

						if (Exists)
							m_Profiles.erase(SettingsIt);
					}
				}				
			}

			return (m_bIsLoaded && VerifyConfig());
		}

		return false;
	}

	/*! \brief Creates the default data
		\return true if successful; false otherwise
	*/
	bool SettingsManager::CreateDefault(const TCHAR *pProfileName_in)
	{
		if (m_pSettingsFile != nullptr && pProfileName_in != nullptr)
		{
			WindowerProfile *pSettings = new WindowerProfile();

			m_pSettingsFile->SetString(INI_SECTION_GENERAL, INI_KEY_CURRENT_PROFILE, pProfileName_in);
			m_pSettingsFile->SetString(INI_SECTION_GENERAL, INI_KEY_GAME_PATH, INI_KEY_GAME_PATH);
			m_pSettingsFile->SetLong(INI_SECTION_GENERAL, INI_AUTO_UPDATE, INI_DEFAULT_AUTO_UPDATE);

			m_pSettingsFile->CreateSection(pProfileName_in);
			SetDefaultProfile(pProfileName_in);			

			if (LoadProfile(pProfileName_in, *pSettings))
				m_Profiles[pProfileName_in] = pSettings;

			return true;
		}

		return false;
	}

	bool SettingsManager::CreateDefaultScoredWords()
	{
		std::string ansi;
		
		convert_ansi(_T("\xD0\xA8\xD0\xA8\xD0\xA8"), ansi);
		m_ScoredWords[ansi.c_str()] = 5;
		convert_ansi(_T("g\xD0\xBEld"), ansi);
		m_ScoredWords[ansi.c_str()] = 5;
		convert_ansi(_T("c\xD0\xBEm"), ansi);
		m_ScoredWords[ansi.c_str()] = 5;
		convert_ansi(_T("c\xC3\xB6m"), ansi);
		m_ScoredWords[ansi.c_str()] = 5;
		m_ScoredWords["c 0 m"] = 5;
		m_ScoredWords["c o m"] = 5;
		m_ScoredWords["w w w"] = 5;
		m_ScoredWords["us_d"] = 5;
		m_ScoredWords["g0id"] = 5;
		m_ScoredWords["goid"] = 5;
		m_ScoredWords["wvvw"] = 5;
		m_ScoredWords["gii"] = 5;
		m_ScoredWords["c0m"] = 5;
		m_ScoredWords["(om"] = 5;
		m_ScoredWords["gii"] = 5;
		m_ScoredWords["U$D"] = 5;
				
		m_ScoredWords["handwork"] = 3;
		m_ScoredWords["discount"] = 3;
		m_ScoredWords["delivery"] = 3;
		m_ScoredWords["instant"] = 3;
		m_ScoredWords["secure"] = 3;
		m_ScoredWords["legal"] = 3;
		m_ScoredWords["% off"] = 3;
		m_ScoredWords["%off"] = 3;
		m_ScoredWords["gold"] = 3;
		
		m_ScoredWords["reliable"] = 2;
		m_ScoredWords["minute"] = 2;
		m_ScoredWords["price"] = 2;
		m_ScoredWords["bonus"] = 2;
		m_ScoredWords["extra"] = 2;
		m_ScoredWords["fast"] = 2;
		m_ScoredWords["safe"] = 2;
		m_ScoredWords["gift"] = 2;
		m_ScoredWords["com"] = 2;
		m_ScoredWords["www"] = 2;
		m_ScoredWords["min"] = 2;
		
		m_ScoredWords["in-game"] = 1;
		m_ScoredWords["website"] = 1;
		m_ScoredWords["dolla"] = 1;
		m_ScoredWords["free"] = 1;
		m_ScoredWords["gil"] = 1;
		m_ScoredWords["usd"] = 1;
		m_ScoredWords["off"] = 1;
		m_ScoredWords["$"] = 1;
		m_ScoredWords["%"] = 1;
		
		return true;
	}

	const ScoredWords::const_iterator& SettingsManager::UpdateScoredWord(ScoredWords::const_iterator &WordIt_in)
	{
		if (WordIt_in != m_ScoredWords.cend())
		{
			string_t Word;
			convert_utf8(WordIt_in->first, Word);
			long Score = WordIt_in->second;

			if (Score < 1L)
			{
				m_pSettingsFile->DeleteKey(INI_SECTION_AUTO_BLACKLIST, Word);
				WordIt_in = m_ScoredWords.erase(WordIt_in);
			}
			else
				m_pSettingsFile->SetLong(INI_SECTION_AUTO_BLACKLIST, Word, Score);
		}

		return WordIt_in;		
	}

	WindowerSettings::const_iterator SettingsManager::GetSettingsPos(const TCHAR *pProfileName_in) const
	{
		return m_Profiles.find(pProfileName_in);
	}

	/*! \brief Retrieves the profile specified by its name
		\param[in] pProfileName_in : the name of the profile
		\return a pointer to the profile if found; nullptr otherwise
	*/
	WindowerProfile* SettingsManager::GetSettings(const TCHAR *pProfileName_in) const
	{
		if (pProfileName_in == nullptr)
			pProfileName_in = GetDefaultProfile();

		WindowerSettings::const_iterator ProfileIt = GetSettingsPos(pProfileName_in);

		if (ProfileIt != m_Profiles.cend())
			return ProfileIt->second;

		return nullptr;
	}

	/*! \brief Copy a profile to another given its name
		\param[in] pDstProfile_in : the name of the destination profile
		\param[in] Src_in : the source profile
		\return true if the profile was copied successfully; false otherwise
	*/
	bool SettingsManager::CopyProfile(const TCHAR *pDstProfile_in, const WindowerProfile &Src_in)
	{
		WindowerProfile *pProfile = GetSettings(pDstProfile_in);

		if (pProfile != nullptr)
			pProfile->Copy(Src_in);

		return (pProfile != nullptr);
	}

	
	/*! \brief Checks if the profile name is a duplicate of an existing profile
		\param[in] CurrentName_in : the current name of the profile
		\param[in,out] Name_in_out : the name to check for duplicates		
		\return true if the name was a duplicate; false otherwise
	*/
	bool SettingsManager::CheckDuplicates(const string_t& CurrentName_in, string_t& Name_in_out)
	{
		const WindowerProfile *pOtherProfile = GetSettings(Name_in_out.c_str());
		bool Result = (pOtherProfile != nullptr);
		string_t NewName(Name_in_out);
		unsigned long Counter = 0UL;		

		// change the new name until it's unique
		while (pOtherProfile != nullptr)
		{
			// create a new name
			format(NewName, _T("%s%ld"), Name_in_out.c_str(), ++Counter);
			// are we back to where we started?
			if (_tcscmp(NewName.c_str(), CurrentName_in.c_str()) == 0)
			{
				// keep the old name
				pOtherProfile = nullptr;
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

		if (pProfile != nullptr)
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
		if (pProfile_in_out != nullptr)
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
					WindowerSettings::const_iterator SettingsIt = GetSettingsPos(CurrentName.c_str());

					if (SettingsIt != m_Profiles.cend())
					{
						// rename the profile internally
						pProfile_in_out->SetName(NewName_in_out.c_str());
						// erase the current entry
						m_Profiles.erase(SettingsIt);
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
		LPCITEMIDLIST pSelectedPIDL = nullptr;
		LPITEMIDLIST PIDL = nullptr;
		BROWSEINFO BrowseInfo;
		bool Result = false;

		if (FAILED(::CoInitialize(nullptr)))
			return false;

		SecureZeroMemory(&BrowseInfo, sizeof(BrowseInfo));

		if (SUCCEEDED(SHGetSpecialFolderLocation(nullptr, CSIDL_DRIVES, &PIDL)))
		{
			BrowseInfo.hwndOwner = nullptr;
			BrowseInfo.pidlRoot = PIDL;
			BrowseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NONEWFOLDERBUTTON | BIF_USENEWUI | BIF_RETURNFSANCESTORS;
			BrowseInfo.lpszTitle = _T("Please select the Final Fantasy XIV installation folder or type the complete path in the edit box below:");

			// select the directory manually
			pSelectedPIDL = SHBrowseForFolder(&BrowseInfo);

			if (pSelectedPIDL != nullptr)
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
		if (m_pSettingsFile != nullptr)
		{
			const TCHAR *pFirstProfile = nullptr;
			bool bSave = false;

			// 1. Check that the general section exists
			if (m_pSettingsFile->SectionExists(INI_SECTION_GENERAL) == false)
			{
				m_pSettingsFile->SetString(INI_SECTION_GENERAL, INI_KEY_CURRENT_PROFILE, INI_KEY_CURRENT_PROFILE);
				m_pSettingsFile->SetString(INI_SECTION_GENERAL, INI_KEY_GAME_PATH, INI_KEY_GAME_PATH);
				bSave = true;
			}

			// 2. Remove outdated entries
			const TCHAR *pOutdatedKeys[3] = { _T("ResX"), _T("ResY"), _T("Borderless") };
			SectionsConstIterator SectionIter, EndIt;
			CSimpleIni::TNamesDepend Sections;
			
			// load the sections
			m_pSettingsFile->GetSections(Sections);
			EndIt = Sections.cend();

			for (SectionIter = Sections.cbegin(); SectionIter != EndIt; ++SectionIter)
			{
				if (_tcsstr(SectionIter->pItem, PROFILE_PREFIX) == SectionIter->pItem)
				{
					if (pFirstProfile == nullptr)
						pFirstProfile = SectionIter->pItem;

					for (int i = 0; i < 3; ++i)
						bSave |= m_pSettingsFile->DeleteKey(SectionIter->pItem, pOutdatedKeys[i]);
				}
			}

			// 3. Check if the default profile exists
			string_t DefaultProfile = m_pSettingsFile->GetString(INI_SECTION_GENERAL, INI_KEY_CURRENT_PROFILE, INI_KEY_CURRENT_PROFILE);

			if (DefaultProfile.empty())
			{
				// set the first profile as default
				if (pFirstProfile == nullptr)
					pFirstProfile = INI_KEY_CURRENT_PROFILE;

				SetDefaultProfile(pFirstProfile);
				DefaultProfile = pFirstProfile;				
				bSave = true;
			}

			if (m_pSettingsFile->SectionExists(DefaultProfile) == false)
				bSave |= CreateDefault(DefaultProfile.c_str());

			if (m_pSettingsFile->SectionExists(INI_SECTION_AUTO_BLACKLIST) == false)
				bSave |= CreateDefaultScoredWords();

			if (bSave)
				Save();

			// 4. Check if the game path is correct
			return CheckGamePath(m_pSettingsFile->GetString(INI_SECTION_GENERAL, INI_KEY_GAME_PATH, INI_KEY_GAME_PATH));
		}

		return false;
	}

	void SettingsManager::SetDefaultProfile(const TCHAR *pProfileName_in)
	{ m_DefaultProfile = pProfileName_in; }

	/*! \brief Checks if the configuration file is loaded
		\return true if the configuration file is loaded; false otherwise
	*/
	bool SettingsManager::IsConfigLoaded() const
	{
		return (m_bIsLoaded && m_pSettingsFile != nullptr && m_pSettingsFile->IsConfigLoaded());
	}

	bool SettingsManager::SaveSettings(WindowerProfile *pSettings_in)
	{
		if (pSettings_in != nullptr)
		{
			const TCHAR *pName = pSettings_in->GetName();
			string_t SectionName = LABELS_PREFIX, LabelSettings;
			const TextLabels &Labels = pSettings_in->GetTextLabels();
			TextLabels::const_iterator LabelIter, LabelEndIt = Labels.cend();

			SectionName += pSettings_in->GetName() + _tcslen(PROFILE_PREFIX);

			SetLong(pName, INI_KEY_BLACKLIST_THRESHOLD, pSettings_in->GetBlacklistThreshold());
			SetLong(pName, INI_KEY_BLACKLIST_COUNT, pSettings_in->GetBlacklistCount());
			SetString(pName, INI_KEY_TIMESTAMP, pSettings_in->GetTimestampFormat());
			SetString(pName, INI_KEY_PASSWORD, pSettings_in->GetCryptedPassword());
			SetBool(pName, INI_KEY_AUTO_SUBMIT, pSettings_in->IsAutoSubmitted());
			SetString(pName, INI_KEY_TELL_SOUND, pSettings_in->GetTellSound());
			SetString(pName, INI_KEY_PLUGINS, pSettings_in->GetPluginList());
			SetString(pName, INI_KEY_USERNAME, pSettings_in->GetUsername());
			SetLong(pName, INI_KEY_LNG, pSettings_in->GetLanguage());
			SetBool(pName, INI_KEY_VSYNC, pSettings_in->GetVSync());
			SetHex(pName, INI_KEY_HASH, pSettings_in->GetKeyHash());

			for (LabelIter = Labels.cbegin(); LabelIter != LabelEndIt; ++LabelIter)
			{
				pSettings_in->SaveLabel(LabelIter->second, LabelSettings);
				SetString(SectionName.c_str(), LabelIter->first.c_str(), LabelSettings.c_str());
			}

			return true;
		}

		return false;
	}

	bool SettingsManager::CopySettings(const WindowerProfile *pSettings_in)
	{
		if (pSettings_in != nullptr)
		{
			WindowerProfile *pProfile = GetSettings(pSettings_in->GetName());

			if (pProfile != nullptr)
			{
				pProfile->Copy(*pSettings_in);

				return Save();
			}
		}

		return false;
	}

	bool SettingsManager::LoadSettings(WindowerProfile* pSettings_out) const
	{
		if (pSettings_out != nullptr)
		{
			WindowerProfile *pProfile = GetSettings(GetDefaultProfile());

			if (pProfile != nullptr)
			{
				pSettings_out->Copy(*pProfile);

				return true;
			}
		}

		return false;
	}

	ULONG SettingsManager::GetUnsignedLong(const TCHAR *pProfileName_in, eIniKeys Key_in) const
	{
		if (m_pSettingsFile != nullptr)
			return m_pSettingsFile->GetUnsignedLong(pProfileName_in, WindowerProfile::Key(Key_in), WindowerProfile::Default<ULONG>(Key_in));
		
		return WindowerProfile::Default<ULONG>(Key_in);
	}

	const TCHAR* SettingsManager::GetString(const TCHAR *pProfileName_in, eIniKeys Key_in) const
	{
		if (m_pSettingsFile != nullptr)
			return m_pSettingsFile->GetString(pProfileName_in, WindowerProfile::Key(Key_in), WindowerProfile::Default<const TCHAR*>(Key_in));

		return WindowerProfile::Default<const TCHAR*>(Key_in);
	}

	LONG SettingsManager::GetLong(const TCHAR *pProfileName_in, eIniKeys Key_in) const
	{
		if (m_pSettingsFile != nullptr)
			return m_pSettingsFile->GetLong(pProfileName_in, WindowerProfile::Key(Key_in), WindowerProfile::Default<LONG>(Key_in));

		return WindowerProfile::Default<LONG>(Key_in);
	}

	bool SettingsManager::GetBool(const TCHAR *pProfileName_in, eIniKeys Key_in) const
	{
		if (m_pSettingsFile != nullptr)
			return (m_pSettingsFile->GetLong(pProfileName_in, WindowerProfile::Key(Key_in), WindowerProfile::Default<LONG>(Key_in)) == 1L);

		return (WindowerProfile::Default<ULONG>(Key_in) == 1L);
	}

	void SettingsManager::SetString(const TCHAR *pProfileName_in, eIniKeys Key_in, const TCHAR *pValue_in)
	{
		if (m_pSettingsFile != nullptr)
			return m_pSettingsFile->SetString(pProfileName_in, WindowerProfile::Key(Key_in), pValue_in, WindowerProfile::Comment(Key_in));
	}

	void SettingsManager::SetString(const TCHAR *pProfileName_in, const TCHAR *pKey_in, const TCHAR *pValue_in)
	{
		if (m_pSettingsFile != nullptr)
			return m_pSettingsFile->SetString(pProfileName_in, pKey_in, pValue_in);
	}

	void SettingsManager::SetHex(const TCHAR *pProfileName_in, eIniKeys Key_in, ULONG Value_in)
	{
		if (m_pSettingsFile != nullptr)
			return m_pSettingsFile->SetHex(pProfileName_in, WindowerProfile::Key(Key_in), Value_in, WindowerProfile::Comment(Key_in));
	}

	void SettingsManager::SetLong(const TCHAR *pProfileName_in, eIniKeys Key_in, const LONG Value_in)
	{
		if (m_pSettingsFile != nullptr)
			return m_pSettingsFile->SetLong(pProfileName_in, WindowerProfile::Key(Key_in), Value_in, WindowerProfile::Comment(Key_in));
	}

	void SettingsManager::SetBool(const TCHAR *pProfileName_in, eIniKeys Key_in, bool Value_in)
	{
		if (m_pSettingsFile != nullptr)
			return m_pSettingsFile->SetLong(pProfileName_in, WindowerProfile::Key(Key_in), Value_in ? 1L : 0L, WindowerProfile::Comment(Key_in));
	}
}