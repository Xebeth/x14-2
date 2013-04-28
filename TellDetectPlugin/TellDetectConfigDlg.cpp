/**************************************************************************
	created		:	2011-10-29
	m_Filename	: 	TellDetectConfigDlg.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Main dialog
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <afxdlgs.h>

#include "TellDetectSettings.h"
#include "TellDetectConfigDlg.h"

BEGIN_MESSAGE_MAP(TellDetectConfigDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &TellDetectConfigDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BROWSE, &TellDetectConfigDlg::OnBrowse)
	ON_BN_CLICKED(IDC_PLAY_SOUND, &TellDetectConfigDlg::OnPlayChime)
END_MESSAGE_MAP()

extern HINSTANCE g_hInstance;

/*! \brief TellDetectConfigDlg default constructor
	\param[in] : the parent window of the dialog
 */
TellDetectConfigDlg::TellDetectConfigDlg(const TCHAR *pConfigFile_in, const TCHAR *pProfileName_in, CWnd* pParentWnd_in)
	: CDialog(TellDetectConfigDlg::IDD, pParentWnd_in), m_hModule(GetModuleHandle(NULL)),
	  m_pSettings(new Windower::TellDetectSettings(pConfigFile_in, pProfileName_in))
{
	CWinApp *pApp = AfxGetApp();

	m_hIcon = pApp->LoadIcon(IDI_TELLDETECT);
}

//! \brief TellDetectConfigDlg destructor
TellDetectConfigDlg::~TellDetectConfigDlg()
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
BOOL TellDetectConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	if (m_pSettings != NULL)
	{
		m_Filename = m_pSettings->GetFilename().c_str();
		SetDlgItemText(IDC_TELL_CHIME, m_Filename);
	}

	return TRUE;
}

/*! \brief Member function called when Windows or an application makes
		   a request to repaint a portion of the window
*/
void TellDetectConfigDlg::OnPaint()
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
HCURSOR TellDetectConfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//! \brief Message handler called when the user presses the OK button
void TellDetectConfigDlg::OnBnClickedOk()
{
	GetDlgItemText(IDC_TELL_CHIME, m_Filename);
	m_pSettings->SetFilename(m_Filename.GetBuffer());

	m_pSettings->Save();

	OnOK();
}

//! \brief Message handler called when the user presses the chime preview button
void TellDetectConfigDlg::OnPlayChime()
{
	CButton *pPlayButton = static_cast<CButton*>(GetDlgItem(IDC_PLAY_SOUND));

	if (m_Filename.IsEmpty() == false && pPlayButton->IsWindowEnabled())
	{
		pPlayButton->EnableWindow(FALSE);
		PlaySound(m_Filename, m_hModule, SND_FILENAME | SND_NODEFAULT);
		pPlayButton->EnableWindow(TRUE);
	}
}

//! \brief Message handler called when the user presses the chime preview button
void TellDetectConfigDlg::OnBrowse()
{
	int Pos = m_Filename.ReverseFind('\\');
	CString DefaultName = m_Filename.Right(m_Filename.GetLength() - Pos);
	CFileDialog Browse(TRUE, _T("*.wav"), DefaultName, OPEN_DIALOG_FLAGS,
					   _T("(*.wav) Sound files|*.wav||"));

	if (Browse.DoModal() == IDOK)
	{
		m_Filename = Browse.GetPathName();
		SetDlgItemText(IDC_TELL_CHIME, m_Filename);
	}
}