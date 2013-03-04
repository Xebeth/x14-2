/**************************************************************************
	created		:	2011-07-10
	filename	: 	AutoLoginSettings.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	AutoLogin plugin settings
**************************************************************************/
#ifndef __AUTO_LOGIN_SETTINGS_H__
#define __AUTO_LOGIN_SETTINGS_H__

namespace Windower
{
	typedef Settings::SettingsIniFile SettingsIniFile;
	
	//! \brief AutoLogin plugin settings
	class AutoLoginSettings : public SettingsIniFile
	{
	public:
		AutoLoginSettings(const TCHAR *pSettingsFile_in, const TCHAR *pProfileName_in);

		bool Load();
		bool Save();

		/*! \brief Sets the username
			\param[in] Username_in : the username
		*/
		void SetUsername(const string_t& Username_in) { m_Username = Username_in; }
		/*! \brief Retrieves the username
			\return the username
		*/
		const string_t& GetUsername() const { return m_Username; }

		/*! \brief Sets the password hash
			\param[in] Password_in : the password hash
		*/
		void SetPassword(const string_t& Password_in) { m_Password = Password_in; }
		/*! \brief Retrieves the password hash
			\return the password hash
		*/
		const string_t& GetPassword() const { return m_Password; }

		/*! \brief Sets the encryption key hash
			\param[in] KeyHash_in : the encryption key hash
		*/
		void SetKeyHash(long KeyHash_in) { m_KeyHash = KeyHash_in; }
		/*! \brief Retrieves the encryption key hash
			\return the encryption key hash
		*/
		long GetKeyHash() const { return m_KeyHash; }
		/*! \brief Sets the handle to the IE server window
			\param[in] hParentWnd_in : the handle to the IE server window
		*/
		void SetParentWnd(HWND hParentWnd_in) { m_hParentWnd = hParentWnd_in; }
		/*! \brief Retrieves the handle to the IE server window
			\return the handle to the IE server window
		*/
		HWND GetParentWnd() const { return m_hParentWnd; }

		const TCHAR* GetConfigFile() const { return m_ConfigFile.c_str(); }

	protected:
		//! the handle to the IE server window
		HWND			m_hParentWnd;
		//! the username
		string_t		m_Username;
		//! the password hash
		string_t		m_Password;
		//! the key hash
		long			m_KeyHash;
		//! the section in which the parameters are written or read
		string_t		m_SectionName;
		//! config file path
		string_t		m_ConfigFile;
	};
}

#endif//__AUTO_LOGIN_SETTINGS_H__