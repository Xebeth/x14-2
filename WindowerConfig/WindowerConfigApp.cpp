/**************************************************************************
	created		:	2011-10-29
	filename	: 	WindowerConfigApp.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower config application
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <vector>

#include <SettingsManager.h>
#include <WindowerSettings.h>
#include <WindowerSettingsManager.h>

#include "WindowerConfigDlg.h"
#include "WindowerConfigApp.h"

WindowerConfigApp g_pApp;

WindowerConfigApp::WindowerConfigApp()
{
	m_pSettingsManager = new Windower::SettingsManager(L"config.ini");
}

WindowerConfigApp::~WindowerConfigApp()
{
	if (m_pSettingsManager != NULL)
	{
		delete m_pSettingsManager;
		m_pSettingsManager = NULL;
	}
}

BOOL WindowerConfigApp::InitInstance()
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

	Windower::WindowerConfigDlg ConfigDlg(m_pSettingsManager);
	m_pMainWnd = &ConfigDlg;

	ConfigDlg.DoModal();

	return FALSE;
}