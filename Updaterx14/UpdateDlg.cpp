/**************************************************************************
	created		:	2013-04-18
	filename	: 	UpdateDlg.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include "resource.h"

#include "UpdateDlg.h"

#include "WindowerUpdater.h"

BEGIN_MESSAGE_MAP(UpdateDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_START, &UpdateDlg::OnStartClick)
END_MESSAGE_MAP()

UpdateDlg::UpdateDlg(Windower::SettingsManager *pSettingsManager, CWnd* pParent)
	: CDialog(IDD_UPDATE_DLG, pParent), m_pSettingsManager(pSettingsManager), m_bStarted(false)
{
	m_pServices = new DummyServices(__PLUGIN_FRAMEWORK_VERSION__, m_pSettingsManager->GetWorkingDir() + _T("config.ini"));
	m_pPluginManager = new PluginFramework::PluginManager(m_pServices);

	m_pUpdater = new WindowerUpdater(_T("http://woodart.free.fr/_tmp/OR3D.7z"), this);

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

UpdateDlg::~UpdateDlg()
{
	if (m_pPluginManager != NULL)
	{
		delete m_pPluginManager;
		m_pPluginManager = NULL;
	}

	if (m_pServices != NULL)
	{
		delete m_pServices;
		m_pServices = NULL;
	}

	if (m_pUpdater != NULL)
	{
		delete m_pUpdater;
		m_pUpdater = NULL;
	}
}

BOOL UpdateDlg::OnInitDialog()
{
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CDialog::OnInitDialog();

	m_pProgressCtrl = static_cast<CProgressCtrl*>(GetDlgItem(IDC_DL_PROGRESS));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void UpdateDlg::OnPaint()
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
HCURSOR UpdateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void UpdateDlg::OnProgress(unsigned long Completed_in, unsigned long Total_in, const TCHAR *pFeedbackMsg_in)
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

void UpdateDlg::OnFinish()
{
	if (::IsWindow(m_hWnd))
	{
		CButton *pStart = static_cast<CButton*>(GetDlgItem(IDC_START));

		pStart->SetWindowText(_T("Start"));
		pStart->EnableWindow(TRUE);

		if (m_pProgressCtrl != NULL)
			m_pProgressCtrl->SetStep(0xFFFFFFFF);
	}

	if (m_pUpdater != NULL)
	{
		m_pUpdater->SaveFile(_T("F:\\LOL.7z"));
	}

	m_bStarted = false;
}

void UpdateDlg::OnStart()
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

void UpdateDlg::OnCancel()
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

void UpdateDlg::OnStartClick()
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

void UpdateDlg::PumpMessages()
{
	AfxPumpMessage();
}