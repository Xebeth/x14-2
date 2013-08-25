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
class TimestampConfigDlg : public Windower::PluginPropertyPage
{
	enum { IDD = IDD_TIMESTAMP_CONFIG };
public:
	explicit TimestampConfigDlg(Windower::WindowerProfile *pSettings_in,
								const string_t &PluginName_in);

	bool IsPageValid(string_t *pFeedback_out) const;
	bool InitializePage();
	bool Commit();
	void Revert();

protected:
	afx_msg void OnFormatChange();

	DECLARE_MESSAGE_MAP()

	string_t m_TimestampFormat;
};

#endif//__TIMESTAMP_CONFIG_DLG_H__