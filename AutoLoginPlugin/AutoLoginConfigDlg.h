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
	class AutoLoginSettings;
}

//! \brief Main dialog
class AutoLoginConfigDlg : public CDialog
{
	enum { IDD = IDD_CONFIG_DIALOG };
public:
	AutoLoginConfigDlg(const TCHAR *pProfileName_in, CWnd* pParent = NULL);
	~AutoLoginConfigDlg();

protected:
	HICON m_hIcon;
	Windower::AutoLoginSettings *m_pSettings;
	string_t m_EncryptionKey;
	string_t m_PasswordHash;
	string_t m_Username;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnPasswordChange();
	afx_msg void OnUsernameChange();
};

#endif//__AUTOLOGIN_CONFIG_DLG_H__