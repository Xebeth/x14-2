/**************************************************************************
	created		:	2013-05-05
	filename	: 	WizardStartPage.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __WIZARD_START_PAGE_H__
#define __WIZARD_START_PAGE_H__

class WizardStartPage : public BaseWizardPage
{
	enum { IDD = IDD_WIZARD_START };
public:
	explicit WizardStartPage(Windower::PluginSettings *pSettings_in);

	void SetSummary(const CString &StartSummary);
	bool ProfileHasChanged() const;

	bool IsPageValid(string_t *pFeedback_out) const;
	bool InitializePage();
	void Revert();
	bool Commit();

protected:
	afx_msg void OnProfileNameChange();
	DECLARE_MESSAGE_MAP()

	CString m_ProfileName;
	CString m_Summary;	
	bool m_bChanged;
};

#endif//__WIZARD_START_PAGE_H__
