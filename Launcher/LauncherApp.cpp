/**************************************************************************
	created		:	2011-05-09
	filename	: 	launcher.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower x14-2 DLL injector
**************************************************************************/
#include "stdafx.h"

#include "LauncherApp.h"

LauncherApp g_pApp;

LauncherApp::LauncherApp()
{
	TCHAR DirPath[_MAX_PATH] = { '\0' };

	GetCurrentDirectory(_MAX_PATH, DirPath);
	m_pSettingsManager = new Windower::SettingsManager(DirPath, L"config.ini");
}

LauncherApp::~LauncherApp()
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
void LauncherCmdLine::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast)
{
	if (bFlag)
	{
		// flags with a mandatory argument
		if (bLast == FALSE)
		{
			if (_tcsicmp(lpszParam, _T("profile")) == 0)
				m_LastFlag = FLAG_PROFILE;
			else if (_tcsicmp(lpszParam, _T("SID")) == 0)
				m_LastFlag = FLAG_SID;
		}
	}
	else
	{
		switch(m_LastFlag)
		{
			case FLAG_PROFILE:
				m_ProfileName.Format(_T("%s%s"), PROFILE_PREFIX, lpszParam);
			break;
			case FLAG_SID:
				m_SID = lpszParam;
			break;
		}
	}
}

BOOL LauncherApp::InitInstance()
{
	DWORD CreationFlags(CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT);
	string_t DLL32Path, ExePath, CmdLine, GamePath;
	TCHAR DirPath[_MAX_PATH] = { '\0' };
	PROCESS_INFORMATION ProcessInfo;
	INITCOMMONCONTROLSEX InitCtrls;
	UpdateDlg *pUpdaterDlg = NULL;
	const TCHAR *pCmdLine = NULL;
	LauncherCmdLine CmdInfo;	

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	GetCurrentDirectory(_MAX_PATH, DirPath);
	ParseCommandLine(CmdInfo);

#ifdef _DEBUG
	if (m_pSettingsManager->IsAutoUpdated())
	{
		// _CrtSetBreakAlloc(956);

		m_pMainWnd = pUpdaterDlg = new UpdateDlg(m_pSettingsManager);

		pUpdaterDlg->DoModal();


		if (pUpdaterDlg != NULL)
		{
			delete pUpdaterDlg;
			m_pMainWnd = pUpdaterDlg = NULL;
		}

		return FALSE;
	}
#endif // _DEBUG

	if (m_pSettingsManager->IsGamePathValid())
	{
		CString ProfileName;

		CmdInfo.GetProfileName(ProfileName);

		if (ProfileName.IsEmpty() == false)
		{
			// if it exits, set it as default
			if (m_pSettingsManager->GetSettings(ProfileName) != NULL)
			{
				m_pSettingsManager->SetDefaultProfile(ProfileName);
				m_pSettingsManager->Save();
			}
		}

		GamePath = m_pSettingsManager->GetGamePath();

		// default injection chain through boot
		format(ExePath, _T("%sboot\\ffxivboot.exe"), GamePath.c_str());
		format(DLL32Path, _T("%s\\bootstrap.dll"), DirPath);

//#ifdef _DEBUG
		#define GAMEVER_BUFFER_SIZE 24

		CString SID;

		CmdInfo.GetSID(SID);

		if (SID.IsEmpty() == false)
		{
			Windower::WindowerProfile Settings;

			if (m_pSettingsManager->LoadDefaultProfile(Settings))
			{
				FILE *pVersionFile = NULL;						
				string_t VerFile;

				format(VerFile, _T("%sgame\\ffxivgame.ver"), GamePath.c_str());

				if (_tfopen_s(&pVersionFile, VerFile.c_str(), _T("r")) == 0)
				{
					TCHAR GameVersion[GAMEVER_BUFFER_SIZE];

					memset(GameVersion, 0, GAMEVER_BUFFER_SIZE * sizeof(TCHAR));

					// retrieve the game version
					if (_fgetts(GameVersion, GAMEVER_BUFFER_SIZE, pVersionFile))
					{
						// inject directly into the game
						format(ExePath, _T("%sgame\\ffxiv.exe"), m_pSettingsManager->GetGamePath());
						format(DLL32Path, _T("%s\\Windowerx14.dll"), DirPath);
						// create the command line parameters
						format(CmdLine, _T("\"DEV.TestSID=%s\"")
										_T(" \"DEV.UseSqPack=1\" \"DEV.DataPathType=1\"")
										_T(" \"DEV.LobbyHost=neolobby01.ffxiv.com\"")
										_T(" \"SYS.LobbyHost=neolobby01.ffxiv.com\"")
										_T(" \"SYS.Region=3\"")
										_T(" \"language=%ld\"")
										_T(" \"ver=%s\""), 
										SID, Settings.GetLanguage(), GameVersion);
						// set the command line
						pCmdLine = CmdLine.c_str();
					}
					// cleanup
					fclose(pVersionFile);
				}
			}
		}
//#endif // _DEBUG

		// create the process
		InjectModule::CreateProcessEx(ExePath, ProcessInfo, pCmdLine,
									  CreationFlags, DLL32Path.c_str());
	}
	else
	{
		MessageBox(NULL, _T("The configuration file is missing or invalid.\n")
						 _T("Starting the configuration utility..."),
						 _T("Error!"), MB_OK | MB_ICONWARNING);
		// start the configuration tool
		ShellExecute(NULL, NULL, _T("WindowerConfig.exe"), NULL, DirPath, SW_SHOW);
	}

	if (pUpdaterDlg != NULL)
	{
		delete pUpdaterDlg;
		m_pMainWnd = pUpdaterDlg = NULL;
	}

	return FALSE;
}