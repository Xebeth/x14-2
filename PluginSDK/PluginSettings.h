/**************************************************************************
	created		:	2013-05-04
	filename	: 	PluginSettings.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef _WINDOWER_PLUGIN_SDK_INCLUDE_
	#error Please include the global header file 'PluginSDK.h'
#endif

#ifndef __PLUGIN_SETTINGS_H__
#define __PLUGIN_SETTINGS_H__

namespace Windower
{
	class PluginSettings : public Settings::SettingsIniFile
	{
	public:
		PluginSettings(const string_t &SourceFile_in, const TCHAR *pProfileName_in);

		void SetCurrentSection(const string_t &CurrentSection_in);
		const string_t& GetCurrentSection() const;

		bool CreateSection(const string_t &SectionName_in, bool SetCurrent_in);

		// Settings accessors
		LONG GetLong(const string_t &Key_in, LONG DefaultValue = 0L) const;
		ULONG GetUnsignedLong(const string_t &Key_in, ULONG DefaultValue = 0UL) const;
		const TCHAR* GetString(const string_t &Key_in, const TCHAR* pDefaultValue = _T("")) const;

		void SetHex(const string_t &Key_in, LONG NewValue_in, const TCHAR *pComment_in = NULL);
		void SetLong(const string_t &Key_in, LONG NewValue_in, const TCHAR *pComment_in = NULL);		
		void SetString(const string_t &Key_in, const string_t &NewValue_in, const TCHAR *pComment_in = NULL);

		bool DeleteKey(const string_t &KeyName_in) const;
		bool KeyExists(const string_t &KeyName_in) const;

	protected:
		//! the current section
		string_t m_CurrentSection;
	};
}

#endif//__PLUGIN_SETTINGS_H__
