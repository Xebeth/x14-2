/**************************************************************************
	created		:	2011-07-10
	filename	: 	TimestampSettings.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Timestamp plugin settings
**************************************************************************/
#ifndef __TIMESTAMP_SETTINGS_H__
#define __TIMESTAMP_SETTINGS_H__

namespace Windower
{
	//! \brief Timestamp plugin settings
	class TimestampSettings : public Settings::SettingsIniFile
	{
	public:
		TimestampSettings(const TCHAR *pSettingsFile_in, const TCHAR *pProfileName_in);

		bool Load();
		bool Save();

		/*! \brief Sets the format of the timestamp
			\param[in] Format_in : the format of the timestamp
		*/
		void SetFormat(const string_t& Format_in) { m_Format = Format_in; }
		/*! \brief Retrieves the username
			\return the username
		*/
		const string_t& GetFormat() const { return m_Format; }

	protected:
		//! the format of the timestamp
		string_t		m_Format;
		//! the section in which the parameters are written or read
		string_t		m_SectionName;
	};
}

#endif//__TIMESTAMP_SETTINGS_H__