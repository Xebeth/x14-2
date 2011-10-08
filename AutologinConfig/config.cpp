// config.cpp : Defines the class behaviors for the application.
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

AutologingConfigApp g_pApp;

// CconfigApp

BEGIN_MESSAGE_MAP(AutologingConfigApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


BOOL AutologingConfigApp::InitInstance()
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

	CconfigDlg ConfigDlg;
	m_pMainWnd = &ConfigDlg;

	ConfigDlg.DoModal();

	return FALSE;
}