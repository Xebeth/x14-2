 /**************************************************************************
	created		:	2013-05-05
	filename	: 	WizardStartPage.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <afxdlgs.h>

#include <PluginPropertyPage.h>

#include "BaseWizardPage.h"
#include "WizardStartPage.h"
#include "WizardDlg.h"

BEGIN_MESSAGE_MAP(WizardStartPage, BaseWizardPage)
	ON_EN_CHANGE(IDC_PROFILE_NAME, &WizardStartPage::OnProfileNameChange)
END_MESSAGE_MAP()

/*! \brief TellDetectConfigDlg default constructor
	\param[in] : the parent window of the dialog
 */
WizardStartPage::WizardStartPage(Windower::PluginSettings *pSettings_in)
	: BaseWizardPage(pSettings_in, WizardStartPage::IDD, IDR_MAINFRAME),
	  m_bChanged(false)
{
	m_psp.dwFlags |= PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
	m_PageFlags = WizardDlg::TASK_START_SUMMARY;	
}

bool WizardStartPage::InitializePage()
{
	m_psp.pszHeaderSubTitle = _T("It will guide you through the basic configuration.");
	m_psp.pszHeaderTitle = _T("Welcome to the x14-2 configuration wizard!");
	
	SetDlgItemText(IDC_PROFILE_NAME, m_ProfileName);
	SetDlgItemText(IDC_START_SUMMARY, m_Summary);

	return true;
}

void WizardStartPage::SetSummary(const CString &StartSummary)
{ m_Summary = StartSummary; }

void WizardStartPage::OnProfileNameChange()
{
	GetDlgItemText(IDC_PROFILE_NAME, m_ProfileName);
	// update the wizard buttons
	UpdateWizardButtons();
}

bool WizardStartPage::IsPageValid(string_t *pFeedback_out) const
{
	CString ProfileName;

	GetDlgItemText(IDC_PROFILE_NAME, ProfileName);

	if (ProfileName.IsEmpty())
	{
		if (pFeedback_out != NULL)
			*pFeedback_out += _T("\n    - The profile name is empty.");

		return false;
	}

	return true;
}

void WizardStartPage::Revert()
{
	m_ProfileName = GetProfileName();
	m_bChanged = false;
}

bool WizardStartPage::Commit()
{
	if (IsPageValid(NULL))
	{
		string_t ProfileName = PROFILE_PREFIX;

		ProfileName += m_ProfileName.GetBuffer();
		m_bChanged = SetCurrentSection(ProfileName);

		return true;
	}

	return false;
}

bool WizardStartPage::ProfileHasChanged() const
{ return m_bChanged; }