/**************************************************************************
	created		:	2011-10-29
	filename	: 	TimestampConfigDlg.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Main dialog
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <afxdlgs.h>

#include <PluginPropertyPage.h>

#include "TimestampConfigDlg.h"

BEGIN_MESSAGE_MAP(TimestampConfigDlg, Windower::PluginPropertyPage)
	ON_EN_CHANGE(IDC_TIMESTAMP_FORMAT, &TimestampConfigDlg::OnFormatChange)
END_MESSAGE_MAP()

/*! \brief TimestampConfigDlg default constructor
	\param[in] : the parent window of the dialog
 */
TimestampConfigDlg::TimestampConfigDlg(Windower::PluginSettings *pSettings_in)
	: Windower::PluginPropertyPage(pSettings_in, TimestampConfigDlg::IDD, IDI_TIMESTAMP) {}

/*! \brief Member function called in response to the WM_INITDIALOG message
	\return TRUE to set the focus to the first control in the dialog; FALSE if the focus was set manually
*/
BOOL TimestampConfigDlg::OnInitDialog()
{
	CEdit *pFormatEdit = static_cast<CEdit*>(GetDlgItem(IDC_TIMESTAMP_FORMAT));
	CString FormatHelp;

	Windower::PluginPropertyPage::OnInitDialog();

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
		SetDlgItemText(IDC_TIMESTAMP_FORMAT, m_pSettings->GetString(TIMESTAMP_KEY, TIMESTAMP_DEFAULT));

	OnFormatChange();

	return TRUE;
}

//! \brief Message handler called when the user presses the OK button
bool TimestampConfigDlg::Save()
{
	if (m_pSettings != NULL)
	{
		CString Format;

		GetDlgItemText(IDC_TIMESTAMP_FORMAT, Format);
		m_pSettings->SetString(TIMESTAMP_KEY, Format.GetBuffer());

		return m_pSettings->Save();
	}

	return false;
}

//! \brief Message handler called when the format edit changes
void TimestampConfigDlg::OnFormatChange()
{
	CString Format;

	GetDlgItemText(IDC_TIMESTAMP_FORMAT, Format);
	TCHAR PreviewText[51] = { '\0' };

	// update the text preview
	if (GetTimeFormat(LOCALE_INVARIANT, NULL, NULL, Format, PreviewText, 32) != 0)
		SetDlgItemText(IDC_PREVIEW_TEXT, PreviewText);
}