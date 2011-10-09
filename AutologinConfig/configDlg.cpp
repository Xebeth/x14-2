// configDlg.cpp : implementation file
//

#include "stdafx.h"
#include <vector>

#include "SettingsManager.h"
#include "AutoLoginSettings.h"

#include "config.h"

#include "CryptUtils.h"
#include "configDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

CconfigDlg::CconfigDlg(CWnd* pParent)
	: CDialog(CconfigDlg::IDD, pParent), m_pSettings(new AutoLoginSettings(_T("autologin.ini")))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CryptUtils::GenerateMachineID(m_EncryptionKey);		
}

CconfigDlg::~CconfigDlg()
{
	delete m_pSettings;
	m_pSettings = NULL;
}

void CconfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CconfigDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CconfigDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT1, &CconfigDlg::OnPasswordChange)
END_MESSAGE_MAP()


// CconfigDlg message handlers

BOOL CconfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	long KeyHash = CryptUtils::Hash(m_EncryptionKey);
	CString StrKeyHash;

	StrKeyHash.Format(_T("0x%08x"), KeyHash);
	SetDlgItemText(IDC_EDIT3, StrKeyHash);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CconfigDlg::OnPaint()
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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CconfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CconfigDlg::OnBnClickedOk()
{
	m_pSettings->SetPassword(m_PasswordHash);
	m_pSettings->SetKeyHash(CryptUtils::Hash(m_EncryptionKey));

	m_pSettings->Save();

	OnOK();
}

void CconfigDlg::OnPasswordChange()
{
	string_t ClearPassword, CryptedPassword;
	CString Password;
	
	GetDlgItemText(IDC_EDIT1, Password);

	ClearPassword = Password.GetBuffer();
	CryptUtils::Crypt(m_EncryptionKey, ClearPassword, CryptedPassword);
	CryptUtils::StringToHex(CryptedPassword, m_PasswordHash);

	SetDlgItemText(IDC_EDIT2, m_PasswordHash.c_str());
}
