/**************************************************************************
	created		:	2011-07-10
	filename	: 	AutoLoginSettings.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#ifndef __AUTO_LOGIN_SETTINGS_H__
#define __AUTO_LOGIN_SETTINGS_H__

namespace Windower
{
	typedef Settings::SettingsIniFile SettingsIniFile;
	
	class AutoLoginSettings : public SettingsIniFile
	{
	public:
		explicit AutoLoginSettings(const TCHAR *pSettingsFile_in);

		bool Load();
		bool Save();

		void SetAutoValidate(bool AutoValidate_in) { m_AutoValidate = AutoValidate_in; }
		bool GetAutoValidate() const { return m_AutoValidate; }
		void SetPassword(const string_t& Password_in) { m_Password = Password_in; }
		const string_t& GetPassword() const { return m_Password; }
		void SetKeyHash(long KeyHash_in) { m_KeyHash = KeyHash_in; }
		long GetKeyHash() const { return m_KeyHash; }
		void SetParentWnd(HWND hParentWnd_in) { m_hParentWnd = hParentWnd_in; }
		HWND GetParentWnd() const { return m_hParentWnd; }

	protected:
		bool			 m_AutoValidate;
		HWND			 m_hParentWnd;
		string_t		 m_Password;
		long			 m_KeyHash;
	};
}

#endif//__AUTO_LOGIN_SETTINGS_H__