/**************************************************************************
	created		:	2011-07-10
	filename	: 	TellDetectSettings.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	TellDetect plugin settings
**************************************************************************/
#ifndef __TELL_DETECT_SETTINGS_H__
#define __TELL_DETECT_SETTINGS_H__

namespace Windower
{
	//! \brief TellDetect plugin settings
	class TellDetectSettings : public Settings::SettingsIniFile
	{
	public:
		TellDetectSettings(const TCHAR *pSettingsFile_in, const TCHAR *pProfileName_in);

		bool Load();
		bool Save();

		/*! \brief Sets the path to the tell sound
			\param[in] Filename_in : the path to the tell sound
		*/
		void SetFilename(const string_t& Filename_in) { m_SoundFilename = Filename_in; }
		/*! \brief Retrieves the path to the tell sound
			\return the path to the tell sound
		*/
		const string_t& GetFilename() const { return m_SoundFilename; }

	protected:
		//! the path to the tell sound
		string_t		m_SoundFilename;
		//! the section in which the parameters are written or read
		string_t		m_SectionName;
	};
}

#endif//__TELL_DETECT_SETTINGS_H__