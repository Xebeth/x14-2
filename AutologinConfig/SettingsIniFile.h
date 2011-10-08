/**************************************************************************
	created		:	2011-06-18
	filename	: 	SettingsIniFile.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#ifndef __SETTINGS_INI_FILE_H__
#define __SETTINGS_INI_FILE_H__

#include "SimpleIni.h"

namespace Settings
{
	class SettingsIniFile : public SettingsInterface
	{
	public:
		SettingsIniFile(const string_t &SourceFile_in);
		virtual ~SettingsIniFile();

		// Data source functions
		virtual void SetSource(const string_t &SourceFile_in);
		virtual bool Load();
		virtual bool Save();

		// Sections functions
		virtual bool CreateSection(const string_t &SectionName_in);
		virtual bool DeleteSection(const string_t &SectionName_in);
		virtual void getSections(CSimpleIni::TNamesDepend &Sections) const;

		// Settings accessors
		LONG GetLong(const string_t &SectionName_in, const string_t &Key_in) const;
		ULONG GetUnsignedLong(const string_t &SectionName_in, const string_t &Key_in) const;
		void SetHex(const string_t &SectionName_in, const string_t &Key_in, LONG NewValue_in);
		void SetLong(const string_t &SectionName_in, const string_t &Key_in, LONG NewValue_in);
		const TCHAR* GetString(const string_t &SectionName_in, const string_t &Key_in) const;
		void SetString(const string_t &SectionName_in, const string_t &Key_in, const string_t &NewValue_in);
	protected:
		string_t m_SourceFile;
		CSimpleIni *m_pIni;
	};
}

#endif//__SETTINGS_INI_FILE_H__