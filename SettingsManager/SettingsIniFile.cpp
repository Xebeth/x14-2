/**************************************************************************
	created		:	2011-06-18
	filename	: 	SettingsIniFile.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Implementation of the settings interface using an INI file
**************************************************************************/
#include "stdafx.h"

#include "SimpleIni.h"
#include "SettingsInterface.h"
#include "SettingsIniFile.h"

namespace Settings
{
	/*! \brief SettingsIniFile constructor
		\param[in] SourceFile_in : the path of the INI file
	*/
	SettingsIniFile::SettingsIniFile(const string_t &SourceFile_in)
		: m_SourceFile(SourceFile_in), m_bIsLoaded(false)
	{
		m_pIni = new CSimpleIni(true, false, false);
		m_pIni->SetSpaces(false);		
	}

	//! \brief SettingsIniFile destructor
	SettingsIniFile::~SettingsIniFile()
	{
		if (m_pIni != NULL)
		{
			delete m_pIni;
			m_pIni = NULL;
		}
	}

	/*! \brief Sets the path of the INI file
		\param[in] SourceFile_in : the path of the INI file
	*/
	void SettingsIniFile::SetSource(const string_t &SourceFile_in)
	{
		m_SourceFile = SourceFile_in;
	}

	/*! \brief Loads the settings from the INI file
		\return true if successful; false otherwise
	*/
	bool SettingsIniFile::Load()
	{
		if (m_pIni != NULL)
			m_bIsLoaded = m_pIni->LoadFile(m_SourceFile.c_str()) == SI_OK;

		return m_bIsLoaded;
	}

	/*! \brief Forces the file to be reloaded
		\return true if successful; false otherwise
	*/
	bool SettingsIniFile::Reload()
	{
		if (m_pIni != NULL)
		{
			m_pIni->Reset();
			
			return Load();
		}

		return false;
	}

	/*! \brief Saves the settings to the INI file
		\return true if successful; false otherwise
	*/
	bool SettingsIniFile::Save()
	{
		if (m_pIni != NULL)
			return m_pIni->SaveFile(m_SourceFile.c_str()) == SI_OK;

		return false;
	}

	/*! \brief Creates a new section in the INI file
		\param[in] SectionName_in : the name of the new section
		\return true if successful; false otherwise
	*/
	bool SettingsIniFile::CreateSection(const string_t &SectionName_in)
	{
		return (m_pIni != NULL && m_pIni->SetValue(SectionName_in.c_str(), NULL, NULL));
	}

	/*! \brief Deletes a section from the INI file
		\param[in] SectionName_in : the name of the section to delete
		\return true if successful; false otherwise
	*/
	bool SettingsIniFile::DeleteSection(const string_t &SectionName_in)
	{
		return (m_pIni != NULL && m_pIni->Delete(SectionName_in.c_str(), NULL));
	}

	/*! \brief Retrieves the long value corresponding to the specified key and section
		\param[in] SectionName_in : the section containing the key
		\param[in] Key_in : the name of the key
		\param[in] DefaultValue : the default value if the key wasn't found
		\return the long value of the key if valid; DefaultValue otherwise
	*/
	LONG SettingsIniFile::GetLong(const string_t &SectionName_in, const string_t &Key_in, LONG DefaultValue) const
	{
		if (m_pIni != NULL)
			return m_pIni->GetLongValue(SectionName_in.c_str(), Key_in.c_str(), DefaultValue);

		return DefaultValue;
	}

	/*! \brief Retrieves the DWORD value corresponding to the specified key and section
		\param[in] SectionName_in : the section containing the key
		\param[in] Key_in : the name of the key
		\param[in] DefaultValue : the default value if the key wasn't found
		\return the DWORD value of the key if valid; DefaultValue otherwise
	*/
	ULONG SettingsIniFile::GetUnsignedLong(const string_t &SectionName_in, const string_t &Key_in, ULONG DefaultValue) const
	{
		if (m_pIni != NULL)
			return m_pIni->GetUnsignedLongValue(SectionName_in.c_str(), Key_in.c_str(), DefaultValue);

		return DefaultValue;
	}

	/*! \brief Sets the specified key value as a long in the specified section
		\param[in] SectionName_in : the section containing the key
		\param[in] Key_in : the name of the key
		\param[in] NewValue_in : the new value for the key
	*/
	void SettingsIniFile::SetLong(const string_t &SectionName_in, const string_t &Key_in, LONG NewValue_in, const TCHAR *pComment_in)
	{
		if (m_pIni != NULL)
			m_pIni->SetLongValue(SectionName_in.c_str(), Key_in.c_str(), NewValue_in, pComment_in);
	}

	/*! \brief Sets the specified key value as an hexadecimal representation of a long in the specified section
		\param[in] SectionName_in : the section containing the key
		\param[in] Key_in : the name of the key
		\param[in] NewValue_in : the new value for the key
	*/
	void SettingsIniFile::SetHex(const string_t &SectionName_in, const string_t &Key_in, LONG NewValue_in, const TCHAR *pComment_in)
	{
		if (m_pIni != NULL)
			m_pIni->SetLongValue(SectionName_in.c_str(), Key_in.c_str(), NewValue_in, pComment_in, true);
	}

	/*! \brief Retrieves the string value corresponding to the specified key and section
		\param[in] SectionName_in : the section containing the key
		\param[in] Key_in : the name of the key
		\param[in] pDefaultValue : the default value if the key wasn't found
		\return the string value of the key if valid; pDefaultValue otherwise
	*/
	const TCHAR* SettingsIniFile::GetString(const string_t &SectionName_in, const string_t &Key_in, const TCHAR* pDefaultValue) const
	{
		if (m_pIni != NULL)
			return m_pIni->GetValue(SectionName_in.c_str(), Key_in.c_str(), pDefaultValue);

		return pDefaultValue;
	}

	/*! \brief Sets the specified key value as string in the specified section
		\param[in] SectionName_in : the section containing the key
		\param[in] Key_in : the name of the key
		\param[in] NewValue_in : the new value for the key
	*/
	void SettingsIniFile::SetString(const string_t &SectionName_in, const string_t &Key_in, const string_t &NewValue_in, const TCHAR *pComment_in) 
	{
		if (m_pIni != NULL)
			m_pIni->SetValue(SectionName_in.c_str(), Key_in.c_str(), NewValue_in.c_str(), pComment_in);
	}

	/*! \brief Retrieves a collection of all the sections in the INI file
		\param[out] Sections_out : a collection of all the sections in the INI file
	*/
	size_t SettingsIniFile::GetSections(CSimpleIni::TNamesDepend &Sections_out) const
	{
		m_pIni->GetAllSections(Sections_out);

		return Sections_out.size();
	}

	/*! \brief Checks if the specified section exists
		\param[in] SectionName_in : section to check
		\return true if the section exists; false otherwise
	*/
	bool SettingsIniFile::SectionExists(const string_t &SectionName_in) const
	{
		return (m_pIni != NULL) ? m_pIni->SectionExists(SectionName_in.c_str()) : false;
	}

	/*! \brief Checks if the specified key exists within the specified section
		\param[in] SectionName_in : section containing the key
		\param[in] KeyName_in : key to check
		\return true if the key exists in the specified section; false otherwise
	*/
	bool SettingsIniFile::KeyExists(const string_t &SectionName_in, const string_t &KeyName_in) const
	{
		return (m_pIni != NULL) ? m_pIni->KeyExists(SectionName_in.c_str(), KeyName_in.c_str()) : false;
	}

	/*! \brief Deletes the specified key exists within the specified section
		\param[in] SectionName_in : section containing the key
		\param[in] KeyName_in : key to delete
		\return true if the key was deleted in the specified section; false otherwise
	*/
	bool SettingsIniFile::DeleteKey(const string_t &SectionName_in, const string_t &KeyName_in) const
	{
		return (m_pIni != NULL) ? m_pIni->DeleteKey(SectionName_in.c_str(), KeyName_in.c_str()) : false;
	}

	/*! \brief Retrieves the path of the settings file
		\return the path of the settings file
	*/
	string_t SettingsIniFile::GetSettingsPath() const
	{
		string_t::size_type Pos = m_SourceFile.find_last_of('\\');
		string_t Path;

		if (Pos != STRING_T_NPOS)
			Path = m_SourceFile.substr(0, Pos + 1);

		return Path;
	}

	/*! \brief Retrieves the drive of the settings file
		\return the drive of the settings file
	*/
	string_t SettingsIniFile::GetSettingsDrive() const
	{
		string_t::size_type Pos = m_SourceFile.find_first_of('\\');
		string_t Path;

		if (Pos != STRING_T_NPOS)
			Path = m_SourceFile.substr(0, Pos + 1);

		return Path;
	}

	/*! \brief Checks if the configuration file is loaded
		\return true if the configuration file is loaded; false otherwise
	*/
	bool SettingsIniFile::IsConfigLoaded() const
	{ return m_bIsLoaded; }
}