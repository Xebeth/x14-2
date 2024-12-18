/**************************************************************************
	created		:	2011-10-29
	m_SndFile	: 	TellDetectConfigDlg.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Main dialog
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <afxdlgs.h>

#include <PluginPropertyPage.h>

#include "TellDetectConfigDlg.h"

BEGIN_MESSAGE_MAP(TellDetectConfigDlg, Windower::PluginPropertyPage)
	ON_BN_CLICKED(IDC_BROWSE, &TellDetectConfigDlg::OnBrowse)
	ON_BN_CLICKED(IDC_PLAY_SOUND, &TellDetectConfigDlg::OnPlayChime)
	ON_EN_CHANGE(IDC_TELL_CHIME, &TellDetectConfigDlg::OnChimeChange)
END_MESSAGE_MAP()

/*! \brief TellDetectConfigDlg default constructor
	\param[in] : the parent window of the dialog
 */
TellDetectConfigDlg::TellDetectConfigDlg(Windower::WindowerProfile *pSettings_in, const string_t &PluginName_in)
	: Windower::PluginPropertyPage(pSettings_in, PluginName_in, TellDetectConfigDlg::IDD, IDI_TELLDETECT) {}

/*! \brief Initializes the controls of the page from the settings
	\return true if the initialization succeeded; false otherwise
*/
bool TellDetectConfigDlg::InitializePage()
{
	SetDlgItemText(IDC_TELL_CHIME, m_SndFile);

	return true;
}

void TellDetectConfigDlg::Revert()
{
	if (m_pSettings != nullptr)
		m_SndFile = m_pSettings->GetTellSound();
}

//! \brief Message handler called when the user presses the OK button
bool TellDetectConfigDlg::Commit()
{
	if (IsPageValid(nullptr) && m_pSettings != nullptr)
	{
		m_pSettings->SetTellSound(m_SndFile.GetBuffer());

		return true;
	}

	return false;
}

//! \brief Message handler called when the user presses the chime preview button
void TellDetectConfigDlg::OnPlayChime()
{
	CButton *pPlayButton = static_cast<CButton*>(GetDlgItem(IDC_PLAY_SOUND));

	if (m_SndFile.IsEmpty() == false && pPlayButton->IsWindowEnabled())
	{
		pPlayButton->EnableWindow(FALSE);
		PlaySound(m_SndFile, nullptr, SND_FILENAME | SND_NODEFAULT);
		pPlayButton->EnableWindow(TRUE);
	}
}

//! \brief Message handler called when the user presses the chime preview button
void TellDetectConfigDlg::OnBrowse()
{
	int Pos = m_SndFile.ReverseFind('\\');
	CString DefaultName = m_SndFile.Right(m_SndFile.GetLength() - Pos);
	CFileDialog Browse(TRUE, _T("*.wav"), DefaultName, OPEN_DIALOG_FLAGS,
					   _T("(*.wav) Sound files|*.wav||"));

	if (Browse.DoModal() == IDOK)
		SetDlgItemText(IDC_TELL_CHIME, Browse.GetPathName());
}

void TellDetectConfigDlg::OnChimeChange()
{
	CString NewSndFile;

	GetDlgItemText(IDC_TELL_CHIME, NewSndFile);

	if (NewSndFile.Compare(m_SndFile) != 0)
		m_SndFile = NewSndFile;

	// update the play button
	GetDlgItem(IDC_PLAY_SOUND)->EnableWindow(m_SndFile.IsEmpty() == false);
	// update the wizard buttons
	UpdateWizardButtons();
}

bool TellDetectConfigDlg::IsPageValid(string_t *pFeedback_out) const
{
	if (m_SndFile.IsEmpty())
	{
		if (pFeedback_out != nullptr)
			*pFeedback_out += _T("\n    - The sound file path is empty.");

		return false;
	}

	return true;
}