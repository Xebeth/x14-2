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
	ON_CONTROL_RANGE(BN_CLICKED, IDC_UPDATE_YES, IDC_UPDATE_NO, &WizardStartPage::OnCheckUpdate)
	ON_EN_CHANGE(IDC_PROFILE_NAME, &WizardStartPage::OnProfileNameChange)
	ON_BN_CLICKED(IDC_BROWSE_GAME_PATH, &WizardStartPage::OnPathBrowse)	
END_MESSAGE_MAP()

/*! \brief TellDetectConfigDlg default constructor
	\param[in] : the parent window of the dialog
 */
WizardStartPage::WizardStartPage(Windower::SettingsManager &SettingsManager_in, 
								 Windower::WindowerProfile *pSettings_in)
	: BaseWizardPage(pSettings_in, _T(""), WizardStartPage::IDD, IDR_MAINFRAME),
	  m_Changed(false), m_CheckUpdate(false), m_SettingsManager(SettingsManager_in)
{
	m_psp.dwFlags |= PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
	m_PageFlags = WizardDlg::TASK_START_SUMMARY;	
}

bool WizardStartPage::InitializePage()
{
	m_psp.pszHeaderSubTitle = _T("It will guide you through the basic configuration.");
	m_psp.pszHeaderTitle = _T("Welcome to the x14-2 configuration wizard!");
	
	CheckRadioButton(IDC_UPDATE_YES, IDC_UPDATE_NO, m_CheckUpdate ? IDC_UPDATE_YES : IDC_UPDATE_NO);
	SetDlgItemText(IDC_GAME_PATH, m_GamePath.c_str());
	SetDlgItemText(IDC_PROFILE_NAME, m_ProfileName);
	SetDlgItemText(IDC_START_SUMMARY, m_Summary);
	
	return __super::InitializePage();
}

void WizardStartPage::SetSummary(const CString &StartSummary)
{ m_Summary = StartSummary; }

void WizardStartPage::OnCheckUpdate(UINT ControlID_in)
{
	m_CheckUpdate = (ControlID_in == IDC_UPDATE_YES);
}

void WizardStartPage::OnProfileNameChange()
{
	GetDlgItemText(IDC_PROFILE_NAME, m_ProfileName);
	// update the wizard buttons
	UpdateWizardButtons();
}

void WizardStartPage::OnPathBrowse()
{
	bool ValidPath = false;

	if (m_SettingsManager.SelectDirectory(m_GamePath))
	{
		if (m_SettingsManager.CheckGamePath(m_GamePath))
			SetDlgItemText(IDC_GAME_PATH, m_GamePath.c_str());
		else
			MessageBox(_T("The specified game directory is incorrect ('boot'/'game' missing)."), _T("Error!"), MB_OK | MB_ICONSTOP);
	}
	// update the wizard buttons
	UpdateWizardButtons();
}

bool WizardStartPage::IsPageValid(string_t *pFeedback_out) const
{
	CString Value;
	bool Result = true;

	GetDlgItemText(IDC_PROFILE_NAME, Value);

	if (Value.IsEmpty())
	{
		Result = false;

		if (pFeedback_out != NULL)
			*pFeedback_out += _T("\n    - The profile name is empty.");
	}

	GetDlgItemText(IDC_GAME_PATH, Value);

	if (Value.IsEmpty())
	{
		Result = false;

		if (pFeedback_out != NULL)
			*pFeedback_out += _T("\n    - The game path is empty.");
	}

	return Result;
}

void WizardStartPage::Revert()
{
	if (m_pSettings != NULL)
		m_ProfileName = m_pSettings->GetName() + _tcslen(PROFILE_PREFIX);

	m_CheckUpdate = m_SettingsManager.IsAutoUpdated();
	m_GamePath = m_SettingsManager.GetGamePath();
	m_Changed = false;
}

bool WizardStartPage::Commit()
{
	if (IsPageValid(NULL) && m_pSettings != NULL)
	{
		string_t Current = m_SettingsManager.GetDefaultProfile();
		string_t ProfileName = PROFILE_PREFIX;

		ProfileName += m_ProfileName.GetBuffer();

		if (Current.compare(ProfileName) != 0)
		{
			m_SettingsManager.SetDefaultProfile(ProfileName.c_str());
			m_pSettings->SetName(ProfileName.c_str());
			m_Changed = true;			
		}

		m_SettingsManager.SetGamePath(m_GamePath.c_str());
		m_SettingsManager.SetAutoUpdate(m_CheckUpdate);

		return true;
	}

	return false;
}

bool WizardStartPage::ProfileHasChanged() const
{ return m_Changed; }