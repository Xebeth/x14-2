/**************************************************************************
	created		:	2011-10-29
	filename	: 	WindowerConfigDlg.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Main dialog
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <vector>
#include <d3d9.h>

#include <CryptUtils.h>
#include <InjectModule.h>
#include <SettingsManager.h>
#include <WindowerSettings.h>
#include <WindowerSettingsManager.h>

#include <PluginFramework.h>
#include <IPluginServices.h>
#include <PluginManager.h>

#include "WindowerConfigDlg.h"

using namespace PluginFramework;

namespace Windower
{
	BEGIN_MESSAGE_MAP(WindowerConfigDlg, CDialog)
		ON_WM_PAINT()
		ON_WM_QUERYDRAGICON()

		ON_CBN_SELCHANGE(IDC_PROFILES_COMBO, &WindowerConfigDlg::OnProfilesChange)
		ON_CBN_KILLFOCUS(IDC_PROFILES_COMBO, &WindowerConfigDlg::OnProfileRename)

		ON_CBN_SELCHANGE(IDC_LNG_COMBO, &WindowerConfigDlg::OnLanguageChange)

		ON_BN_CLICKED(IDC_DELETE_PROFILE, &WindowerConfigDlg::OnDeleteProfile)
		ON_BN_CLICKED(IDC_BROWSE_GAME_PATH, &WindowerConfigDlg::OnPathBrowse)
		ON_BN_CLICKED(IDC_NEW_PROFILE, &WindowerConfigDlg::OnNewProfile)		
		ON_BN_CLICKED(IDC_VSYNC, &WindowerConfigDlg::OnVSyncChange)		

		ON_EN_CHANGE(IDC_GAME_PATH, &WindowerConfigDlg::OnPathChange)

		ON_NOTIFY(NM_DBLCLK, IDC_PLUGIN_LIST, OnConfigure)
		
		ON_BN_CLICKED(IDC_SAVE, &WindowerConfigDlg::OnSave)
		ON_BN_CLICKED(IDOK, &WindowerConfigDlg::OnLaunch)		
	END_MESSAGE_MAP()

	WindowerConfigDlg::WindowerConfigDlg(SettingsManager *pSettingsManager, CWnd* pParent)
		: CDialog(WindowerConfigDlg::IDD, pParent), m_pSettingsManager(pSettingsManager)
	{
		m_pServices = new DummyServices(__PLUGIN_FRAMEWORK_VERSION__, m_pSettingsManager->GetWorkingDir() + _T("config.ini"));
		m_pPluginManager = new PluginManager(m_pServices);

		m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		m_pCurrentSettings = NULL;
		m_CurrentSel = 0;
	}

	WindowerConfigDlg::~WindowerConfigDlg()
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
	}

	void WindowerConfigDlg::InsertPlugin(CListCtrl *pListCtrl_in_out, const PluginInfo &PluginInfo_in)
	{
		if (pListCtrl_in_out == NULL)
			return;

		LVITEM LvItem;
		string_t Text;

		LvItem.mask = LVIF_TEXT | LVIF_PARAM;
		LvItem.lParam = (LPARAM)&PluginInfo_in;
		LvItem.iItem = 0;
		
		// plugin name
		LvItem.iSubItem = LIST_COL_NAME;
		LvItem.pszText = (LPTSTR)PluginInfo_in.GetName().c_str();

		int Index = pListCtrl_in_out->InsertItem(&LvItem);

		LvItem.iItem = Index;
		LvItem.mask = LVIF_TEXT;

		// plugin version
		LvItem.iSubItem = LIST_COL_VERSION;
		Text = PluginInfo_in.GetVersion();
		LvItem.pszText = (LPTSTR)Text.c_str();
		pListCtrl_in_out->SetItem(&LvItem);
		// plugin version
		LvItem.iSubItem = LIST_COL_FRAMEWORK;
		Text = PluginInfo_in.GetFrameworkVersion();
		LvItem.pszText = (LPTSTR)Text.c_str();
		pListCtrl_in_out->SetItem(&LvItem);
		// plugin author
		LvItem.iSubItem = LIST_COL_AUTHOR;
		LvItem.pszText = (LPTSTR)PluginInfo_in.GetAuthor().c_str();
		pListCtrl_in_out->SetItem(&LvItem);
		// plugin compatibility
		LvItem.iSubItem = LIST_COL_COMPAT;
		switch(PluginInfo_in.GetCompatibilityFlags())
		{
		case 1:
			Text = _T("Windower");
			break;
		case 2:
			Text = _T("Bootstrap");
			break;
		case -1:
			Text = _T("Any");
			break;
		}
		LvItem.pszText = (LPTSTR)Text.c_str();
		pListCtrl_in_out->SetItem(&LvItem);
		// plugin description
		LvItem.iSubItem = LIST_COL_DESC;
		LvItem.pszText = (LPTSTR)PluginInfo_in.GetDesc().c_str();
		pListCtrl_in_out->SetItem(&LvItem);
		// plugin path
		LvItem.iSubItem = LIST_COL_PATH;
		LvItem.pszText = (LPTSTR)PluginInfo_in.GetDLLPath().c_str();
		pListCtrl_in_out->SetItem(&LvItem);

		if (m_pCurrentSettings != NULL)
			pListCtrl_in_out->SetCheck(Index, m_pCurrentSettings->IsPluginActive(PluginInfo_in.GetName()));
	}

	BOOL WindowerConfigDlg::OnInitDialog()
	{
		CComboBox *pProfiles = static_cast<CComboBox*>(GetDlgItem(IDC_PROFILES_COMBO));
		CListCtrl *pPluginList = static_cast<CListCtrl*>(GetDlgItem(IDC_PLUGIN_LIST));
		CComboBox *pLng = static_cast<CComboBox*>(GetDlgItem(IDC_LNG_COMBO));
		CEdit *pGamePath = static_cast<CEdit*>(GetDlgItem(IDC_GAME_PATH));
		string_t GamePath = m_pSettingsManager->GetGamePath();

		CDialog::OnInitDialog();

		// Set the icon for this dialog.  The framework does this automatically
		//  when the application's main window is not a dialog
		SetIcon(m_hIcon, TRUE);			// Set big icon
		SetIcon(m_hIcon, FALSE);		// Set small icon

		pPluginList->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES);

		pPluginList->InsertColumn(LIST_COL_NAME,		_T("Plugin name"),	 LVCFMT_LEFT, 100);
		pPluginList->InsertColumn(LIST_COL_VERSION,		_T("Version"),		 LVCFMT_LEFT, 50);
		pPluginList->InsertColumn(LIST_COL_FRAMEWORK,	_T("Framework"),	 LVCFMT_LEFT, 70);
		pPluginList->InsertColumn(LIST_COL_AUTHOR,		_T("Author"),		 LVCFMT_LEFT, 80);
		pPluginList->InsertColumn(LIST_COL_COMPAT,		_T("Compatibility"), LVCFMT_LEFT, 80);
		pPluginList->InsertColumn(LIST_COL_DESC,		_T("Description"),	 LVCFMT_LEFT, 400);
		pPluginList->InsertColumn(LIST_COL_PATH,		_T("Filename"),		 LVCFMT_LEFT, 300);

		pGamePath->SetWindowText(GamePath.c_str());

		while (m_pSettingsManager->IsGamePathValid() == false)
		{
			if (m_pSettingsManager->SelectDirectory(GamePath))
				pGamePath->SetWindowText(GamePath.c_str());
			else
				break;
		}

		// fill the language combo
		pLng->AddString(_T("日本語"));
		pLng->AddString(_T("English"));
		pLng->AddString(_T("German"));
		pLng->AddString(_T("Français"));

		if (pProfiles != NULL)
		{
			const WindowerSettings& Profiles = m_pSettingsManager->GetProfiles();
			const TCHAR *pDefault = m_pSettingsManager->GetDefaultProfile();
			WindowerSettings::const_iterator SettingIt;
			WindowerProfile *pProfile;
			const TCHAR *pName;
			int ItemCount = 0;

			for (SettingIt = Profiles.begin(); SettingIt !=  Profiles.end(); ++SettingIt)
			{
				pProfile = SettingIt->second;

				if (pProfile != NULL)
				{
					pName = pProfile->GetName();

					ItemCount = pProfiles->AddString(pName + PROFILE_PREFIX_LENGTH);
					pProfiles->SetItemData(ItemCount, (DWORD_PTR)SettingIt->second);

					if (_tcscmp(pName, pDefault) == 0)
						pProfiles->SetCurSel(ItemCount);

					++ItemCount;
				}
			}

			OnProfilesChange();

			GetDlgItem(IDC_DELETE_PROFILE)->EnableWindow(ItemCount > 1);
		}

		if (m_pPluginManager != NULL && m_pPluginManager->ListPlugins(m_pSettingsManager->GetWorkingDir() + _T("plugins")) > 0U)
		{
			const RegisteredPlugins& Plugins = m_pPluginManager->GetRegisteredPlugins();
			RegisteredPlugins::const_iterator PluginIt;

			for (PluginIt = Plugins.begin(); PluginIt != Plugins.end(); ++PluginIt)
				InsertPlugin(pPluginList, PluginIt->second);
		}

		return TRUE;  // return TRUE  unless you set the focus to a control
	}

	void WindowerConfigDlg::OnConfigure(NMHDR* pNMHDR, LRESULT* pResult)
	{
		if (m_pCurrentSettings != NULL)
		{
			CListCtrl *pPluginList = static_cast<CListCtrl*>(GetDlgItem(IDC_PLUGIN_LIST));

			for (int Index = 0; Index < pPluginList->GetItemCount(); ++Index)
			{
				if (pPluginList->GetItemState(Index, LVIS_SELECTED) == LVIS_SELECTED)
				{
					DWORD_PTR ItemData = pPluginList->GetItemData(Index);

					if (ItemData != -1 && ItemData != NULL)
					{
						PluginInfo *pPluginInfo = reinterpret_cast<PluginInfo*>(ItemData);
						const string_t &PluginName = pPluginInfo->GetName();
						bool Result;
					
						Result = m_pPluginManager->ConfigurePlugin(PluginName, (LPVOID)m_pCurrentSettings->GetName());
						m_pPluginManager->UnloadPlugin(PluginName);

						if (Result)
							m_pSettingsManager->Reload();

						break;
					}
				}
			}
		}

		*pResult = 0;
	}

	// If you add a minimize button to your dialog, you will need the code below
	//  to draw the icon.  For MFC applications using the document/view model,
	//  this is automatically done for you by the framework.

	void WindowerConfigDlg::OnPaint()
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
	HCURSOR WindowerConfigDlg::OnQueryDragIcon()
	{
		return static_cast<HCURSOR>(m_hIcon);
	}

	void WindowerConfigDlg::OnProfileRename()
	{
		CComboBox *pProfiles = static_cast<CComboBox*>(GetDlgItem(IDC_PROFILES_COMBO));

		if (m_pCurrentSettings != NULL && pProfiles != NULL)
		{
			CString DisplayName, CurrentName;
			string_t NewName;

			pProfiles->GetWindowText(DisplayName);
			CurrentName = m_pCurrentSettings->GetName();
			format(NewName, _T("%s%s"), PROFILE_PREFIX, DisplayName);			

			// update the profile name
			if (CurrentName.Compare(NewName.c_str()) != 0)
			{
				if (m_pSettingsManager->RenameProfile(m_pCurrentSettings, NewName))
				{
					// delete the current combo item
					pProfiles->DeleteString(m_CurrentSel);
					// add the item back (update the display name as RenameProfile 
					// could have changed it in case the provided was a duplicate)
					DisplayName = NewName.c_str() + PROFILE_PREFIX_LENGTH;
					m_CurrentSel = pProfiles->AddString(DisplayName);
					pProfiles->SetItemData(m_CurrentSel, (DWORD_PTR)m_pCurrentSettings);
					// select the new item in the combo
					pProfiles->SetCurSel(m_CurrentSel);
					// update the default profile
					m_pSettingsManager->SetDefaultProfile(NewName.c_str());
				}
				else
				{
					// revert to the old name
					DisplayName = CurrentName.GetBuffer() + PROFILE_PREFIX_LENGTH;
					pProfiles->SetWindowText(DisplayName);
				}
			}
		}
	}

	void WindowerConfigDlg::RefreshPluginList()
	{
		if (m_pCurrentSettings != NULL)
		{
			CListCtrl *pPluginList = static_cast<CListCtrl*>(GetDlgItem(IDC_PLUGIN_LIST));

			for (int Index = 0; Index < pPluginList->GetItemCount(); ++Index)
			{
				DWORD_PTR ItemData = pPluginList->GetItemData(Index);

				if (ItemData != -1 && ItemData != NULL)
				{
					PluginInfo *pPluginInfo = reinterpret_cast<PluginInfo*>(ItemData);
					const string_t &PluginName = pPluginInfo->GetName();

					pPluginList->SetCheck(Index, m_pCurrentSettings->IsPluginActive(PluginName));
				}
			}
		}
	}

	void WindowerConfigDlg::OnLanguageChange()
	{
		CComboBox *pLng = static_cast<CComboBox*>(GetDlgItem(IDC_LNG_COMBO));

		if (m_pCurrentSettings != NULL && pLng != NULL)
			m_pCurrentSettings->SetLanguage(pLng->GetCurSel());
	}

	void WindowerConfigDlg::OnProfilesChange()
	{
		CComboBox *pProfiles = static_cast<CComboBox*>(GetDlgItem(IDC_PROFILES_COMBO));
		CComboBox *pLng		 = static_cast<CComboBox*>(GetDlgItem(IDC_LNG_COMBO));
		CEdit *pGamePath	 = static_cast<CEdit*>(GetDlgItem(IDC_GAME_PATH));
		CButton *pVSync		 = static_cast<CButton*>(GetDlgItem(IDC_VSYNC));

		if (m_pCurrentSettings != NULL)
			UpdateActivePlugins();

		m_pCurrentSettings = NULL;

		if (pProfiles != NULL && pVSync != NULL && pGamePath != NULL)
		{
			m_CurrentSel = pProfiles->GetCurSel();

			if (m_CurrentSel >= 0)
			{
				m_pCurrentSettings = reinterpret_cast<WindowerProfile*>(pProfiles->GetItemData(m_CurrentSel));

				if (m_pCurrentSettings != NULL)
				{
					long Lng = m_pCurrentSettings->GetLanguage();

					// validate the value
					if (Lng < 0L || Lng > 4L)
						Lng = 1L;

					m_pSettingsManager->SetDefaultProfile(m_pCurrentSettings->GetName());

					pVSync->SetCheck(m_pCurrentSettings->GetVSync() ? BST_CHECKED : BST_UNCHECKED);
					pGamePath->SetWindowText(m_pSettingsManager->GetGamePath());
					pLng->SetCurSel(Lng);

					RefreshPluginList();
				}
			}
		}
	}

	void WindowerConfigDlg::OnLaunch()
	{
		OnSave();

		PROCESS_INFORMATION ProcessInfo;
		TCHAR DLL32Path[_MAX_PATH];
		TCHAR ExePath[_MAX_PATH];

		_stprintf_s(DLL32Path, _MAX_PATH, _T("%s\\bootstrap.dll"), m_pSettingsManager->GetWorkingDir().c_str());
		_stprintf_s(ExePath, _MAX_PATH, _T("%sboot\\ffxivboot.exe"), m_pSettingsManager->GetGamePath());

		InjectModule::CreateProcessEx(ExePath, ProcessInfo, NULL, CREATE_DEFAULT_ERROR_MODE |
									  CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT, DLL32Path);

		OnOK();
	}

	void WindowerConfigDlg::OnSave()
	{
		UpdateActivePlugins();

		if (m_pSettingsManager != NULL)
			m_pSettingsManager->Save();
	}

	void WindowerConfigDlg::GenerateNewName(CString &NewName_in_out)
	{
		int Number = 0, ScanPos = 0, Length = NewName_in_out.GetLength();

		NewName_in_out.Tokenize(_T("0123456789"), ScanPos);
		Number = _ttoi(NewName_in_out.Right(Length - ScanPos + 1));
		NewName_in_out.Format(_T("%s%i"), NewName_in_out.Left(ScanPos - 1), ++Number);
	}

	void WindowerConfigDlg::OnNewProfile()
	{
		CComboBox *pProfiles = static_cast<CComboBox*>(GetDlgItem(IDC_PROFILES_COMBO));

		if (pProfiles != NULL)
		{
			TCHAR *pDisplayName = _T("New Profile");
			WindowerProfile *pNewSettings = NULL;
			CString NewName;

			// generate the new internal name
			NewName.Format(_T("%s%s"), PROFILE_PREFIX, pDisplayName);

			while(pProfiles->FindString(0, pDisplayName) != CB_ERR)
			{
				GenerateNewName(NewName);
				pDisplayName = NewName.GetBuffer() + PROFILE_PREFIX_LENGTH;
			}

			if (m_pCurrentSettings != NULL)
			{
				UpdateActivePlugins();
				pNewSettings = m_pSettingsManager->DuplicateProfile(NewName, *m_pCurrentSettings);
			}

			if (pNewSettings != NULL)
			{
				m_pSettingsManager->SetDefaultProfile(NewName);
				m_CurrentSel = pProfiles->AddString(pDisplayName);
				m_pCurrentSettings = pNewSettings;

				pProfiles->SetItemData(m_CurrentSel, (DWORD_PTR)pNewSettings);
				pProfiles->SetCurSel(m_CurrentSel);
				pProfiles->SetEditSel(-1, -1);
				pProfiles->SetFocus();

				ActivateAllPlugins();
			}

			GetDlgItem(IDC_DELETE_PROFILE)->EnableWindow(pProfiles->GetCount() > 1);
		}	
	}

	void WindowerConfigDlg::OnDeleteProfile()
	{
		CComboBox *pProfiles = static_cast<CComboBox*>(GetDlgItem(IDC_PROFILES_COMBO));
		bool Enable = (pProfiles->GetCount() > 1);
			
		if (Enable)
		{
			m_pSettingsManager->DeleteProfile(m_pCurrentSettings->GetName());
			pProfiles->DeleteString(m_CurrentSel);
			m_pCurrentSettings = NULL;
			// set the new selection
			pProfiles->SetCurSel((m_CurrentSel == 0) ? 0 : --m_CurrentSel);
			// update the data
			OnProfilesChange();
			RefreshPluginList();
		}

		GetDlgItem(IDC_DELETE_PROFILE)->EnableWindow(pProfiles->GetCount() > 1);
	}

	void WindowerConfigDlg::OnVSyncChange()
	{
		CButton *pVSync = static_cast<CButton*>(GetDlgItem(IDC_VSYNC));

		if (m_pSettingsManager != NULL && pVSync != NULL)
		{
			m_pCurrentSettings->SetVSync(pVSync->GetCheck() == BST_CHECKED);
		}
	}

	void WindowerConfigDlg::ActivateAllPlugins()
	{
		if (m_pCurrentSettings != NULL)
		{
			bool Configure = (MessageBox(_T("Would you like to configure the plugins?"),
										 _T("Windower configuration"), MB_YESNO | MB_ICONQUESTION) == IDYES);

			CListCtrl *pPluginList = static_cast<CListCtrl*>(GetDlgItem(IDC_PLUGIN_LIST));

			for (int Index = 0; Index < pPluginList->GetItemCount(); ++Index)
			{
				DWORD_PTR ItemData = pPluginList->GetItemData(Index);

				if (ItemData != -1 && ItemData != NULL)
				{
					PluginInfo *pPluginInfo = reinterpret_cast<PluginInfo*>(ItemData);
					const string_t &PluginName = pPluginInfo->GetName();

					if (Configure && m_pPluginManager->LoadPlugin(PluginName))
					{
						if (m_pPluginManager->IsPluginConfigurable(PluginName)
						 && m_pPluginManager->ConfigurePlugin(PluginName, (LPVOID)m_pCurrentSettings->GetName()))
						{
							m_pSettingsManager->Reload();
						}

						m_pPluginManager->UnloadPlugin(PluginName);
					}

					m_pCurrentSettings->ActivatePlugin(PluginName);
				}
			}

			RefreshPluginList();
		}
	}

	void WindowerConfigDlg::UpdateActivePlugins()
	{
		if (m_pCurrentSettings)
		{
			CListCtrl *pPluginList = static_cast<CListCtrl*>(GetDlgItem(IDC_PLUGIN_LIST));

			for (int Index = 0; Index < pPluginList->GetItemCount(); ++Index)
			{
				DWORD_PTR ItemData = pPluginList->GetItemData(Index);

				if (ItemData != -1 && ItemData != NULL)
				{
					PluginInfo *pPluginInfo = reinterpret_cast<PluginInfo*>(ItemData);
					bool Active = (pPluginList->GetCheck(Index) == BST_CHECKED);
					const string_t &PluginName = pPluginInfo->GetName();					

					m_pCurrentSettings->ActivatePlugin(PluginName, Active);
				}
			}
		}
	}

	void WindowerConfigDlg::OnPathBrowse()
	{
		CEdit *pGamePath = static_cast<CEdit*>(GetDlgItem(IDC_GAME_PATH));
		bool ValidPath = false;
		string_t GamePath;

		if (m_pSettingsManager->SelectDirectory(GamePath) == false)
		{
			TCHAR CurrentDirectory[_MAX_PATH];

			GetCurrentDirectory(_MAX_PATH, CurrentDirectory);
			GamePath = CurrentDirectory;

			if (GamePath.back() != '\\')
				GamePath += '\\';
		}

		if (pGamePath != NULL)
			pGamePath->SetWindowText(GamePath.c_str());
	}

	void WindowerConfigDlg::OnPathChange()
	{
		CEdit *pGamePath = static_cast<CEdit*>(GetDlgItem(IDC_GAME_PATH));
		CButton *pLaunch = static_cast<CButton*>(GetDlgItem(IDOK));

		if (pLaunch != NULL && pGamePath != NULL)
		{
			CString Path;
			bool Valid;

			pGamePath->GetWindowText(Path);
			// check the game path
			Valid = m_pSettingsManager->CheckGamePath(Path.GetBuffer());
			pLaunch->EnableWindow(Valid);

			if (Valid)
				m_pSettingsManager->SetGamePath(Path);
			else
				MessageBox(_T("The specified game directory is incorrect ('boot'/'game' missing)."), _T("Error!"), MB_OK | MB_ICONSTOP);
		}
	}
}