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

LauncherApp::LauncherApp() : m_pPluginManager(nullptr),
	m_pSettingsManager(nullptr), m_pPluginServices(nullptr) {}

LauncherApp::~LauncherApp()
{
	if (m_pSettingsManager != nullptr)
	{
		delete m_pSettingsManager;
		m_pSettingsManager = nullptr;
	}

	if (m_pPluginManager != nullptr)
	{
		delete m_pPluginManager;
		m_pPluginManager = nullptr;
	}

	if (m_pPluginServices != nullptr)
	{
		delete m_pPluginServices;
		m_pPluginServices = nullptr;
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
		}
#ifdef _DEBUG
		// flags with no argument
		if (_tcsicmp(lpszParam, _T("firstrun")) == 0)
			m_bFirstRun = true;
#endif // _DEBUG
	}
	else
	{
		// ReSharper disable once CppIncompleteSwitchStatement
		switch(m_LastFlag)
		{
			case FLAG_PROFILE:
				m_ProfileName.Format(_T("%s%s"), PROFILE_PREFIX, lpszParam);
			break;
		}
	}
}

bool LauncherApp::CreateLink(const string_t &SavePath_in, const string_t &LinkTarget_in,
							 const string_t &WorkingDir_in, const TCHAR *pArgs_in,
							 const TCHAR *pDesc_in, int IconIndex_in)
{
	IShellLink* pShellLink = nullptr;
	HRESULT hResult;

	if (FAILED(::CoInitialize(nullptr)))
		return false;

	// get a pointer to the IShellLink interface
	hResult = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, reinterpret_cast<LPVOID*>(&pShellLink)); 

	if (SUCCEEDED(hResult)) 
	{ 
		IPersistFile* pPersistentFile; 

		// Set the path to the shortcut target and add the description
		pShellLink->SetIconLocation(LinkTarget_in.c_str(), IconIndex_in);
		pShellLink->SetWorkingDirectory(WorkingDir_in.c_str());		
		pShellLink->SetPath(LinkTarget_in.c_str());

		if (pArgs_in != nullptr)
			pShellLink->SetArguments(pArgs_in);
		
		if (pDesc_in != nullptr)
			pShellLink->SetDescription(pDesc_in); 

		// get a pointer to the IPersistFile interface
		hResult = pShellLink->QueryInterface(IID_IPersistFile, reinterpret_cast<LPVOID*>(&pPersistentFile)); 

		if (SUCCEEDED(hResult) && pPersistentFile != nullptr) 
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
	IShellLink *pShellLink = nullptr;
	HRESULT hResult;

	if (FAILED(::CoInitialize(nullptr)))
		return LinkTarget_out;

	// get a pointer to the IShellLink interface
	hResult = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, reinterpret_cast<LPVOID*>(&pShellLink)); 

	if (SUCCEEDED(hResult) && pShellLink != nullptr) 
	{
		IPersistFile* pPersistentFile = nullptr; 

		// get a pointer to the IPersistFile interface
		hResult = pShellLink->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&pPersistentFile)); 

		if (SUCCEEDED(hResult) && pPersistentFile != nullptr) 
		{
			// Load the shortcut. 
			hResult = pPersistentFile->Load(pLinkPath_in, STGM_READ); 

			if (SUCCEEDED(hResult)) 
			{ 
				// Resolve the link. 
				hResult = pShellLink->Resolve(hWnd_in, 0); 

				if (SUCCEEDED(hResult)) 
				{ 
					WIN32_FIND_DATA wfd = { NULL };

					// Get the path to the link target. 
					pShellLink->GetPath(LinkTarget_out.GetBuffer(MAX_PATH), MAX_PATH, static_cast<WIN32_FIND_DATA*>(&wfd), NULL);
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
/*
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
*/
	// check if a profile was specified on the command line
	if (CmdInfo.GetProfileName(ProfileName).IsEmpty() == false)
	{
		// if it exits, set it as default
		if (m_pSettingsManager->GetSettings(ProfileName) != nullptr)
		{
			m_pSettingsManager->SetDefaultProfile(ProfileName);
			ShowConfig = !m_pSettingsManager->Save();
		}
	}
/*
#ifdef _DEBUG
	if (Tasks != WizardDlg::TASK_NONE)
	{
		WizardDlg ConfigurationWizard(*m_pPluginManager, *m_pSettingsManager);

		if ((Tasks & (WizardDlg::TASK_CHECK_UPDATES | WizardDlg::TASK_CONFIGURE_PLUGINS)) != WizardDlg::TASK_NONE)
			m_pPluginManager->ListPlugins(m_pSettingsManager->GetWorkingDir() + PLUGIN_DIRECTORY);

		if (ConfigurationWizard.CreatePages(Tasks) > 0U)
			ShowConfig = (ConfigurationWizard.DoModal() == IDOK);

		if (Launch)
		{
			m_pSettingsManager->SetDefaultProfile(ConfigurationWizard.GetProfileName());
			m_pSettingsManager->Save();
		}
	}
#endif // _DEBUG
*/
	// load the default profile
	if (m_pSettingsManager->LoadDefaultProfile(CurrentProfile))
	{
		// retrieve the game path
		GamePath = m_pSettingsManager->GetGamePath();
		// default injection chain through boot
		format(ExePath, _T("%sboot\\ffxivboot.exe"), GamePath.c_str());
		format(DLL32Path, _T("%s\\bootstrap.dll"), DirPath);
		Launch = true;
	}

	if (ShowConfig || Launch == false)
	{
		ConfigDlg ConfigurationDlg(*m_pPluginManager, *m_pSettingsManager);

		if ((Tasks & (WizardDlg::TASK_CHECK_UPDATES | WizardDlg::TASK_CONFIGURE_PLUGINS)) == WizardDlg::TASK_NONE)
			m_pPluginManager->ListPlugins(m_pSettingsManager->GetWorkingDir() + PLUGIN_DIRECTORY);

		m_pMainWnd = &ConfigurationDlg;
		// show the configuration dialog
		Launch = (ConfigurationDlg.DoModal() == IDOK);
		m_pMainWnd = nullptr;
	}

	if (Launch)
	{
		PROCESS_INFORMATION ProcessInfo = { nullptr };

		// initialize the structure
		memset(&ProcessInfo, 0, sizeof(ProcessInfo));
		// create the game process
		InjectModule::CreateProcessEx(ExePath, ProcessInfo, CmdLine.c_str(),
									  NORMAL_PRIORITY_CLASS, DLL32Path.c_str());
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

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(VisualManager));
	// tooltip support
	ttParams.m_bVislManagerTheme = TRUE;
	GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	CWinApp::InitInstance();
}