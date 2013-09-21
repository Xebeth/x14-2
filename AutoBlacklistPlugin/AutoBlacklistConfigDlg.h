/**************************************************************************
	created		:	2011-10-29
	filename	: 	AutoBlacklistConfigDlg.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Main dialog
**************************************************************************/
#ifndef __AUTO_BLACKLIST_CONFIG_DLG_H__
#define __AUTO_BLACKLIST_CONFIG_DLG_H__

#define OPEN_DIALOG_FLAGS (OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_ENABLESIZING)

//! \brief Main dialog
class AutoBlacklistConfigDlg : public Windower::PluginPropertyPage
{
	typedef std::list<CString> DeletedWords;
	enum { IDD = IDD_AUTOBLACKLIST_CONFIG };
	
	enum eWordListColumns
	{
		LIST_COL_WORD = 0,
		LIST_COL_SCORE,
	};
public:
	explicit AutoBlacklistConfigDlg(Windower::WindowerProfile *pSettings_in,
									const string_t &PluginName_in);

	bool IsPageValid(string_t *pFeedback_out) const;
	bool InitializePage();
	bool Commit();
	void Revert();

protected:
	void UpdateDeleteButton(int iItem_in, int iSubItem_in, bool bShow_in);
	void InsertWord(const CString &Word_in, long Score_in) const;
	bool UpdateWordList() const;

	DECLARE_MESSAGE_MAP()

	BOOL PreTranslateMessage(MSG *pMsg_in);

	afx_msg void OnClickAddWord();
	afx_msg void OnLeaveWordEdit();
	afx_msg void OnDeleteSelected();
	afx_msg void OnChangeNewWord();
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedWordList(NMHDR *pNMHDR, LRESULT *pResult);
	
	int m_BlacklistCount, m_BlacklistThreshold;
	DeletedWords m_DeletedWords;
	int m_nItem, m_nSubItem;	
};

#endif//__AUTO_BLACKLIST_CONFIG_DLG_H__