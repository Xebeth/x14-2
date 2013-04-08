/**************************************************************************
	created		:	2011-10-29
	filename	: 	WindowerConfigApp.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower config application
**************************************************************************/
#include "stdafx.h"
#include "resource.h"

#include "WindowerConfigDlg.h"
#include "WindowerConfigApp.h"

WindowerConfigApp g_pApp;

WindowerConfigApp::WindowerConfigApp()
{
	TCHAR DirPath[_MAX_PATH] = { '\0' };

	GetCurrentDirectory(_MAX_PATH, DirPath);
	m_pSettingsManager = new Windower::SettingsManager(DirPath, L"config.ini");
}

WindowerConfigApp::~WindowerConfigApp()
{
	if (m_pSettingsManager != NULL)
	{
		delete m_pSettingsManager;
		m_pSettingsManager = NULL;
	}
}

/*! \brief Override parse parameter function called by ParseCommandLine()
	\param[in] lpszParam_in Parameter name or parameter value
	\param[in] bFlag_in If true lpszParam_in is a parameter name else it is a parameter value
	\param[in] bLast_in If true this is the last parameter name or parameter value of the command line
*/
void ConfigCmdLine::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast)
{
	if (bFlag)
	{
		// flags with a mandatory argument
		if (bLast == FALSE)
		{
			if (_tcsicmp(lpszParam, _T("profile")) == 0)
				m_LastFlag = FLAG_PROFILE;
		}
	}
	else
	{
		switch(m_LastFlag)
		{
			case FLAG_PROFILE:
				m_ProfileName.Format(_T("%s%s"), PROFILE_PREFIX, lpszParam);
			break;
		}
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

	ConfigCmdLine CmdInfo;
	CString ProfileName;

	ParseCommandLine(CmdInfo);
	CmdInfo.GetProfileName(ProfileName);

	if (ProfileName.IsEmpty() == false)
		m_pSettingsManager->SetDefaultProfile(ProfileName);

	Windower::WindowerConfigDlg ConfigDlg(m_pSettingsManager);
	m_pMainWnd = &ConfigDlg;

	ConfigDlg.DoModal();

	return FALSE;
}