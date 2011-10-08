// configDlg.h : header file
//

#pragma once

// CconfigDlg dialog
class CconfigDlg : public CDialog
{
// Construction
public:
	CconfigDlg(CWnd* pParent = NULL);	// standard constructor
	~CconfigDlg();

// Dialog Data
	enum { IDD = IDD_CONFIG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	AutoLoginSettings *m_pSettings;
	CryptUtils m_Crypt;
	string_t m_EncryptionKey;
	string_t m_PasswordHash;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnPasswordChange();
};
