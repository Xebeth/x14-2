/**************************************************************************
	created		:	2011-06-18
	filename	: 	SettingsIniFile.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Implementation of the settings interface using an INI file
**************************************************************************/
#ifndef __SETTINGS_INI_FILE_H__
#define __SETTINGS_INI_FILE_H__

#ifndef _SETTINGS_MANAGER_SDK_INCLUDE_
	#error Please include the global header file 'SettingsManager.h'
#endif

namespace Settings
{
	//! \brief Implementation of the settings interface using an INI file
	class SettingsIniFile : public SettingsInterface
	{
	public:
		explicit SettingsIniFile(const string_t &SourceFile_in);
		virtual ~SettingsIniFile();

		// Data source functions
		virtual void SetSource(const string_t &SourceFile_in);
		virtual bool Load();
		virtual bool Save();
		virtual bool Reload();

		// Sections functions
		virtual bool CreateSection(const string_t &SectionName_in);
		virtual bool DeleteSection(const string_t &SectionName_in);
		virtual size_t GetSections(CSimpleIni::TNamesDepend &Sections_out) const;

		// Settings accessors
		LONG GetLong(const string_t &SectionName_in, const string_t &Key_in, LONG DefaultValue = 0L) const;
		ULONG GetUnsignedLong(const string_t &SectionName_in, const string_t &Key_in, ULONG DefaultValue = 0UL) const;
		const TCHAR* GetString(const string_t &SectionName_in, const string_t &Key_in, const TCHAR* pDefaultValue = _T("")) const;

		void SetHex(const string_t &SectionName_in, const string_t &Key_in, LONG NewValue_in, const TCHAR *pComment_in = NULL);
		void SetLong(const string_t &SectionName_in, const string_t &Key_in, LONG NewValue_in, const TCHAR *pComment_in = NULL);		
		void SetString(const string_t &SectionName_in, const string_t &Key_in, const string_t &NewValue_in, const TCHAR *pComment_in = NULL);

		bool DeleteKey(const string_t &SectionName_in, const string_t &KeyName_in) const;
		bool KeyExists(const string_t &SectionName_in, const string_t &KeyName_in) const;
		bool SectionExists(const string_t &SectionName_in) const;

		string_t GetSettingsDrive() const;
		string_t GetSettingsPath() const;

	protected:
		//! the current section
		string_t m_CurrentSection;
		//! the path of the INI file
		string_t m_SourceFile;
		//! the INI object
		CSimpleIni *m_pIni;
	};
}

#endif//__SETTINGS_INI_FILE_H__