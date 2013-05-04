/**************************************************************************
	created		:	2011-05-09
	filename	: 	launcher.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower x14-2 DLL injector
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <shlobj.h>

#include "EditLinkTarget.h"
#include "LauncherApp.h"
#include "UpdaterDlg.h"
#include "ConfigDlg.h"

LauncherApp g_pApp;

LauncherApp::LauncherApp()
{
	TCHAR DirPath[_MAX_PATH] = { '\0' };

	GetCurrentDirectory(_MAX_PATH, DirPath);
	m_pSettingsManager = new Windower::SettingsManager(DirPath, _T("config.ini"));
	m_pDummyServices = new DummyServices(__PLUGIN_FRAMEWORK_VERSION__, m_pSettingsManager->GetConfigFile());
	m_pPluginManager = new PluginFramework::PluginManager(m_pDummyServices);
}

LauncherApp::~LauncherApp()
{
	if (m_pSettingsManager != NULL)
	{
		delete m_pSettingsManager;
		m_pSettingsManager = NULL;
	}

	if (m_pPluginManager != NULL)
	{
		delete m_pPluginManager;
		m_pPluginManager = NULL;
	}

	if (m_pDummyServices != NULL)
	{
		delete m_pDummyServices;
		m_pDummyServices = NULL;
	}

	VisualManager::DestroyInstance();
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
		// flags with no argument
		else if (_tcsicmp(lpszParam, _T("firstrun")) == 0)
			m_bFirstRun = true;
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

CString& LauncherApp::ResolveLink(HWND hWnd_in, const TCHAR *pLinkPath_in, CString &LinkTarget_out)
{
	IShellLink *psl;
	HRESULT hres;

	::CoInitialize(NULL);

	// Get a pointer to the IShellLink interface. It is assumed that CoInitialize
	// has already been called. 
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl); 
	if (SUCCEEDED(hres)) 
	{
		IPersistFile* ppf; 

		// Get a pointer to the IPersistFile interface. 
		hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf); 

		if (SUCCEEDED(hres)) 
		{
			// Load the shortcut. 
			hres = ppf->Load(pLinkPath_in, STGM_READ); 

			if (SUCCEEDED(hres)) 
			{ 
				// Resolve the link. 
				hres = psl->Resolve(hWnd_in, 0); 

				if (SUCCEEDED(hres)) 
				{ 
					WIN32_FIND_DATA wfd;

					// Get the path to the link target. 
					hres = psl->GetPath(LinkTarget_out.GetBuffer(MAX_PATH), MAX_PATH, (WIN32_FIND_DATA*)&wfd, NULL); 
					// unlock the buffer
					LinkTarget_out.ReleaseBuffer(-1);
				} 
			} 

			// Release the pointer to the IPersistFile interface. 
			ppf->Release(); 
		} 

		// Release the pointer to the IShellLink interface. 
		psl->Release(); 
	} 

	::CoUninitialize();

	return LinkTarget_out;
}

bool LauncherApp::CreateCmdLine(string_t &CmdLine_out, const string_t &GamePath_in,
								long LanguageID_in, const TCHAR *pSID_in) const
{
	if (pSID_in != NULL)
	{
		FILE *pVersionFile = NULL;						
		string_t VerFile;
		
		format(VerFile, _T("%sgame\\ffxivgame.ver"), GamePath_in.c_str());
		
		if (_tfopen_s(&pVersionFile, VerFile.c_str(), _T("r")) == 0)
		{
			TCHAR GameVersion[GAMEVER_BUFFER_SIZE];
		
			memset(GameVersion, 0, GAMEVER_BUFFER_SIZE * sizeof(TCHAR));
		
			// retrieve the game version
			if (_fgetts(GameVersion, GAMEVER_BUFFER_SIZE, pVersionFile))
			{
				// create the command line parameters
				format(CmdLine_out, _T("\"DEV.TestSID=%s\"")
									_T(" \"DEV.UseSqPack=1\" \"DEV.DataPathType=1\"")
									_T(" \"DEV.LobbyHost=neolobby01.ffxiv.com\"")
									_T(" \"SYS.LobbyHost=neolobby01.ffxiv.com\"")
									_T(" \"SYS.Region=3\"")
									_T(" \"language=%ld\"")
									_T(" \"ver=%s\""), 
									pSID_in, LanguageID_in, GameVersion);
			}
			// cleanup
			fclose(pVersionFile);

			return true;
		}
	}

	return false;
}
BOOL LauncherApp::InitInstance()
{
	bool ShowConfig = true, Launch = false, PluginsInit = false;
	string_t DLL32Path, ExePath, CmdLine, GamePath;
	TCHAR DirPath[_MAX_PATH] = { '\0' };
	Windower::WindowerProfile Settings;
	INITCOMMONCONTROLSEX InitCtrls;
	UpdaterDlg *pUpdaterDlg = NULL;	
	ConfigDlg *pConfigDlg = NULL;
	CMFCToolTipInfo ttParams;
	CString SID, ProfileName;
	LauncherCmdLine CmdInfo;

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	// disable Aero to prevent bugs with dialogs client area => see VisualManager::IsOwnerDrawCaption()
	// this forces AFX_GLOBAL_DATA.DwmIsCompositionEnabled() to return FALSE
	afxGlobalData.bDisableAero = TRUE;
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(VisualManager));
	// tooltip support
	ttParams.m_bVislManagerTheme = TRUE;
	GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	CWinApp::InitInstance();

	GamePath = m_pSettingsManager->GetGamePath();
	GetCurrentDirectory(_MAX_PATH, DirPath);
	ParseCommandLine(CmdInfo);

#ifdef _DEBUG
	// check for updates first since it could change 
	// what "valid" is for the configuration...
	if (m_pSettingsManager->IsAutoUpdated())
	{
		if (m_pPluginManager != NULL)
		{
			m_pPluginManager->ListPlugins(m_pSettingsManager->GetWorkingDir() + _T("plugins"));
			PluginsInit = true;
		}

		pUpdaterDlg = new UpdaterDlg(m_pSettingsManager);

		pUpdaterDlg->DoModal();

		if (pUpdaterDlg != NULL)
		{
			delete pUpdaterDlg;
			pUpdaterDlg = NULL;
		}
	}
#endif // _DEBUG

	// check if the game path is configured or if it is a fresh start
	if (m_pSettingsManager->IsGamePathValid() == false || CmdInfo.IsFirstRun())
	{
		MessageBox(NULL, _T("The configuration file is missing or invalid.\n"),
							_T("Error!"), MB_OK | MB_ICONWARNING);

		// @TODO Configuration wizard
		while (m_pSettingsManager->IsGamePathValid() == false
			&& m_pSettingsManager->SelectDirectory(GamePath))
		{
			m_pSettingsManager->SetGamePath(GamePath.c_str());
		}
		// save the config
		ShowConfig = !m_pSettingsManager->Save();
	}
	// check if a profile 
	if (CmdInfo.GetProfileName(ProfileName).IsEmpty() == false)
	{
		// if it exits, set it as default
		if (m_pSettingsManager->GetSettings(ProfileName) != NULL)
		{
			m_pSettingsManager->SetDefaultProfile(ProfileName);
			ShowConfig = !m_pSettingsManager->Save();
		}
	}

	// load the default profile
	if (m_pSettingsManager->LoadDefaultProfile(Settings))
	{
		// check if an SID has been provided
		if (CmdInfo.GetSID(SID).IsEmpty() == false)
		{
			// inject directly into the game
			format(ExePath, _T("%sgame\\ffxiv.exe"), GamePath.c_str());
			format(DLL32Path, _T("%s\\x14-2core.dll"), DirPath);
			// generate the command line
			Launch = CreateCmdLine(CmdLine, GamePath, Settings.GetLanguage(), SID);
		}
		else
		{
			// default injection chain through boot
			format(ExePath, _T("%sboot\\ffxivboot.exe"), GamePath.c_str());
			format(DLL32Path, _T("%s\\bootstrap.dll"), DirPath);
			Launch = true;
		}
	}

	if (m_pSettingsManager != NULL && m_pPluginManager != NULL && (ShowConfig || Launch == false))
	{
		if (PluginsInit == false)
			m_pPluginManager->ListPlugins(m_pSettingsManager->GetWorkingDir() + _T("plugins"));

		m_pMainWnd = pConfigDlg = new ConfigDlg(*m_pPluginManager, *m_pSettingsManager);
		Launch = (pConfigDlg->DoModal() == IDOK);

		if (pConfigDlg != NULL)
		{
			delete pConfigDlg;
			m_pMainWnd = pConfigDlg = NULL;
		}
	}

	if (Launch)
	{
#ifdef _DEBUG
		HANDLE hProcess = InjectModule::FindProcess(_T("ffxiv.exe"));

		if (hProcess != NULL)
		{
			HMODULE hDLL = NULL;

			// inject directly into the game
			format(ExePath, _T("%sgame\\ffxiv.exe"), GamePath.c_str());
			format(DLL32Path, _T("%s\\x14-2core.dll"), DirPath);
			// inject the DLL in the running game
			InjectModule::InjectModule(hProcess, DLL32Path.c_str());

			CloseHandle(hProcess);
		}
		else
#endif // _DEBUG
		{
			DWORD CreationFlags(CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT);
			PROCESS_INFORMATION ProcessInfo;

			// create the game process
			InjectModule::CreateProcessEx(ExePath, ProcessInfo, CmdLine.c_str(),
											CreationFlags, DLL32Path.c_str());
			// cleanup
			CloseHandle(ProcessInfo.hProcess);
			CloseHandle(ProcessInfo.hThread);
		}
	}

	return FALSE;
}