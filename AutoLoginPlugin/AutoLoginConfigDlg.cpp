/**************************************************************************
	created		:	2011-10-29
	filename	: 	AutoLoginConfigDlg.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Main dialog
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <vector>

#include <CryptUtils.h>
#include <SettingsManager.h>
#include "AutoLoginSettings.h"

#include "AutoLoginConfigDlg.h"

BEGIN_MESSAGE_MAP(AutoLoginConfigDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &AutoLoginConfigDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_PASSWORD, &AutoLoginConfigDlg::OnPasswordChange)
	ON_EN_CHANGE(IDC_USERNAME, &AutoLoginConfigDlg::OnUsernameChange)
END_MESSAGE_MAP()

/*! \brief AutoLoginConfigDlg default constructor
	\param[in] : the parent window of the dialog
 */
AutoLoginConfigDlg::AutoLoginConfigDlg(const TCHAR *pConfigFile_in, const TCHAR *pProfileName_in, CWnd* pParentWnd_in)
	: CDialog(AutoLoginConfigDlg::IDD, pParentWnd_in),
	  m_pSettings(new Windower::AutoLoginSettings(pConfigFile_in, pProfileName_in))
{
	string_t ConfigFile = pConfigFile_in;
	std::list<string_t> Tokens;

	m_hIcon = AfxGetApp()->LoadIcon(IDI_CONFIG);
	// retrieve the drive letter from the config path
	tokenize(ConfigFile, Tokens, _T("\\"), _T("\""));	
	// generate the encryption key
	CryptUtils::GenerateMachineID(m_EncryptionKey, Tokens.front().c_str());
}

//! \brief AutoLoginConfigDlg destructor
AutoLoginConfigDlg::~AutoLoginConfigDlg()
{
	if (m_pSettings != NULL)
	{
		delete m_pSettings;
		m_pSettings = NULL;
	}
}

/*! \brief Member function called in response to the WM_INITDIALOG message
	\return TRUE to set the focus to the first control in the dialog; FALSE if the focus was set manually
*/
BOOL AutoLoginConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	if (m_pSettings->Load())
	{
		// retrieve the encryption key hash
		long KeyHash = CryptUtils::Hash(m_EncryptionKey);
		CString StrKeyHash;

		// set the edit text with the encryption key hash
		StrKeyHash.Format(_T("0x%08x"), KeyHash);
		SetDlgItemText(IDC_ENCRYPTION_KEY_HASH, StrKeyHash);

		if (m_pSettings != NULL)
		{
			m_PasswordHash = m_pSettings->GetPassword();
			SetDlgItemText(IDC_PASSWORD_HASH, m_PasswordHash.c_str());

			m_Username = m_pSettings->GetUsername();
			SetDlgItemText(IDC_USERNAME, m_Username.c_str());
		}
	}

	return TRUE;
}

/*! \brief Member function called when Windows or an application makes
		   a request to repaint a portion of the window
*/
void AutoLoginConfigDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

/*! \brief Member function called by a minimized window that does not have an icon defined for its class
	\return a cursor or icon handle
*/
HCURSOR AutoLoginConfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//! \brief Message handler called when the user presses the OK button
void AutoLoginConfigDlg::OnBnClickedOk()
{
	m_pSettings->SetUsername(m_Username);
	m_pSettings->SetPassword(m_PasswordHash);
	m_pSettings->SetKeyHash(CryptUtils::Hash(m_EncryptionKey));

	m_pSettings->Save();

	OnOK();
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
