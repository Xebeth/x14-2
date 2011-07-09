/**************************************************************************
	created		:	2011-06-03
	filename	: 	SettingsInterface.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Settings interface
**************************************************************************/
#ifndef __SETTINGS_INTERFACE_H__
#define __SETTINGS_INTERFACE_H__

namespace Settings
{
	class SettingsInterface
	{
	public:
		// Data source functions
		virtual bool Load() =0;
		virtual bool Save() =0;

		// Sections functions
		virtual bool CreateSection(const string_t &SectionName_in) =0;
		virtual bool DeleteSection(const string_t &SectionName_in) =0;

		// Settings accessors
		virtual LONG GetLong(const string_t &SectionName_in, const string_t &Key_in) const =0;
		virtual void SetLong(const string_t &SectionName_in, const string_t &Key_in, LONG NewValue_in) =0;
		virtual const TCHAR* GetString(const string_t &SectionName_in, const string_t &Key_in) const =0;
		virtual void SetString(const string_t &SectionName_in, const string_t &Key_in, const string_t &NewValue_in) =0;
	};
}

#endif//__SETTINGS_INTERFACE_H__