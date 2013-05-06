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

#include "AutoLogin.h"
#include "AutoLoginConfigDlg.h"

BEGIN_MESSAGE_MAP(AutoLoginConfigDlg, Windower::PluginPropertyPage)
	ON_BN_CLICKED(IDC_AUTOSUBMIT_CHK, &AutoLoginConfigDlg::OnAutoSubmitCheck)
	ON_EN_CHANGE(IDC_PASSWORD, &AutoLoginConfigDlg::OnPasswordChange)
	ON_EN_CHANGE(IDC_USERNAME, &AutoLoginConfigDlg::OnUsernameChange)
END_MESSAGE_MAP()

/*! \brief AutoLoginConfigDlg default constructor
	\param[in] : the parent window of the dialog
 */
AutoLoginConfigDlg::AutoLoginConfigDlg(Windower::PluginSettings *pSettings_in)
	: Windower::PluginPropertyPage(pSettings_in, AutoLoginConfigDlg::IDD, IDI_CONFIG) {}

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

	if (pAutoSubmitChk != NULL)
		pAutoSubmitChk->SetCheck(m_AutoSubmit ? BST_CHECKED : BST_UNCHECKED);

	return true;
}

void AutoLoginConfigDlg::Revert()
{
	CryptUtils::GenerateMachineID(m_EncryptionKey, GetSettingsDrive().c_str());
	m_AutoSubmit = (GetLong(AUTO_SUBMIT_KEY) == 1L);
	m_PasswordHash = GetString(PASSWORD_KEY);
	m_Username = GetString(USERNAME_KEY);
}

//! \brief Message handler called when the user presses the OK button
bool AutoLoginConfigDlg::Commit()
{
	if (IsPageValid(NULL))
	{
		SetString(USERNAME_KEY,	m_Username.GetBuffer());
		SetLong(AUTO_SUBMIT_KEY, m_AutoSubmit ? 1L : 0L);
		SetString(PASSWORD_KEY,	m_PasswordHash.GetBuffer());
		SetHex(KEY_HASH_KEY, CryptUtils::Hash(m_EncryptionKey));	

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

	m_AutoSubmit = (pAutoSubmitChk != NULL && pAutoSubmitChk->GetCheck() == BST_CHECKED);
}

bool AutoLoginConfigDlg::IsPageValid(string_t *pFeedback_out) const
{
	if (m_Username.IsEmpty())
	{
		if (pFeedback_out != NULL)
			*pFeedback_out += _T("\n    - The username is empty.");

		return false;
	}

	return true;
}