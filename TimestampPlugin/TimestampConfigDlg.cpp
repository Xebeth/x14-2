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
TimestampConfigDlg::TimestampConfigDlg(Windower::WindowerProfile *pSettings_in, const string_t &PluginName_in)
	: Windower::PluginPropertyPage(pSettings_in, PluginName_in, TimestampConfigDlg::IDD, IDI_TIMESTAMP) {}

/*! \brief Initializes the controls of the page from the settings
	\return true if the initialization succeeded; false otherwise
*/
bool TimestampConfigDlg::InitializePage()
{
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
	SetDlgItemText(IDC_TIMESTAMP_FORMAT, m_TimestampFormat.c_str());

	OnFormatChange();

	return true;
}

void TimestampConfigDlg::Revert()
{
	if (m_pSettings != NULL)
		m_TimestampFormat = m_pSettings->GetTimestampFormat();
}

//! \brief Message handler called when the user presses the OK button
bool TimestampConfigDlg::Commit()
{
	if (IsPageValid(NULL) && m_pSettings != NULL)
	{
		m_pSettings->SetTimestampFormat(m_TimestampFormat);

		return true;
	}

	return false;
}

bool TimestampConfigDlg::IsPageValid(string_t *pFeedback_out) const
{
	if (m_TimestampFormat.empty())
	{
		if (pFeedback_out != NULL)
			*pFeedback_out += _T("\n    - The format string is empty.");

		return false;
	}

	return true;
}

//! \brief Message handler called when the format edit changes
void TimestampConfigDlg::OnFormatChange()
{
	TCHAR PreviewText[51] = { '\0' };
	CString Format;

	GetDlgItemText(IDC_TIMESTAMP_FORMAT, Format);
	m_TimestampFormat = Format.GetBuffer();

	// update the text preview
	if (GetTimeFormat(LOCALE_INVARIANT, NULL, NULL, Format, PreviewText, 32) != 0)
		SetDlgItemText(IDC_PREVIEW_TEXT, PreviewText);
	// update the wizard buttons
	UpdateWizardButtons();
}