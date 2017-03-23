/**************************************************************************
	created		:	2013-05-05
	filename	: 	WizardStartPage.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __WIZARD_START_PAGE_H__
#define __WIZARD_START_PAGE_H__

namespace Windower
{
	class SettingsManager;
}

class WizardStartPage : public BaseWizardPage
{
	enum { IDD = IDD_WIZARD_START };
public:
	WizardStartPage(Windower::SettingsManager &SettingsManager_in,
					Windower::WindowerProfile *pSettings_in);

	void SetSummary(const CString &StartSummary);
	bool ProfileHasChanged() const;

	bool IsPageValid(string_t *pFeedback_out) const override;
	bool InitializePage() override;
	void Revert() override;
	bool Commit() override;

protected:
	afx_msg void OnCheckUpdate(UINT ControlID_in);
	afx_msg void OnProfileNameChange();
	afx_msg void OnPathBrowse();
	DECLARE_MESSAGE_MAP()
	
	Windower::SettingsManager &m_SettingsManager;
	CString m_ProfileName;
	string_t m_GamePath;
	bool m_CheckUpdate;
	CString m_Summary;	
	bool m_Changed;
};

#endif//__WIZARD_START_PAGE_H__
