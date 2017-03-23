/**************************************************************************
	created		:	2011-10-29
	m_SndFile	: 	AutoBlacklistConfigDlg.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Main dialog
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <afxdlgs.h>

#include <PluginPropertyPage.h>

#include "AutoBlacklistConfigDlg.h"

BEGIN_MESSAGE_MAP(AutoBlacklistConfigDlg, Windower::PluginPropertyPage)
	ON_NOTIFY(NM_DBLCLK, IDC_WORD_LIST, OnClickList)
	ON_EN_KILLFOCUS(IDC_EDIT_CELL, &AutoBlacklistConfigDlg::OnLeaveWordEdit)
	ON_BN_CLICKED(IDC_DEL_SELECTED, &AutoBlacklistConfigDlg::OnDeleteSelected)
	ON_EN_CHANGE(IDC_NEW_WORD, &AutoBlacklistConfigDlg::OnChangeNewWord)
	ON_EN_CHANGE(IDC_NEW_SCORE, &AutoBlacklistConfigDlg::OnChangeNewWord)
	ON_BN_CLICKED(IDC_ADD_WORD, &AutoBlacklistConfigDlg::OnClickAddWord)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_WORD_LIST, &AutoBlacklistConfigDlg::OnLvnItemchangedWordList)
END_MESSAGE_MAP()

/*! \brief AutoBlacklistConfigDlg default constructor
	\param[in] : the parent window of the dialog
 */
AutoBlacklistConfigDlg::AutoBlacklistConfigDlg(Windower::WindowerProfile *pSettings_in, const string_t &PluginName_in)
	: Windower::PluginPropertyPage(pSettings_in, PluginName_in, AutoBlacklistConfigDlg::IDD, IDI_AUTOBLACKLIST),
	  m_nSubItem(-1), m_nItem(-1), m_BlacklistThreshold(15), m_BlacklistCount(3) {}

bool AutoBlacklistConfigDlg::UpdateWordList() const
{
	if (m_pSettings != nullptr)
	{
		const Windower::ScoredWords *pScoredWords = m_pSettings->GetScoredWords();

		if (pScoredWords != nullptr)
		{
			Windower::ScoredWords::const_iterator WordIt, EndIt = pScoredWords->cend();
			CString Word;

			for(WordIt = pScoredWords->cbegin(); WordIt != EndIt; ++WordIt)
			{
				Word.Format(_T("%S"), WordIt->first.c_str());
				InsertWord(Word, WordIt->second);
			}

			return true;
		}
	}

	return false;
}

void AutoBlacklistConfigDlg::OnDeleteSelected()
{
	CListCtrl *pWordList = static_cast<CListCtrl*>(GetDlgItem(IDC_WORD_LIST));

	if (pWordList != nullptr)
	{
		for (int i = 0; i < pWordList->GetItemCount(); ++i)
		{
			if (pWordList->GetItemState(i, LVNI_SELECTED) == LVNI_SELECTED)
			{
				m_DeletedWords.push_back(pWordList->GetItemText(i, LIST_COL_WORD));
				pWordList->DeleteItem(i);
				i = -1;
			}
		}
	}

	GetDlgItem(IDC_NEW_WORD)->SetFocus();
}

void AutoBlacklistConfigDlg::InsertWord(const CString &Word_in, long Score_in) const
{
	CListCtrl *pWordList = static_cast<CListCtrl*>(GetDlgItem(IDC_WORD_LIST));

	if (pWordList != nullptr)
	{
		CString Word = Word_in;
		CString Score;
		LVITEM LvItem;
		
		LvItem.mask = LVIF_TEXT;
		LvItem.iItem = 0;
		// plugin name
		LvItem.iSubItem = LIST_COL_WORD;
		LvItem.pszText = (LPWSTR)Word.GetBuffer();
		LvItem.iItem = pWordList->InsertItem(&LvItem);
		// plugin version
		LvItem.iSubItem = LIST_COL_SCORE;
		Score.Format(_T("%ld"), Score_in);
		LvItem.pszText = (LPWSTR)Score.GetBuffer();
		pWordList->SetItem(&LvItem);
	}
}

void AutoBlacklistConfigDlg::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CListCtrl *pWordList = static_cast<CListCtrl*>(GetDlgItem(IDC_WORD_LIST));
	CEdit *pEdit = static_cast<CEdit*>(GetDlgItem(IDC_EDIT_CELL));
	LPNMLISTVIEW pActiveItem = (LPNMLISTVIEW) pNMHDR;	
	
	if (pActiveItem != nullptr && pWordList != nullptr)
	{
		//get the row number
		m_nItem = pActiveItem->iItem;
		//get the column number
		m_nSubItem = pActiveItem->iSubItem;

		if (m_nSubItem != -1 && m_nItem != -1)
		{
			//Retrieve the text of the selected subItem from the list
			CString ItemText = pWordList->GetItemText(m_nItem, m_nSubItem);
			CRect ItemRect, ListRect, PageRect;
			// this macro is used to retrieve the rectangle of the selected SubItem
			pWordList->GetSubItemRect(m_nItem, m_nSubItem, LVIR_BOUNDS, ItemRect);
			//Get the Rectangle of the listControl
			pWordList->GetWindowRect(ListRect);
			//Get the Rectangle of the Dialog
			GetWindowRect(PageRect);

			int x = ListRect.left - PageRect.left;
			int y = ListRect.top - PageRect.top;

			pEdit->SetWindowText(ItemText);
			pEdit->SetSel(0, -1, TRUE);
			pEdit->SetFocus();
		
			::SetWindowPos(pEdit->GetSafeHwnd(), pWordList->GetSafeHwnd(),
						   ItemRect.left + x + 2, ItemRect.top + y + 2,
						   ItemRect.Width(), ItemRect.Height(), SWP_NOREDRAW);
			pEdit->ShowWindow(SW_SHOW);
		}
		else
		{
			m_nSubItem = m_nItem = -1;
		}
	}
	
	*pResult = 0;
}

/*! \brief Initializes the controls of the page from the settings
	\return true if the initialization succeeded; false otherwise
*/
bool AutoBlacklistConfigDlg::InitializePage()
{
	CListCtrl *pWordList = static_cast<CListCtrl*>(GetDlgItem(IDC_WORD_LIST));

	SetDlgItemText(IDC_HELP_TEXT, _T("When the score of a message exceeds the score threshold, it is blacklisted.\r\n")
								  _T("After the specified number of offenses, the user is automatically added to your blacklist."));

	if (pWordList != nullptr)
	{
		pWordList->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP | LVS_EX_GRIDLINES);

		pWordList->InsertColumn(LIST_COL_WORD,	_T("Word"),	 LVCFMT_LEFT, 361);
		pWordList->InsertColumn(LIST_COL_SCORE,	_T("Score"), LVCFMT_LEFT,  50);
		UpdateWordList();

		SetDlgItemInt(IDC_SCORE_THRESHOLD, m_BlacklistThreshold);
		SetDlgItemInt(IDC_BLOCK_AT, m_BlacklistCount);

		return true;
	}

	return false;
}


void AutoBlacklistConfigDlg::Revert() 
{
	if (m_pSettings != nullptr)
	{
		m_BlacklistThreshold = m_pSettings->GetBlacklistThreshold();
		m_BlacklistCount = m_pSettings->GetBlacklistCount();
	}
}

//! \brief Message handler called when the user presses the OK button
bool AutoBlacklistConfigDlg::Commit()
{
	if (IsPageValid(nullptr) && m_pSettings != nullptr)
	{
		m_pSettings->SetBlacklistThreshold(GetDlgItemInt(IDC_SCORE_THRESHOLD));
		m_pSettings->SetBlacklistCount(GetDlgItemInt(IDC_BLOCK_AT));

		CListCtrl *pWordList = static_cast<CListCtrl*>(GetDlgItem(IDC_WORD_LIST));

		if (pWordList != nullptr)
		{
			int Count = pWordList->GetItemCount();
			CString Text;
			long Score;

			GetDlgItemText(IDC_NEW_WORD, Text);

			for (int i = 0; i < Count; ++i)
			{
				Text = pWordList->GetItemText(i, LIST_COL_WORD);
				Score = _ttol(pWordList->GetItemText(i, LIST_COL_SCORE));

				m_pSettings->AddScoredWord(Text.GetBuffer(), Score);
			}

			DeletedWords::const_iterator WordIt, EndIt = m_DeletedWords.cend();

			for (WordIt = m_DeletedWords.cbegin(); WordIt != EndIt; ++WordIt)
				m_pSettings->AddScoredWord(*WordIt, -1L);
		}

		return true;
	}

	return false;
}

bool AutoBlacklistConfigDlg::IsPageValid(string_t *pFeedback_out) const
{
	int Threshold = GetDlgItemInt(IDC_SCORE_THRESHOLD);
	int Count = GetDlgItemInt(IDC_BLOCK_AT);

	if (pFeedback_out != nullptr)
	{
		if (Threshold < 5)
			*pFeedback_out += _T("\n    - The threshold score must be >= 5");
	}

	return (Threshold >= 5);
}

void AutoBlacklistConfigDlg::OnLeaveWordEdit()
{
	CListCtrl *pWordList = static_cast<CListCtrl*>(GetDlgItem(IDC_WORD_LIST));
	CEdit *pEdit = static_cast<CEdit*>(GetDlgItem(IDC_EDIT_CELL));

	if (pEdit != nullptr && pWordList != nullptr && m_nSubItem != -1 && m_nItem != -1)
	{
		CString Text;

		pEdit->GetWindowText(Text);
		pEdit->ShowWindow(SW_HIDE);

		if(m_nItem != -1)
			pWordList->SetItemText(m_nItem, m_nSubItem, Text);
	}
}

void AutoBlacklistConfigDlg::OnChangeNewWord()
{
	CButton *pAddWord = static_cast<CButton*>(GetDlgItem(IDC_ADD_WORD));

	if (pAddWord != nullptr)
	{
		CString Text;
		int Score = GetDlgItemInt(IDC_NEW_SCORE);
		int Length = GetDlgItemText(IDC_NEW_WORD, Text);

		pAddWord->EnableWindow(Score > 0U && Length > 0);
	}
}

void AutoBlacklistConfigDlg::OnClickAddWord()
{
	int Score = GetDlgItemInt(IDC_NEW_SCORE);
	CString Text;

	GetDlgItemText(IDC_NEW_WORD, Text);

	if (Text.IsEmpty())
	{
		GetDlgItem(IDC_NEW_WORD)->SetFocus();
	}
	else if (Score <= 0)
	{
		GetDlgItem(IDC_NEW_SCORE)->SetFocus();
	}
	else
	{
		CListCtrl *pWordList = static_cast<CListCtrl*>(GetDlgItem(IDC_WORD_LIST));
		CString ItemText;

		if (pWordList != nullptr)
		{
			int Count = pWordList->GetItemCount();

			for (int i = 0; i < Count; ++i)
			{
				if (pWordList->GetItemText(i, LIST_COL_WORD).Compare(Text) == 0)
				{
					Text.Format(_T("%ld"), Score);
					pWordList->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
					pWordList->SetItemText(i, LIST_COL_SCORE, Text);

					return;
				}			
			}
		}
		// new word
		SetDlgItemText(IDC_NEW_SCORE, _T(""));
		SetDlgItemText(IDC_NEW_WORD, _T(""));
		InsertWord(Text, Score);
	}
}

void AutoBlacklistConfigDlg::OnLvnItemchangedWordList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CButton *pDelButton = static_cast<CButton*>(GetDlgItem(IDC_DEL_SELECTED));
	CListCtrl *pWordList = static_cast<CListCtrl*>(GetDlgItem(IDC_WORD_LIST));
	
	if (pWordList != nullptr && pDelButton != nullptr)
	{
		int Count = pWordList->GetItemCount();

		pDelButton->EnableWindow(FALSE);

		for (int i = 0; i < Count; ++i)
		{
			if (pWordList->GetItemState(i, LVNI_SELECTED) == LVNI_SELECTED)
			{
				pDelButton->EnableWindow(TRUE);

				break;
			}
		}
	}

	*pResult = 0;
}


/*! \brief Prevents the dialog from closing when the user pressed ENTER
		   while the focus is in the search edit (start searching instead)
	\param[in] pMsg_in : the message received by the page
	\returns TRUE if the message was handled; FALSE otherwise
*/
BOOL AutoBlacklistConfigDlg::PreTranslateMessage(MSG *pMsg_in)
{
	// we want to intercept the message that says the user pressed the 'Enter' key
	if((pMsg_in->message == WM_KEYDOWN) && (pMsg_in->wParam == VK_RETURN))
	{
		// get the control with the focus
		CWnd *pFocusWnd = GetFocus();

		// get the ID of the control with the focus
		switch(pFocusWnd->GetDlgCtrlID())
		{
			case IDC_NEW_SCORE:
			case IDC_NEW_WORD:
				OnClickAddWord();

				return TRUE; // the message has been "translated", don't dispatch it
			break;
			case IDC_DEL_SELECTED:
				OnDeleteSelected();

				return TRUE; // the message has been "translated", don't dispatch it
			break;
		}
	}

	return CPropertyPage::PreTranslateMessage(pMsg_in);
}