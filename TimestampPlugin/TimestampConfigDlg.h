/**************************************************************************
	created		:	2011-10-29
	filename	: 	TimestampConfigDlg.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Main dialog
**************************************************************************/
#ifndef __TIMESTAMP_CONFIG_DLG_H__
#define __TIMESTAMP_CONFIG_DLG_H__


//! \brief Main dialog
class TimestampConfigDlg : public CDialog
{
	enum { IDD = IDD_TIMESTAMP_CONFIG };
public:
	TimestampConfigDlg(const TCHAR *pConfigFile_in, const TCHAR *pProfileName_in, CWnd* pParent = NULL);
	~TimestampConfigDlg();

protected:
	HICON m_hIcon;
	Windower::TimestampSettings *m_pSettings;

	// Generated message map functions
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();
	afx_msg void OnBnClickedOk();
	afx_msg void OnFormatChange();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()	
};

#endif//__TIMESTAMP_CONFIG_DLG_H__