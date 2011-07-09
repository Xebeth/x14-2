/**************************************************************************
	created		:	2011-06-18
	filename	: 	SettingsIniFile.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "SettingsInterface.h"
#include "SettingsIniFile.h"

namespace Settings
{
	SettingsIniFile::SettingsIniFile(const string_t &SourceFile_in)
	{
		m_pIni = new CSimpleIni(true, false, false);
		m_pIni->SetSpaces(false);

		m_SourceFile = SourceFile_in;
	}

	SettingsIniFile::~SettingsIniFile()
	{
		if (m_pIni != NULL)
		{
			delete m_pIni;
			m_pIni = NULL;
		}
	}

	void SettingsIniFile::SetSource(const string_t &SourceFile_in)
	{
		m_SourceFile = SourceFile_in;
	}

	bool SettingsIniFile::Load()
	{
		if (m_pIni != NULL)
			return m_pIni->LoadFile(m_SourceFile.c_str()) == SI_OK;

		return false;
	}

	bool SettingsIniFile::Save()
	{
		if (m_pIni != NULL)
			return m_pIni->SaveFile(m_SourceFile.c_str()) == SI_OK;

		return false;
	}

	bool SettingsIniFile::CreateSection(const string_t &SectionName_in)
	{
		if (m_pIni != NULL)
		{
			m_pIni->SetValue(SectionName_in.c_str(), NULL, NULL);

			return true;
		}

		return false;
	}

	bool SettingsIniFile::DeleteSection(const string_t &SectionName_in)
	{
		if (m_pIni != NULL)
		{
			m_pIni->Delete(SectionName_in.c_str(), NULL);

			return true;
		}

		return false;
	}

	LONG SettingsIniFile::GetLong(const string_t &SectionName_in, const string_t &Key_in) const
	{
		if (m_pIni != NULL)
			return m_pIni->GetLongValue(SectionName_in.c_str(), Key_in.c_str());

		return 0L;
	}

	void SettingsIniFile::SetLong(const string_t &SectionName_in, const string_t &Key_in, LONG NewValue_in)
	{
		if (m_pIni != NULL)
			m_pIni->SetLongValue(SectionName_in.c_str(), Key_in.c_str(), NewValue_in);
	}

	const TCHAR* SettingsIniFile::GetString(const string_t &SectionName_in, const string_t &Key_in) const
	{
		if (m_pIni != NULL)
			return m_pIni->GetValue(SectionName_in.c_str(), Key_in.c_str());

		return NULL;
	}

	void SettingsIniFile::SetString(const string_t &SectionName_in, const string_t &Key_in, const string_t &NewValue_in) 
	{
		if (m_pIni != NULL)
			m_pIni->SetValue(SectionName_in.c_str(), Key_in.c_str(), NewValue_in.c_str());
	}

	void SettingsIniFile::getSections(CSimpleIni::TNamesDepend &Sections) const
	{
		m_pIni->GetAllSections(Sections);
	}
}