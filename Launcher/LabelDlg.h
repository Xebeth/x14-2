/**************************************************************************
created		:	2011-10-29
filename	: 	LabelDlg.h
author		:	Xebeth`
copyright	:	North Edge (2011)
purpose		:	Main dialog
**************************************************************************/
#ifndef __LABEL_DLG_H__
#define __LABEL_DLG_H__

enum eLabelListColumns
{
	LIST_COL_NAME = 0,
	LIST_COL_POSITION,
	LIST_COL_FONT,
	LIST_COL_FONT_COLOR,
	LIST_COL_BOLD,
	LIST_COL_ITALIC,
	LIST_COL_COLLAPSED,
	LIST_COL_COUNT
};

class ColorListCtrl : public CMFCListCtrl
{
protected:
	virtual HFONT OnGetCellFont(int nRow, int nColumn, DWORD dwData = 0);
	virtual COLORREF OnGetCellTextColor(int nRow, int nColumn);
};

//! \brief Main dialog
class LabelDlg : public CDialogEx
{
	enum { IDD = IDD_LABEL_DLG };
public:
	LabelDlg(Windower::SettingsManager &SettingsManager_in,
			 Windower::WindowerProfile &CurrentProfile_in,
			 CWnd* pParent_in = NULL);

	static COLORREF FromARGB(unsigned long ARGB_in);

protected:
	void DoDataExchange(CDataExchange* pDX_in);
	// Generated message map functions
	BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedTextLabels();

	void AddLabel(const string_t &Name_in, const Windower::LabelSettings &Settings_in, bool Selected_in);
	void UpdateLabelList();
	
	Windower::WindowerProfile &m_CurrentSettings;
	Windower::SettingsManager &m_SettingsManager;
	Windower::LabelSettings *m_pCurrentLabel;
	ColorListCtrl m_LabelList;
	int m_CurrentSel;
public:
	afx_msg void OnLvnItemchangedLabelList(NMHDR *pNMHDR, LRESULT *pResult);
	void UpdateUI();
	afx_msg void OnBnClickedUpdateLabel();
	unsigned long ToARGB(COLORREF Color_in);
};

#endif//__LABEL_DLG_H__