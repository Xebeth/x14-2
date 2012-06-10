/**************************************************************************
	created		:	2011-10-29
	filename	: 	TimestampConfigDlg.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Main dialog
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <vector>

#include <CryptUtils.h>
#include <SettingsManager.h>
#include "TimestampSettings.h"

#include "TimestampConfigDlg.h"

BEGIN_MESSAGE_MAP(TimestampConfigDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &TimestampConfigDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_TIMESTAMP_FORMAT, &TimestampConfigDlg::OnFormatChange)
END_MESSAGE_MAP()

/*! \brief TimestampConfigDlg default constructor
	\param[in] : the parent window of the dialog
 */
TimestampConfigDlg::TimestampConfigDlg(const TCHAR *pProfileName_in, CWnd* pParentWnd_in)
	: CDialog(TimestampConfigDlg::IDD, pParentWnd_in),
	  m_pSettings(new Windower::TimestampSettings(_T("config.ini"), pProfileName_in))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_TIMESTAMP);
}

//! \brief TimestampConfigDlg destructor
TimestampConfigDlg::~TimestampConfigDlg()
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
BOOL TimestampConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	CEdit *pFormatEdit = static_cast<CEdit*>(GetDlgItem(IDC_TIMESTAMP_FORMAT));
	CString FormatHelp;

	pFormatEdit->SetLimitText(50);

	FormatHelp = _T("Default value: [HH:mm:ss]\r\n\r\n")
				 _T("h\tHours with no leading zero for single-digit hours; 12-hour clock\r\n")
				 _T("hh\tHours with leading zero for single-digit hours; 12-hour clock\r\n")
				 _T("H\tHours with no leading zero for single-digit hours; 24-hour clock\r\n")
				 _T("HH\tHours with leading zero for single-digit hours; 24-hour clock\r\n")
				 _T("m\tMinutes with no leading zero for single-digit minutes\r\n")
				 _T("mm\tMinutes with leading zero for single-digit minutes\r\n")
				 _T("s\tSeconds with no leading zero for single-digit seconds\r\n")
				 _T("ss\tSeconds with leading zero for single-digit seconds\r\n")
				 _T("t\tOne character time marker string, such as A or P\r\n")
				 _T("tt\tMulti-character time marker string, such as AM or PM\r\n\r\n")
				 _T("Characters in the format string that are enclosed in single quotation marks\r\n")
				 _T("appear in the same location and unchanged in the output string.");

	SetDlgItemText(IDC_FORMAT_HELP, FormatHelp);

	if (m_pSettings != NULL)
		SetDlgItemText(IDC_TIMESTAMP_FORMAT, m_pSettings->GetFormat().c_str());

	OnFormatChange();

	return TRUE;
}

/*! \brief Member function called when Windows or an application makes
		   a request to repaint a portion of the window
*/
void TimestampConfigDlg::OnPaint()
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
HCURSOR TimestampConfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//! \brief Message handler called when the user presses the OK button
void TimestampConfigDlg::OnBnClickedOk()
{
	CString Format;

	GetDlgItemText(IDC_TIMESTAMP_FORMAT, Format);
	m_pSettings->SetFormat(Format.GetBuffer());

	m_pSettings->Save();

	OnOK();
}

//! \brief Message handler called when the format edit changes
void TimestampConfigDlg::OnFormatChange()
{
	CString Format;

	GetDlgItemText(IDC_TIMESTAMP_FORMAT, Format);
	TCHAR PreviewText[51] = { '\0' };

	// update the text preview
	if (GetTimeFormat(LOCALE_INVARIANT, NULL, NULL, Format.GetBuffer(), PreviewText, 32) != 0)
		SetDlgItemText(IDC_PREVIEW_TEXT, PreviewText);
}