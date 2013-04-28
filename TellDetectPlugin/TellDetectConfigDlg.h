/**************************************************************************
	created		:	2011-10-29
	filename	: 	TellDetectConfigDlg.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Main dialog
**************************************************************************/
#ifndef __TELL_DETECT_CONFIG_DLG_H__
#define __TELL_DETECT_CONFIG_DLG_H__

#define OPEN_DIALOG_FLAGS (OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_ENABLESIZING)

namespace Windower
{
	class TellDetectSettings;
}

//! \brief Main dialog
class TellDetectConfigDlg : public CDialog
{
	enum { IDD = IDD_TELLDETECT_CONFIG };
public:
	TellDetectConfigDlg(const TCHAR *pConfigFile_in, const TCHAR *pProfileName_in, CWnd* pParent = NULL);
	~TellDetectConfigDlg();

protected:
	HICON m_hIcon;
	CString m_Filename;
	HMODULE m_hModule;

	Windower::TellDetectSettings *m_pSettings;

	// Generated message map functions
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();
	afx_msg void OnBrowse();
	afx_msg void OnPlayChime();
	afx_msg void OnBnClickedOk();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()	
};

#endif//__TELL_DETECT_CONFIG_DLG_H__