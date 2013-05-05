/**************************************************************************
	created		:	2011-10-29
	filename	: 	AutoLoginConfigDlg.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Main dialog
**************************************************************************/
#ifndef __AUTOLOGIN_CONFIG_DLG_H__
#define __AUTOLOGIN_CONFIG_DLG_H__

namespace Windower
{
	class PluginSettings;
}

//! \brief Main dialog
class AutoLoginConfigDlg : public Windower::PluginPropertyPage
{
	enum { IDD = IDD_CONFIG_DIALOG };
public:
	explicit AutoLoginConfigDlg(Windower::PluginSettings *pSettings_in);

	bool Save();

protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPasswordChange();
	afx_msg void OnUsernameChange();
	afx_msg void OnAutoSubmitCheck();

	string_t m_EncryptionKey;
	string_t m_PasswordHash;
	string_t m_Username;
	bool m_AutoSubmit;
};

#endif//__AUTOLOGIN_CONFIG_DLG_H__