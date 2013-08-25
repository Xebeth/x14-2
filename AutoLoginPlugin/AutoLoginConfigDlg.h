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
	class WindowerProfile;
}

//! \brief Main dialog
class AutoLoginConfigDlg : public Windower::PluginPropertyPage
{
	enum { IDD = IDD_CONFIG_DIALOG };
public:
	explicit AutoLoginConfigDlg(Windower::WindowerProfile *pSettings_in,
								const string_t &PluginName_in);

	bool IsPageValid(string_t *pFeedback_out) const;
	bool InitializePage();
	bool Commit();
	void Revert();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPasswordChange();
	afx_msg void OnUsernameChange();
	afx_msg void OnAutoSubmitCheck();

	string_t m_EncryptionKey;
	CString m_PasswordHash;
	CString m_Username;
	bool m_AutoSubmit;
};

#endif//__AUTOLOGIN_CONFIG_DLG_H__