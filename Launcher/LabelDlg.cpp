/**************************************************************************
created		:	2011-10-29
filename	: 	LabelDlg.cpp
author		:	Xebeth`
copyright	:	North Edge (2011)
purpose		:	Main dialog
**************************************************************************/
#include "stdafx.h"
#include "resource.h"

#include "LauncherApp.h"
#include "LabelDlg.h"

BEGIN_MESSAGE_MAP(LabelDlg, CDialogEx)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LABEL_LIST, &LabelDlg::OnLvnItemchangedLabelList)
	ON_BN_CLICKED(IDC_UPDATE_LABEL, &LabelDlg::OnBnClickedUpdateLabel)
END_MESSAGE_MAP()

LabelDlg::LabelDlg(Windower::SettingsManager &SettingsManager_in,
				   Windower::WindowerProfile &CurrentProfile_in,
				   CWnd* pParent_in)
	: CDialogEx(LabelDlg::IDD, pParent_in), m_CurrentSel(-1),
	  m_SettingsManager(SettingsManager_in),
	  m_CurrentSettings(CurrentProfile_in) {}

void LabelDlg::DoDataExchange(CDataExchange *pDX_in)
{
	CDialogEx::DoDataExchange(pDX_in);

	DDX_Control(pDX_in, IDC_LABEL_LIST, m_LabelList);
}

BOOL LabelDlg::OnInitDialog()
{
	CMFCFontComboBox *pFontName = static_cast<CMFCFontComboBox*>(GetDlgItem(IDC_FONT_NAME));

	CDialogEx::OnInitDialog();

	m_LabelList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP | LVS_EX_GRIDLINES);
	
	m_LabelList.InsertColumn(LIST_COL_NAME,			_T("Label name"),	 LVCFMT_LEFT, 100);
	m_LabelList.InsertColumn(LIST_COL_POSITION,		_T("Position"),		 LVCFMT_LEFT,  60);
	m_LabelList.InsertColumn(LIST_COL_FONT,			_T("Font"),			 LVCFMT_LEFT, 140);
	m_LabelList.InsertColumn(LIST_COL_FONT_COLOR,	_T("Text color"),	 LVCFMT_LEFT,  70);
	m_LabelList.InsertColumn(LIST_COL_BOLD,			_T("Bold"),			 LVCFMT_LEFT,  35);
	m_LabelList.InsertColumn(LIST_COL_ITALIC,		_T("Italic"),		 LVCFMT_LEFT,  35);

	pFontName->m_bDrawUsingFont = TRUE;

	UpdateLabelList();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void LabelDlg::UpdateLabelList()
{
	Windower::TextLabels &Labels = m_CurrentSettings.GetTextLabels();
	Windower::TextLabels::const_iterator LabelIt, EndIt = Labels.cend();
	int Count = 0;

	m_LabelList.DeleteAllItems();
	m_LabelList.SetFocus();
	m_pCurrentLabel = NULL;

	for (LabelIt = Labels.cbegin(); LabelIt != EndIt; ++LabelIt)
		AddLabel(LabelIt->first, LabelIt->second, (Count++ == m_CurrentSel));

	UpdateUI();
}

void LabelDlg::AddLabel(const string_t &Name_in, const Windower::LabelSettings &Settings_in, bool Selected_in)
{
	LVITEM LvItem;
	string_t Text;

	LvItem.state = Selected_in ? LVIS_SELECTED | LVIS_FOCUSED : 0;
	LvItem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
	LvItem.stateMask = LVIS_SELECTED | LVIS_FOCUSED;	
	LvItem.lParam = (LPARAM)&Settings_in;	
	LvItem.iItem = 0;

	if (Selected_in)
		m_pCurrentLabel = (Windower::LabelSettings*)&Settings_in;

	// label name
	LvItem.iSubItem = LIST_COL_NAME;
	LvItem.pszText = (LPTSTR)Name_in.c_str();

	int Index = m_LabelList.InsertItem(&LvItem);

	LvItem.iItem = Index;
	LvItem.mask = LVIF_TEXT;

	// label position
	LvItem.iSubItem = LIST_COL_POSITION;
	format(Text, _T("%ld,%ld"), Settings_in.X, Settings_in.Y);
	LvItem.pszText = (LPTSTR)Text.c_str();
	m_LabelList.SetItem(&LvItem);
	// label font
	LvItem.iSubItem = LIST_COL_FONT;
	format(Text, _T("%s %hu"), Settings_in.FontName.c_str(), Settings_in.FontSize);
	LvItem.pszText = (LPTSTR)Text.c_str();
	m_LabelList.SetItem(&LvItem);
	// label text color
	LvItem.iSubItem = LIST_COL_FONT_COLOR;
	format(Text, _T("#%6X"), Settings_in.TextColor & 0x00FFFFFF);
	LvItem.pszText = (LPTSTR)Text.c_str();
	m_LabelList.SetItem(&LvItem);
	// label bold
	LvItem.iSubItem = LIST_COL_BOLD;
	LvItem.pszText = Settings_in.bBold ? _T("yes") : _T("no");
	m_LabelList.SetItem(&LvItem);
	// label italic
	LvItem.iSubItem = LIST_COL_ITALIC;
	LvItem.pszText = Settings_in.bItalic ? _T("yes") : _T("no");
	m_LabelList.SetItem(&LvItem);
}

COLORREF ColorListCtrl::OnGetCellTextColor(int nRow, int nColumn)
{
	if (nColumn == LIST_COL_FONT)
	{
		const Windower::LabelSettings *pSettings = (const Windower::LabelSettings*)GetItemData(nRow);

		if (pSettings != NULL)
			return LabelDlg::FromARGB(pSettings->TextColor);
	}

	// return COLORREF or call default
	return CMFCListCtrl::OnGetCellTextColor(nRow, nColumn);
}

HFONT ColorListCtrl::OnGetCellFont(int nRow, int nColumn, DWORD dwData)
{
	if (nColumn == LIST_COL_FONT)
	{
		const Windower::LabelSettings *pSettings = (const Windower::LabelSettings*)GetItemData(nRow);

		if (pSettings != NULL)
		{
			CFont Result;

			Result.CreateFontW(16, 0, DMORIENT_PORTRAIT, DMORIENT_PORTRAIT,
							   pSettings->bBold ? FW_BOLD : FW_NORMAL, pSettings->bItalic,
							   FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							   ANTIALIASED_QUALITY, FF_DONTCARE, pSettings->FontName.c_str());

			return (HFONT)Result.Detach();
		}
	}

	// return COLORREF or call default
	return CMFCListCtrl::OnGetCellFont(nRow, nColumn, dwData);
}
void LabelDlg::OnLvnItemchangedLabelList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if ((pNMLV->uChanged & LVIF_STATE))
	{
		if ((pNMLV->uNewState & LVIS_SELECTED) == LVIS_SELECTED)
		{
			m_pCurrentLabel = (Windower::LabelSettings*)m_LabelList.GetItemData(pNMLV->iItem);
			m_CurrentSel = pNMLV->iItem;
		}
		else
		{
			m_pCurrentLabel = NULL;
			m_CurrentSel = -1;
		}

		UpdateUI();
	}

	*pResult = 0;
}

void LabelDlg::UpdateUI()
{
	bool Enable = (m_pCurrentLabel != NULL);

	CMFCColorButton *pTextColor = static_cast<CMFCColorButton*>(GetDlgItem(IDC_TEXT_COLOR));
	CMFCFontComboBox *pFontName = static_cast<CMFCFontComboBox*>(GetDlgItem(IDC_FONT_NAME));
	CMFCButton *pItalic = static_cast<CMFCButton*>(GetDlgItem(IDC_ITALIC_CHK));
	CMFCButton *pBold = static_cast<CMFCButton*>(GetDlgItem(IDC_BOLD_CHK));

	GetDlgItem(IDC_FONT_SIZE)->EnableWindow(Enable);
	GetDlgItem(IDC_UPDATE_LABEL)->EnableWindow();
	GetDlgItem(IDC_POS_X)->EnableWindow(Enable);
	GetDlgItem(IDC_POS_Y)->EnableWindow(Enable);	
	pTextColor->EnableWindow(Enable);
	pFontName->EnableWindow(Enable);
	pItalic->EnableWindow(Enable);
	pBold->EnableWindow(Enable);

	if (Enable)
	{
		pItalic->SetCheck(m_pCurrentLabel->bItalic ? BST_CHECKED : BST_UNCHECKED);
		pBold->SetCheck(m_pCurrentLabel->bBold ? BST_CHECKED : BST_UNCHECKED);
		SetDlgItemInt(IDC_FONT_SIZE, m_pCurrentLabel->FontSize, false);
		pTextColor->SetColor(FromARGB(m_pCurrentLabel->TextColor));
		pFontName->SelectFont(m_pCurrentLabel->FontName.c_str());
		SetDlgItemInt(IDC_POS_X, m_pCurrentLabel->X);
		SetDlgItemInt(IDC_POS_Y, m_pCurrentLabel->Y);
	}
}

COLORREF LabelDlg::FromARGB(unsigned long ARGB_in)
{
	unsigned short R = (unsigned short)((ARGB_in & 0x00FF0000) >> 16);
	unsigned short G = (unsigned short)((ARGB_in & 0x0000FF00) >> 8);
	unsigned short B = (unsigned short)(ARGB_in & 0x000000FF);

	return RGB(R, G, B);
}

unsigned long LabelDlg::ToARGB(COLORREF Color_in)
{
	unsigned long R = ((Color_in & 0x000000FF) << 16);
	unsigned long G = (Color_in & 0x0000FF00);
	unsigned long B = ((Color_in & 0x00FF0000) >> 16);

	return (0xFF000000 | R | G | B);
}

void LabelDlg::OnBnClickedUpdateLabel()
{
	if (m_pCurrentLabel != NULL)
	{
		CMFCColorButton *pTextColor = static_cast<CMFCColorButton*>(GetDlgItem(IDC_TEXT_COLOR));
		CMFCFontComboBox *pFontName = static_cast<CMFCFontComboBox*>(GetDlgItem(IDC_FONT_NAME));
		CMFCButton *pItalic = static_cast<CMFCButton*>(GetDlgItem(IDC_ITALIC_CHK));
		CMFCButton *pBold = static_cast<CMFCButton*>(GetDlgItem(IDC_BOLD_CHK));

		m_pCurrentLabel->FontSize = (unsigned short)GetDlgItemInt(IDC_FONT_SIZE, NULL, FALSE);
		m_pCurrentLabel->bItalic = (pItalic->GetCheck() == BST_CHECKED);
		m_pCurrentLabel->FontName = pFontName->GetSelFont()->m_strName;
		m_pCurrentLabel->TextColor = ToARGB(pTextColor->GetColor());
		m_pCurrentLabel->bBold = (pBold->GetCheck() == BST_CHECKED);
		m_pCurrentLabel->X = (long)GetDlgItemInt(IDC_POS_X);
		m_pCurrentLabel->Y = (long)GetDlgItemInt(IDC_POS_Y);

		UpdateLabelList();
	}
}