/**************************************************************************
	created		:	2011-10-29
	filename	: 	AutoLoginConfigDlg.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Main dialog
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <afxdlgs.h>

#include <PluginPropertyPage.h>

#include "AutoLoginConfigDlg.h"

BEGIN_MESSAGE_MAP(AutoLoginConfigDlg, Windower::PluginPropertyPage)
	ON_BN_CLICKED(IDC_AUTOSUBMIT_CHK, &AutoLoginConfigDlg::OnAutoSubmitCheck)
	ON_EN_CHANGE(IDC_PASSWORD, &AutoLoginConfigDlg::OnPasswordChange)
	ON_EN_CHANGE(IDC_USERNAME, &AutoLoginConfigDlg::OnUsernameChange)
END_MESSAGE_MAP()

/*! \brief AutoLoginConfigDlg default constructor
	\param[in] : the parent window of the dialog
 */
AutoLoginConfigDlg::AutoLoginConfigDlg(Windower::WindowerProfile *pSettings_in, const string_t &PluginName_in)
	: Windower::PluginPropertyPage(pSettings_in, PluginName_in, AutoLoginConfigDlg::IDD, IDI_CONFIG), m_AutoSubmit(false) {}

/*! \brief Initializes the controls of the page from the settings
	\return true if the initialization succeeded; false otherwise
*/
bool AutoLoginConfigDlg::InitializePage()
{
	// retrieve the encryption key hash
	CButton *pAutoSubmitChk = static_cast<CButton*>(GetDlgItem(IDC_AUTOSUBMIT_CHK));
	long KeyHash = CryptUtils::Hash(m_EncryptionKey);
	CString StrKeyHash;

	// set the edit text with the encryption key hash
	StrKeyHash.Format(_T("0x%08x"), KeyHash);

	SetDlgItemText(IDC_ENCRYPTION_KEY_HASH, StrKeyHash);
	SetDlgItemText(IDC_PASSWORD_HASH, m_PasswordHash);
	SetDlgItemText(IDC_USERNAME, m_Username);
	SetDlgItemText(IDC_PASSWORD, _T(""));

	if (pAutoSubmitChk != nullptr)
		pAutoSubmitChk->SetCheck(m_AutoSubmit ? BST_CHECKED : BST_UNCHECKED);

	return true;
}

void AutoLoginConfigDlg::Revert()
{
	CryptUtils::GenerateMachineID(m_EncryptionKey, _T("C:\\"));

	if (m_pSettings != nullptr)
	{
		m_PasswordHash = m_pSettings->GetCryptedPassword();
		m_AutoSubmit = m_pSettings->IsAutoSubmitted();
		m_Username = m_pSettings->GetUsername();
	}
}

//! \brief Message handler called when the user presses the OK button
bool AutoLoginConfigDlg::Commit()
{
	if (IsPageValid(nullptr) && m_pSettings != nullptr)
	{
		m_pSettings->SetCryptedPassword(m_PasswordHash.GetBuffer());
		m_pSettings->SetKeyHash(CryptUtils::Hash(m_EncryptionKey));
		m_pSettings->SetUsername(m_Username.GetBuffer());
		m_pSettings->SetAutoSubmit(m_AutoSubmit);
		
		return true;
	}

	return false;
}

//! \brief Message handler called when the password edit content changes
void AutoLoginConfigDlg::OnPasswordChange()
{
	CString Password;
	
	GetDlgItemText(IDC_PASSWORD, Password);

	if (Password.IsEmpty() == false)
	{
		string_t ClearPassword, CryptedPassword, PasswordHash;

		PasswordHash = m_PasswordHash.GetBuffer();
		ClearPassword = Password.GetBuffer();

		CryptUtils::Crypt(m_EncryptionKey, ClearPassword, CryptedPassword);
		CryptUtils::StringToHex(CryptedPassword, PasswordHash);
		m_PasswordHash = PasswordHash.c_str();

		SetDlgItemText(IDC_PASSWORD_HASH, m_PasswordHash);
		// update the wizard buttons
		UpdateWizardButtons();
	}
}

//! \brief Message handler called when the username edit content changes
void AutoLoginConfigDlg::OnUsernameChange()
{
	GetDlgItemText(IDC_USERNAME, m_Username);
	// update the wizard buttons
	UpdateWizardButtons();
}

//! \brief Message handler called when the username clicks the auto-submit checkbox
void AutoLoginConfigDlg::OnAutoSubmitCheck()
{
	CButton *pAutoSubmitChk = static_cast<CButton*>(GetDlgItem(IDC_AUTOSUBMIT_CHK));

	m_AutoSubmit = (pAutoSubmitChk != nullptr && pAutoSubmitChk->GetCheck() == BST_CHECKED);
}

bool AutoLoginConfigDlg::IsPageValid(string_t *pFeedback_out) const
{
	if (m_Username.IsEmpty())
	{
		if (pFeedback_out != nullptr)
			*pFeedback_out += _T("\n    - The username is empty.");

		return false;
	}

	return true;
}