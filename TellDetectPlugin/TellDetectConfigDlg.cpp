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

extern HINSTANCE g_hInstance;

/*! \brief TellDetectConfigDlg default constructor
	\param[in] : the parent window of the dialog
 */
TellDetectConfigDlg::TellDetectConfigDlg(Windower::PluginSettings *pSettings_in)
	: Windower::PluginPropertyPage(pSettings_in, TellDetectConfigDlg::IDD, IDI_TELLDETECT),
	  m_hModule(GetModuleHandle(NULL)) {}

/*! \brief Member function called in response to the WM_INITDIALOG message
	\return TRUE to set the focus to the first control in the dialog; FALSE if the focus was set manually
*/
BOOL TellDetectConfigDlg::OnInitDialog()
{
	Windower::PluginPropertyPage::OnInitDialog();

	if (m_pSettings != NULL)
	{
		m_SndFile = m_pSettings->GetString(TELL_SOUND_KEY, TELL_SOUND_DEFAULT);
		SetDlgItemText(IDC_TELL_CHIME, m_SndFile);
	}

	return TRUE;
}

//! \brief Message handler called when the user presses the OK button
bool TellDetectConfigDlg::Save()
{
	if (m_pSettings != NULL)
	{
		m_pSettings->SetString(TELL_SOUND_KEY, m_SndFile.GetBuffer());

		return m_pSettings->Save();
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
		PlaySound(m_SndFile, m_hModule, SND_FILENAME | SND_NODEFAULT);
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

	if (m_pSettings != NULL && NewSndFile.Compare(m_SndFile) != 0)
		m_SndFile = NewSndFile;
}