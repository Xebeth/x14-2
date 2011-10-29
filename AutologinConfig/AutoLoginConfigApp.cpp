/**************************************************************************
	created		:	2011-10-29
	filename	: 	AutoLoginConfigApp.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	AutoLogin config application
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <vector>

#include <SettingsManager.h>
#include <AutoLoginSettings.h>

#include "AutoLoginConfigDlg.h"
#include "AutoLoginConfigApp.h"

BOOL AutoLoginConfigApp::InitInstance()
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

	AutoLoginConfigDlg ConfigDlg;
	m_pMainWnd = &ConfigDlg;

	ConfigDlg.DoModal();

	return FALSE;
}