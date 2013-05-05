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
	: Windower::PluginPropertyPage(pSettings_in, AutoLoginConfigDlg::IDD, IDI_CONFIG)	  
{
	// generate the encryption key
	if (m_pSettings != NULL)
		CryptUtils::GenerateMachineID(m_EncryptionKey, m_pSettings->GetSettingsDrive().c_str());
}

/*! \brief Member function called in response to the WM_INITDIALOG message
	\return TRUE to set the focus to the first control in the dialog; FALSE if the focus was set manually
*/
BOOL AutoLoginConfigDlg::OnInitDialog()
{
	Windower::PluginPropertyPage::OnInitDialog();

	// retrieve the encryption key hash
	long KeyHash = CryptUtils::Hash(m_EncryptionKey);
	CString StrKeyHash;

	// set the edit text with the encryption key hash
	StrKeyHash.Format(_T("0x%08x"), KeyHash);
	SetDlgItemText(IDC_ENCRYPTION_KEY_HASH, StrKeyHash);

	if (m_pSettings != NULL)
	{
		CButton *pAutoSubmitChk = static_cast<CButton*>(GetDlgItem(IDC_AUTOSUBMIT_CHK));

		m_PasswordHash = m_pSettings->GetString(PASSWORD_KEY);
		SetDlgItemText(IDC_PASSWORD_HASH, m_PasswordHash.c_str());

		m_Username = m_pSettings->GetString(USERNAME_KEY);
		SetDlgItemText(IDC_USERNAME, m_Username.c_str());

		m_AutoSubmit = (m_pSettings->GetLong(AUTO_SUBMIT_KEY) == 1UL) ? true : false;

		if (pAutoSubmitChk != NULL)
			pAutoSubmitChk->SetCheck(m_AutoSubmit ? BST_CHECKED : BST_UNCHECKED);
	}

	return TRUE;
}

//! \brief Message handler called when the user presses the OK button
bool AutoLoginConfigDlg::Save()
{
	if (m_pSettings != NULL)
	{
		m_pSettings->SetString(USERNAME_KEY,	m_Username);
		m_pSettings->SetString(PASSWORD_KEY,	m_PasswordHash);	
		m_pSettings->SetLong(AUTO_SUBMIT_KEY,	m_AutoSubmit ? 1L : 0L);
		m_pSettings->SetLong(KEY_HASH_KEY,		CryptUtils::Hash(m_EncryptionKey));	

		return m_pSettings->Save();
	}

	return false;
}

//! \brief Message handler called when the password edit content changes
void AutoLoginConfigDlg::OnPasswordChange()
{
	string_t ClearPassword, CryptedPassword;
	CString Password;
	
	GetDlgItemText(IDC_PASSWORD, Password);

	ClearPassword = Password.GetBuffer();
	CryptUtils::Crypt(m_EncryptionKey, ClearPassword, CryptedPassword);
	CryptUtils::StringToHex(CryptedPassword, m_PasswordHash);

	SetDlgItemText(IDC_PASSWORD_HASH, m_PasswordHash.c_str());
}

//! \brief Message handler called when the username edit content changes
void AutoLoginConfigDlg::OnUsernameChange()
{
	CString Username;

	GetDlgItemText(IDC_USERNAME, Username);
	m_Username = Username.GetBuffer();
}

//! \brief Message handler called when the username clicks the auto-submit checkbox
void AutoLoginConfigDlg::OnAutoSubmitCheck()
{
	CButton *pAutoSubmitChk = static_cast<CButton*>(GetDlgItem(IDC_AUTOSUBMIT_CHK));

	m_AutoSubmit = (pAutoSubmitChk != NULL && pAutoSubmitChk->GetCheck() == BST_CHECKED);
}