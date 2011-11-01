/**************************************************************************
	created		:	2011-10-29
	filename	: 	WindowerConfigDlg.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Main dialog
**************************************************************************/
#ifndef __WINDOWER_CONFIG_DLG_H__
#define __WINDOWER_CONFIG_DLG_H__

namespace Windower
{
	//! \brief Main dialog
	class WindowerConfigDlg : public CDialog
	{
		enum { IDD = IDD_CONFIG_DIALOG };
	public:
		WindowerConfigDlg(SettingsManager *pSettingsManager, CWnd* pParent = NULL);

		afx_msg void OnProfileNameChange();
		afx_msg void OnResolutionChange();
		afx_msg void OnProfilesChange();
		
		afx_msg void OnDeleteProfile();
		afx_msg void OnVSyncChange();
		afx_msg void OnNewProfile();
		afx_msg void OnSave();

	protected:
		HICON m_hIcon;
		SettingsManager *m_pSettingsManager;
		WindowerProfile *m_pCurrentSettings;
		int m_CurrentSel;

		void GenerateNewName(CString &NewName_in_out);
		void FillSupportedRes();

		// Generated message map functions
		virtual BOOL OnInitDialog();
		afx_msg void OnPaint();
		afx_msg HCURSOR OnQueryDragIcon();
		DECLARE_MESSAGE_MAP()
	};
}

#endif//__WINDOWER_CONFIG_DLG_H__