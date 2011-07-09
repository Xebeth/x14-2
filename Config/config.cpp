// config.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <vector>

#include "SimpleIni.h"

#include "SettingsInterface.h"
#include "WindowerSettings.h"
#include "SettingsIniFile.h"

#include "SettingsManager.h"

#include "config.h"
#include "configDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

CconfigApp g_pApp;

// CconfigApp

BEGIN_MESSAGE_MAP(CconfigApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CconfigApp construction

CconfigApp::CconfigApp()
{
	m_pSettingsManager = new SettingsManager(L"config.ini");
}

CconfigApp::~CconfigApp()
{
	if (m_pSettingsManager != NULL)
	{
		delete m_pSettingsManager;
		m_pSettingsManager = NULL;
	}
}

// CconfigApp initialization

BOOL CconfigApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	CconfigDlg ConfigDlg(m_pSettingsManager);
	m_pMainWnd = &ConfigDlg;

	INT_PTR nResponse = ConfigDlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}