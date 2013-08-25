/**************************************************************************
	created		:	2013-04-18
	filename	: 	UpdaterDlg.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <afxdlgs.h>

#include <PluginPropertyPage.h>

#include "BaseWizardPage.h"
#include "UpdaterDlg.h"
#include "WindowerUpdater.h"
#include "WizardDlg.h"

BEGIN_MESSAGE_MAP(UpdaterDlg, BaseWizardPage)
	ON_BN_CLICKED(IDC_START, &UpdaterDlg::OnStartClick)
END_MESSAGE_MAP()

UpdaterDlg::UpdaterDlg(Windower::WindowerProfile *pSettings_in)
	: BaseWizardPage(pSettings_in, _T(""), IDD_UPDATE_DLG, IDR_MAINFRAME),
	  m_bStarted(false), m_pUpdater(NULL)
{
	m_pUpdater = new Windower::WindowerUpdater(_T("http://woodart.free.fr/_tmp/config.zip"), this);
	m_PageFlags = WizardDlg::TASK_CHECK_UPDATES;
	m_psp.dwFlags |= PSP_HIDEHEADER;
}

UpdaterDlg::~UpdaterDlg()
{
	if (m_pUpdater != NULL)
	{
		delete m_pUpdater;
		m_pUpdater = NULL;
	}
}

bool UpdaterDlg::InitializePage()
{
	m_pProgressCtrl = static_cast<CProgressCtrl*>(GetDlgItem(IDC_DL_PROGRESS));

	return __super::InitializePage();
}

void UpdaterDlg::OnProgress(unsigned long Completed_in, unsigned long Total_in, const TCHAR *pFeedbackMsg_in)
{
	if (m_pProgressCtrl != NULL)
	{
		m_pProgressCtrl->SetRange32(0, Total_in);
		m_pProgressCtrl->SetPos(Completed_in);

		if (Completed_in <= Total_in && Total_in != 0UL)
		{
			float ProgressPercent = 100.f * ((float)Completed_in / Total_in);

			m_ProgressText.Format(_T("%ld / %ld bytes (%0.2f%%)"), Completed_in, Total_in, ProgressPercent);
			GetDlgItem(IDC_DL_SIZE)->SetWindowText(m_ProgressText);
		}

		if (pFeedbackMsg_in == NULL || m_StatusText.Compare(pFeedbackMsg_in) != 0)
		{
			GetDlgItem(IDC_STATUS_TXT)->SetWindowText(pFeedbackMsg_in);
			m_StatusText = pFeedbackMsg_in;
		}
	}
}

void UpdaterDlg::OnFinish()
{
	if (::IsWindow(m_hWnd))
	{
		CButton *pStart = static_cast<CButton*>(GetDlgItem(IDC_START));

		pStart->SetWindowText(_T("Start"));
		pStart->EnableWindow(TRUE);
	}

	if (m_pUpdater != NULL)
	{
		m_pUpdater->SaveFile(_T("F:\\config.ini"), _T("config.ini"));
	}

	m_bStarted = false;
}

void UpdaterDlg::OnStart()
{
	if (::IsWindow(m_hWnd))
	{
		CButton *pStart = static_cast<CButton*>(GetDlgItem(IDC_START));

		pStart->SetWindowText(_T("Stop"));
		pStart->EnableWindow(TRUE);

		if (m_pProgressCtrl != NULL)
			m_pProgressCtrl->SetStep(0);
	}

	m_bStarted = true;
}

void UpdaterDlg::OnCancel()
{
	if (::IsWindow(m_hWnd))
	{
		CButton *pStart = static_cast<CButton*>(GetDlgItem(IDC_START));

		pStart->SetWindowText(_T("Start"));
		pStart->EnableWindow(TRUE);

		if (m_pProgressCtrl != NULL)
			m_pProgressCtrl->SetStep(0);
	}

	m_bStarted = false;
}

void UpdaterDlg::OnStartClick()
{
	GetDlgItem(IDC_START)->EnableWindow(FALSE);

	if (m_pUpdater != NULL)
	{
		if (m_bStarted)
			m_pUpdater->Cancel();
		else
			m_pUpdater->Start();
	}
}

void UpdaterDlg::PumpMessages()
{
	AfxPumpMessage();
}