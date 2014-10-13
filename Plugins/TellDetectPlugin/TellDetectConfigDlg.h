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

//! \brief Main dialog
class TellDetectConfigDlg : public Windower::PluginPropertyPage
{
	enum { IDD = IDD_TELLDETECT_CONFIG };
public:
	explicit TellDetectConfigDlg(Windower::WindowerProfile *pSettings_in,
								 const string_t &PluginName_in);

	bool IsPageValid(string_t *pFeedback_out) const;
	bool InitializePage();
	bool Commit();
	void Revert();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBrowse();
	afx_msg void OnPlayChime();
	afx_msg void OnChimeChange();	

	CString m_SndFile;
};

#endif//__TELL_DETECT_CONFIG_DLG_H__