/**************************************************************************
	created		:	2011-06-18
	filename	: 	SettingsIniFile.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Implementation of the settings interface using an INI file
**************************************************************************/
#include "stdafx.h"

#include "SettingsInterface.h"
#include "SettingsIniFile.h"

namespace Settings
{
	/*! \brief SettingsIniFile constructor
		\param[in] SourceFile_in : the path of the INI file
	*/
	SettingsIniFile::SettingsIniFile(const string_t &SourceFile_in)
	{
		m_pIni = new CSimpleIni(true, false, false);
		m_pIni->SetSpaces(false);

		m_SourceFile = SourceFile_in;
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
			return m_pIni->LoadFile(m_SourceFile.c_str()) == SI_OK;

		return false;
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
		if (m_pIni != NULL)
		{
			m_pIni->SetValue(SectionName_in.c_str(), NULL, NULL);

			return true;
		}

		return false;
	}

	/*! \brief Deletes a section from the INI file
		\param[in] SectionName_in : the name of the section to delete
		\return true if successful; false otherwise
	*/
	bool SettingsIniFile::DeleteSection(const string_t &SectionName_in)
	{
		if (m_pIni != NULL)
			return m_pIni->Delete(SectionName_in.c_str(), NULL);

		return false;
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
	void SettingsIniFile::SetLong(const string_t &SectionName_in, const string_t &Key_in, LONG NewValue_in)
	{
		if (m_pIni != NULL)
			m_pIni->SetLongValue(SectionName_in.c_str(), Key_in.c_str(), NewValue_in);
	}

	/*! \brief Sets the specified key value as an hexadecimal representation of a long in the specified section
		\param[in] SectionName_in : the section containing the key
		\param[in] Key_in : the name of the key
		\param[in] NewValue_in : the new value for the key
	*/
	void SettingsIniFile::SetHex(const string_t &SectionName_in, const string_t &Key_in, LONG NewValue_in)
	{
		if (m_pIni != NULL)
			m_pIni->SetLongValue(SectionName_in.c_str(), Key_in.c_str(), NewValue_in, NULL, true);
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
	void SettingsIniFile::SetString(const string_t &SectionName_in, const string_t &Key_in, const string_t &NewValue_in) 
	{
		if (m_pIni != NULL)
			m_pIni->SetValue(SectionName_in.c_str(), Key_in.c_str(), NewValue_in.c_str());
	}

	/*! \brief Retrieves a collection of all the sections in the INI file
		\param[out] Sections_out : a collection of all the sections in the INI file
	*/
	size_t SettingsIniFile::GetSections(CSimpleIni::TNamesDepend &Sections_out) const
	{
		m_pIni->GetAllSections(Sections_out);

		return Sections_out.size();
	}
}