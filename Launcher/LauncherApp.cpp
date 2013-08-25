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
#include "ConfigDlg.h"
#include "WizardDlg.h"

LauncherApp g_pApp;

LauncherApp::LauncherApp() : m_pSettingsManager(NULL),
	m_pPluginManager(NULL), m_pPluginServices(NULL) {}

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

	if (m_pPluginServices != NULL)
	{
		delete m_pPluginServices;
		m_pPluginServices = NULL;
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
		if (_tcsicmp(lpszParam, _T("firstrun")) == 0)
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

bool LauncherApp::CreateLink(const string_t &SavePath_in, const string_t &LinkTarget_in,
							 const string_t &WorkingDir_in, const TCHAR *pArgs_in,
							 const TCHAR *pDesc_in, int IconIndex_in)
{
	IShellLink* pShellLink = NULL;
	HRESULT hResult = S_FALSE;

	::CoInitialize(NULL);

	// get a pointer to the IShellLink interface
	hResult = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&pShellLink); 

	if (SUCCEEDED(hResult)) 
	{ 
		IPersistFile* pPersistentFile; 

		// Set the path to the shortcut target and add the description
		pShellLink->SetIconLocation(LinkTarget_in.c_str(), IconIndex_in);
		pShellLink->SetWorkingDirectory(WorkingDir_in.c_str());		
		pShellLink->SetPath(LinkTarget_in.c_str());

		if (pArgs_in != NULL)
			pShellLink->SetArguments(pArgs_in);
		
		if (pDesc_in != NULL)
			pShellLink->SetDescription(pDesc_in); 

		// get a pointer to the IPersistFile interface
		hResult = pShellLink->QueryInterface(IID_IPersistFile, (LPVOID*)&pPersistentFile); 

		if (SUCCEEDED(hResult) && pPersistentFile != NULL) 
		{ 
			// Save the link by calling IPersistFile::Save
			hResult = pPersistentFile->Save(SavePath_in.c_str(), TRUE); 
			pPersistentFile->Release(); 
		} 

		pShellLink->Release(); 
	} 

	::CoUninitialize();

	return SUCCEEDED(hResult);
}

CString& LauncherApp::ResolveLink(HWND hWnd_in, const TCHAR *pLinkPath_in, CString &LinkTarget_out)
{
	IShellLink *pShellLink = NULL;
	HRESULT hResult = S_FALSE;

	::CoInitialize(NULL);

	// get a pointer to the IShellLink interface
	hResult = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&pShellLink); 

	if (SUCCEEDED(hResult) && pShellLink != NULL) 
	{
		IPersistFile* pPersistentFile = NULL; 

		// get a pointer to the IPersistFile interface
		hResult = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistentFile); 

		if (SUCCEEDED(hResult) && pPersistentFile != NULL) 
		{
			// Load the shortcut. 
			hResult = pPersistentFile->Load(pLinkPath_in, STGM_READ); 

			if (SUCCEEDED(hResult)) 
			{ 
				// Resolve the link. 
				hResult = pShellLink->Resolve(hWnd_in, 0); 

				if (SUCCEEDED(hResult)) 
				{ 
					WIN32_FIND_DATA wfd;

					// Get the path to the link target. 
					hResult = pShellLink->GetPath(LinkTarget_out.GetBuffer(MAX_PATH), MAX_PATH, (WIN32_FIND_DATA*)&wfd, NULL); 
					// unlock the buffer
					LinkTarget_out.ReleaseBuffer(-1);
				} 
			} 

			// Release the pointer to the IPersistFile interface. 
			pPersistentFile->Release(); 
		} 

		// Release the pointer to the IShellLink interface. 
		pShellLink->Release(); 
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
	string_t DLL32Path, ExePath, CmdLine, GamePath;	
	Windower::WindowerProfile CurrentProfile;
	bool ShowConfig = true, Launch = false;
	TCHAR DirPath[_MAX_PATH] = { '\0' };	
	UINT Tasks = WizardDlg::TASK_NONE;
	CString SID, ProfileName;
	LauncherCmdLine CmdInfo;

	InitMFC();

	GetCurrentDirectory(_MAX_PATH, DirPath);
	ParseCommandLine(CmdInfo);

	m_pSettingsManager = new Windower::SettingsManager(DirPath, _T("config.ini"));
	m_pPluginServices = new Windower::PluginServices(__PLUGIN_FRAMEWORK_VERSION__,
													 m_Modules,
													 m_pSettingsManager);
	m_pPluginManager = new PluginFramework::PluginManager(m_pPluginServices);
	
	if (CmdInfo.IsFirstRun() || m_pSettingsManager->IsConfigLoaded() == false)
	{
		// for the first time use, perform all the tasks
		Tasks = WizardDlg::ALL_TASKS_MASK;
	}
	else
	{
		// check for updates
		if (m_pSettingsManager->IsAutoUpdated())
			Tasks |= WizardDlg::TASK_CHECK_UPDATES;
		// check the game path
		if (m_pSettingsManager->IsGamePathValid() == false)
			Tasks |= WizardDlg::TASK_CONFIGURE_PATH;
	}
	// check if a profile was specified on the command line
	if (CmdInfo.GetProfileName(ProfileName).IsEmpty() == false)
	{
		// if it exits, set it as default
		if (m_pSettingsManager->GetSettings(ProfileName) != NULL)
		{
			m_pSettingsManager->SetDefaultProfile(ProfileName);
			ShowConfig = !m_pSettingsManager->Save();
		}
	}

	if (Tasks != WizardDlg::TASK_NONE)
	{
		WizardDlg ConfigurationWizard(*m_pPluginManager, *m_pSettingsManager);

		if ((Tasks & (WizardDlg::TASK_CHECK_UPDATES | WizardDlg::TASK_CONFIGURE_PLUGINS)) != WizardDlg::TASK_NONE)
			m_pPluginManager->ListPlugins(m_pSettingsManager->GetWorkingDir() + _T("plugins"));

		if (ConfigurationWizard.CreatePages(Tasks) > 0U)
			ShowConfig = (ConfigurationWizard.DoModal() == IDOK);

		if (Launch)
		{
			m_pSettingsManager->SetDefaultProfile(ConfigurationWizard.GetProfileName());
			m_pSettingsManager->Save();
		}
	}

	// load the default profile
	if (m_pSettingsManager->LoadDefaultProfile(CurrentProfile))
	{
		// retrieve the game path
		GamePath = m_pSettingsManager->GetGamePath();
		// check if an SID has been provided
		if (CmdInfo.GetSID(SID).IsEmpty() == false)
		{
			// inject directly into the game
			format(ExePath, _T("%sgame\\ffxiv.exe"), GamePath.c_str());
			format(DLL32Path, _T("%s\\x14-2core.dll"), DirPath);
			// generate the command line
			Launch = CreateCmdLine(CmdLine, GamePath, CurrentProfile.GetLanguage(), SID);
		}
		else
		{
			// default injection chain through boot
			format(ExePath, _T("%sboot\\ffxivboot.exe"), GamePath.c_str());
			format(DLL32Path, _T("%s\\bootstrap.dll"), DirPath);
			Launch = true;
		}
	}

	if (ShowConfig || Launch == false)
	{
		ConfigDlg ConfigurationDlg(*m_pPluginManager, *m_pSettingsManager);

		if ((Tasks & (WizardDlg::TASK_CHECK_UPDATES | WizardDlg::TASK_CONFIGURE_PLUGINS)) == WizardDlg::TASK_NONE)
			m_pPluginManager->ListPlugins(m_pSettingsManager->GetWorkingDir() + _T("plugins"));

		m_pMainWnd = &ConfigurationDlg;
		// show the configuration dialog
		Launch = (ConfigurationDlg.DoModal() == IDOK);
		m_pMainWnd = NULL;
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

			// initialize the structure
			memset(&ProcessInfo, 0, sizeof(ProcessInfo));
			// create the game process
			InjectModule::CreateProcessEx(ExePath, ProcessInfo, CmdLine.c_str(),
										  CreationFlags, DLL32Path.c_str());
			// cleanup
			CloseHandle(ProcessInfo.hProcess);
			CloseHandle(ProcessInfo.hThread);
		}
	}

	VisualManager::DestroyInstance();

	return FALSE;
}

/*! \brief 
*/
void LauncherApp::InitMFC()
{
	INITCOMMONCONTROLSEX InitCtrls;
	CMFCToolTipInfo ttParams;

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
}