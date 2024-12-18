/**************************************************************************
	created		:	2011-10-29
	filename	: 	ConfigDlg.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Main dialog
**************************************************************************/
#include "stdafx.h"
#include "resource.h"

#include "LauncherApp.h"
#include "EditLinkTarget.h"
#include "ConfigDlg.h"
#include "LabelDlg.h"

BEGIN_MESSAGE_MAP(ConfigDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_CBN_SELCHANGE(IDC_PROFILES_COMBO, &ConfigDlg::OnProfilesChange)
	ON_CBN_KILLFOCUS(IDC_PROFILES_COMBO, &ConfigDlg::OnProfileRename)

	ON_BN_CLICKED(IDC_DELETE_PROFILE, &ConfigDlg::OnDeleteProfile)
	ON_BN_CLICKED(IDC_BROWSE_GAME_PATH, &ConfigDlg::OnPathBrowse)
	ON_BN_CLICKED(IDC_NEW_PROFILE, &ConfigDlg::OnNewProfile)
	ON_BN_CLICKED(IDC_CREATE_LINK, &ConfigDlg::OnCreateLink)

	ON_BN_CLICKED(IDC_AUTOUPDATE, &ConfigDlg::OnAutoUpdateChange)
	ON_BN_CLICKED(IDC_VSYNC, &ConfigDlg::OnVSyncChange)

	ON_EN_CHANGE(IDC_GAME_PATH, &ConfigDlg::OnPathChange)

	ON_NOTIFY(NM_DBLCLK, IDC_PLUGIN_LIST, OnConfigure)

	ON_BN_CLICKED(IDC_SAVE, &ConfigDlg::OnSave)
	ON_BN_CLICKED(IDC_TEXT_LABELS, &ConfigDlg::OnBnClickedTextLabels)
END_MESSAGE_MAP()

/*! \brief (MFC) Called by the framework to exchange and validate dialog data.
\param[in] pDX_in : a pointer to a CDataExchange object.
*/
void ConfigDlg::DoDataExchange(CDataExchange *pDX_in)
{
	CDialogEx::DoDataExchange(pDX_in);

	DDX_Control(pDX_in, IDC_CREATE_LINK, m_LinkButton);
	DDX_Control(pDX_in, IDC_GAME_PATH, m_GamePath);
}

ConfigDlg::ConfigDlg(PluginFramework::PluginManager &PluginManager_in,
					 Windower::SettingsManager &SettingsManager_in, CWnd* pParent_in)
	: CDialogEx(IDD, pParent_in), m_PluginManager(PluginManager_in), m_pCurrentSettings(nullptr),
	  m_SettingsManager(SettingsManager_in), m_CurrentSel(0), m_hIcon(nullptr) {}

void ConfigDlg::InsertPlugin(CListCtrl *pListCtrl_in_out, const PluginFramework::PluginInfo &PluginInfo_in) const
{
	if (pListCtrl_in_out == nullptr)
		return;

	LVITEM LvItem;
	string_t Text;

	LvItem.mask = LVIF_TEXT | LVIF_PARAM;
	LvItem.lParam = reinterpret_cast<LPARAM>(&PluginInfo_in);
	LvItem.iItem = 0;
		
	// plugin name
	LvItem.iSubItem = LIST_COL_NAME;
	LvItem.pszText = const_cast<LPTSTR>(PluginInfo_in.GetName().c_str());

	int Index = pListCtrl_in_out->InsertItem(&LvItem);

	LvItem.iItem = Index;
	LvItem.mask = LVIF_TEXT;

	// plugin version
	LvItem.iSubItem = LIST_COL_VERSION;
	Text = PluginInfo_in.GetVersion();
	LvItem.pszText = const_cast<LPTSTR>(Text.c_str());
	pListCtrl_in_out->SetItem(&LvItem);
	// plugin version
	LvItem.iSubItem = LIST_COL_FRAMEWORK;
	Text = PluginInfo_in.GetFrameworkVersion();
	LvItem.pszText = const_cast<LPTSTR>(Text.c_str());
	pListCtrl_in_out->SetItem(&LvItem);
	// plugin author
	LvItem.iSubItem = LIST_COL_AUTHOR;
	LvItem.pszText = const_cast<LPTSTR>(PluginInfo_in.GetAuthor().c_str());
	pListCtrl_in_out->SetItem(&LvItem);
	// plugin compatibility
	LvItem.iSubItem = LIST_COL_COMPAT;

	DWORD flags = (PluginInfo_in.GetCompatibilityFlags() & 0x0FFFFFFF);

	switch (flags)
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
	default:
		Text = _T("None");
	}
	LvItem.pszText = const_cast<LPTSTR>(Text.c_str());
	pListCtrl_in_out->SetItem(&LvItem);
	// plugin description
	LvItem.iSubItem = LIST_COL_DESC;
	LvItem.pszText = const_cast<LPTSTR>(PluginInfo_in.GetDesc().c_str());
	pListCtrl_in_out->SetItem(&LvItem);
	// plugin path
	LvItem.iSubItem = LIST_COL_PATH;
	LvItem.pszText = const_cast<LPTSTR>(PluginInfo_in.GetDLLPath().c_str());
	pListCtrl_in_out->SetItem(&LvItem);

	if (m_pCurrentSettings != nullptr)
		pListCtrl_in_out->SetCheck(Index, m_pCurrentSettings->IsPluginActive(PluginInfo_in.GetName()));
}

BOOL ConfigDlg::OnInitDialog()
{
	CComboBox *pProfiles = static_cast<CComboBox*>(GetDlgItem(IDC_PROFILES_COMBO));
	CListCtrl *pPluginList = static_cast<CListCtrl*>(GetDlgItem(IDC_PLUGIN_LIST));
	string_t GamePath = m_SettingsManager.GetGamePath();

	// set the dialog icon
	m_hIcon = AfxGetApp()->LoadIcon(IDI_CONFIG);

	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if (VisualDialog::CreateImageList(m_ImageList, IDL_CONFIG, ICON_COUNT, CSize(24, 24), ILC_COLOR32))
	{
		m_LinkButton.SetImageList(&m_ImageList);
		m_LinkButton.SetImage(ICON_CREATE_LINK);
		m_LinkButton.SetTooltip(_T("Create a shortcut to this profile on the desktop"));
	}

	m_GamePath.SetWindowText(m_SettingsManager.GetGamePath());
	m_GamePath.SetCallback(static_cast<fnOnDrop>(&ConfigDlg::OnTargetDrop), this);
	m_GamePath.SetTooltip(_T("Select the root directory of the game or drag && drop a shortcut here"));

	pPluginList->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES);

	pPluginList->InsertColumn(LIST_COL_NAME,		_T("Plugin name"),	 LVCFMT_LEFT, 100);
	pPluginList->InsertColumn(LIST_COL_VERSION,		_T("Version"),		 LVCFMT_LEFT,  50);
	pPluginList->InsertColumn(LIST_COL_FRAMEWORK,	_T("Framework"),	 LVCFMT_LEFT,  70);
	pPluginList->InsertColumn(LIST_COL_AUTHOR,		_T("Author"),		 LVCFMT_LEFT,  80);
	pPluginList->InsertColumn(LIST_COL_COMPAT,		_T("Compatibility"), LVCFMT_LEFT,  80);
	pPluginList->InsertColumn(LIST_COL_DESC,		_T("Description"),	 LVCFMT_LEFT, 400);
	pPluginList->InsertColumn(LIST_COL_PATH,		_T("Filename"),		 LVCFMT_LEFT, 300);

	m_GamePath.SetWindowText(GamePath.c_str());

	if (pProfiles != nullptr)
	{
		const Windower::WindowerSettings& Profiles = m_SettingsManager.GetProfiles();
		Windower::WindowerSettings::const_iterator SettingIt, EndIt = Profiles.cend();
		const TCHAR *pDefault = m_SettingsManager.GetDefaultProfile();			
		Windower::WindowerProfile *pProfile;
		const TCHAR *pName;
		int ItemCount = 0;

		for (SettingIt = Profiles.cbegin(); SettingIt != EndIt; ++SettingIt)
		{
			pProfile = SettingIt->second;

			if (pProfile != nullptr)
			{
				pName = pProfile->GetName();

				ItemCount = pProfiles->AddString(pName + _tcslen(PROFILE_PREFIX));
				pProfiles->SetItemData(ItemCount, reinterpret_cast<DWORD_PTR>(SettingIt->second));

				if (_tcscmp(pName, pDefault) == 0)
					pProfiles->SetCurSel(ItemCount);

				++ItemCount;
			}
		}

		OnProfilesChange();

		GetDlgItem(IDC_DELETE_PROFILE)->EnableWindow(ItemCount > 1);
	}

	const PluginFramework::RegisteredPlugins& Plugins = m_PluginManager.GetRegisteredPlugins();
	PluginFramework::RegisteredPlugins::const_iterator PluginIt, EndIt = Plugins.cend();

	for (PluginIt = Plugins.cbegin(); PluginIt != EndIt; ++PluginIt)
		InsertPlugin(pPluginList, PluginIt->second);

	// force the dialog to use a fixed DPI resolution
	VisualDialog::SetFixedDPI(m_hWnd, IDD, DLG_FIXED_DPI);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ConfigDlg::OnConfigure(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (m_pCurrentSettings != nullptr)
	{
		CListCtrl *pPluginList = static_cast<CListCtrl*>(GetDlgItem(IDC_PLUGIN_LIST));

		for (int Index = 0; Index < pPluginList->GetItemCount(); ++Index)
		{
			if (pPluginList->GetItemState(Index, LVIS_SELECTED) == LVIS_SELECTED)
			{
				DWORD_PTR ItemData = pPluginList->GetItemData(Index);

				if (ItemData != -1 && ItemData != NULL)
				{
					PluginFramework::PluginInfo *pPluginInfo = reinterpret_cast<PluginFramework::PluginInfo*>(ItemData);
					const string_t &PluginName = pPluginInfo->GetName();
					bool Result;
					
					Result = m_PluginManager.ConfigurePlugin(PluginName, nullptr);
					m_PluginManager.UnloadPlugin(PluginName);

					if (Result)
						m_SettingsManager.Reload();

					break;
				}
			}
		}
	}

	*pResult = 0;
}

void ConfigDlg::OnProfileRename()
{
	CComboBox *pProfiles = static_cast<CComboBox*>(GetDlgItem(IDC_PROFILES_COMBO));

	if (m_pCurrentSettings != nullptr && pProfiles != nullptr)
	{
		CString DisplayName, CurrentName;
		string_t NewName;

		pProfiles->GetWindowText(DisplayName);
		CurrentName = m_pCurrentSettings->GetName();
		format(NewName, _T("%s%s"), PROFILE_PREFIX, DisplayName);			

		// update the profile name
		if (CurrentName.Compare(NewName.c_str()) != 0)
		{
			if (m_SettingsManager.RenameProfile(m_pCurrentSettings, NewName))
			{
				// delete the current combo item
				pProfiles->DeleteString(m_CurrentSel);
				// add the item back (update the display name as RenameProfile 
				// could have changed it in case the provided was a duplicate)
				DisplayName = NewName.c_str() + _tcslen(PROFILE_PREFIX);
				m_CurrentSel = pProfiles->AddString(DisplayName);
				pProfiles->SetItemData(m_CurrentSel, reinterpret_cast<DWORD_PTR>(m_pCurrentSettings));
				// select the new item in the combo
				pProfiles->SetCurSel(m_CurrentSel);
				// update the default profile
				m_SettingsManager.SetDefaultProfile(NewName.c_str());
			}
			else
			{
				// revert to the old name
				DisplayName = CurrentName.GetBuffer() + _tcslen(PROFILE_PREFIX);
				pProfiles->SetWindowText(DisplayName);
			}
		}
	}
}

void ConfigDlg::RefreshPluginList() const
{
	if (m_pCurrentSettings != nullptr)
	{
		CListCtrl *pPluginList = static_cast<CListCtrl*>(GetDlgItem(IDC_PLUGIN_LIST));

		for (int Index = 0; Index < pPluginList->GetItemCount(); ++Index)
		{
			DWORD_PTR ItemData = pPluginList->GetItemData(Index);

			if (ItemData != -1 && ItemData != NULL)
			{
				PluginFramework::PluginInfo *pPluginInfo = reinterpret_cast<PluginFramework::PluginInfo*>(ItemData);
				const string_t &PluginName = pPluginInfo->GetName();

				pPluginList->SetCheck(Index, m_pCurrentSettings->IsPluginActive(PluginName));
			}
		}
	}
}

void ConfigDlg::OnProfilesChange()
{
	CComboBox *pProfiles = static_cast<CComboBox*>(GetDlgItem(IDC_PROFILES_COMBO));
	CButton *pTextLabels = static_cast<CButton*>(GetDlgItem(IDC_TEXT_LABELS));
	CButton *pAutoUpdate = static_cast<CButton*>(GetDlgItem(IDC_AUTOUPDATE));
	CButton *pVSync		 = static_cast<CButton*>(GetDlgItem(IDC_VSYNC));
	

	if (m_pCurrentSettings != nullptr)
		UpdateActivePlugins();

	m_pCurrentSettings = nullptr;

	if (pProfiles != nullptr && pVSync != nullptr)
	{
		m_CurrentSel = pProfiles->GetCurSel();

		if (m_CurrentSel >= 0)
		{
			m_pCurrentSettings = reinterpret_cast<Windower::WindowerProfile*>(pProfiles->GetItemData(m_CurrentSel));

			if (m_pCurrentSettings != nullptr)
			{
				m_SettingsManager.SetDefaultProfile(m_pCurrentSettings->GetName());

				pAutoUpdate->SetCheck(m_SettingsManager.IsAutoUpdated() ? BST_CHECKED : BST_UNCHECKED);
				pVSync->SetCheck(m_pCurrentSettings->GetVSync() ? BST_CHECKED : BST_UNCHECKED);

				RefreshPluginList();
			}
		}
	}

	pTextLabels->EnableWindow(m_pCurrentSettings != nullptr);
}

void ConfigDlg::OnOK()
{
	// save the configuration
	OnSave();

	CDialogEx::OnOK();
}

void ConfigDlg::OnCancel()
{
	EndDialog(IDCANCEL);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ConfigDlg::OnSave()
{
	UpdateActivePlugins();
	m_SettingsManager.Save();
}

void ConfigDlg::GenerateNewName(CString &NewName_in_out)
{
	int Number, ScanPos = 0, Length = NewName_in_out.GetLength();

	NewName_in_out.Tokenize(_T("0123456789"), ScanPos);
	Number = _ttoi(NewName_in_out.Right(Length - ScanPos + 1));
	NewName_in_out.Format(_T("%s%i"), NewName_in_out.Left(ScanPos - 1).GetBuffer(), ++Number);
}

void ConfigDlg::OnNewProfile()
{
	CComboBox *pProfiles = static_cast<CComboBox*>(GetDlgItem(IDC_PROFILES_COMBO));

	if (pProfiles != nullptr)
	{
		Windower::WindowerProfile *pNewSettings = nullptr;
		TCHAR *pDisplayName = _T("New Profile");		
		CString NewName;

		// generate the new internal name
		NewName.Format(_T("%s%s"), PROFILE_PREFIX, pDisplayName);

		while(pProfiles->FindString(0, pDisplayName) != CB_ERR)
		{
			GenerateNewName(NewName);
			pDisplayName = NewName.GetBuffer() + _tcslen(PROFILE_PREFIX);
		}

		if (m_pCurrentSettings != nullptr)
		{
			UpdateActivePlugins();
			pNewSettings = m_SettingsManager.DuplicateProfile(NewName, *m_pCurrentSettings);
		}

		if (pNewSettings != nullptr)
		{
			m_SettingsManager.SetDefaultProfile(NewName);
			m_CurrentSel = pProfiles->AddString(pDisplayName);
			m_pCurrentSettings = pNewSettings;

			pProfiles->SetItemData(m_CurrentSel, reinterpret_cast<DWORD_PTR>(pNewSettings));
			pProfiles->SetCurSel(m_CurrentSel);
			pProfiles->SetEditSel(-1, -1);
			pProfiles->SetFocus();

			ActivateAllPlugins();
		}

		GetDlgItem(IDC_DELETE_PROFILE)->EnableWindow(pProfiles->GetCount() > 1);
	}	
}

void ConfigDlg::OnDeleteProfile()
{
	CComboBox *pProfiles = static_cast<CComboBox*>(GetDlgItem(IDC_PROFILES_COMBO));
	bool Enable = (pProfiles->GetCount() > 1);
			
	if (Enable)
	{
		m_SettingsManager.DeleteProfile(m_pCurrentSettings->GetName());
		pProfiles->DeleteString(m_CurrentSel);
		m_pCurrentSettings = nullptr;
		// set the new selection
		pProfiles->SetCurSel((m_CurrentSel == 0) ? 0 : --m_CurrentSel);
		// update the data
		OnProfilesChange();
		RefreshPluginList();
	}

	GetDlgItem(IDC_DELETE_PROFILE)->EnableWindow(pProfiles->GetCount() > 1);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ConfigDlg::OnVSyncChange()
{
	CButton *pVSync = static_cast<CButton*>(GetDlgItem(IDC_VSYNC));

	if (pVSync != nullptr)
	{
		m_pCurrentSettings->SetVSync(pVSync->GetCheck() == BST_CHECKED);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ConfigDlg::OnAutoUpdateChange()
{
	CButton *pAutoUpdate = static_cast<CButton*>(GetDlgItem(IDC_AUTOUPDATE));

	if (pAutoUpdate != nullptr)
	{
		m_SettingsManager.SetAutoUpdate(pAutoUpdate->GetCheck() == BST_CHECKED);
	}
}

void ConfigDlg::ActivateAllPlugins()
{
	if (m_pCurrentSettings != nullptr)
	{
		bool Configure = (MessageBox(_T("Would you like to configure the plugins?"),
										_T("x14-2 configuration"), MB_YESNO | MB_ICONQUESTION) == IDYES);

		CListCtrl *pPluginList = static_cast<CListCtrl*>(GetDlgItem(IDC_PLUGIN_LIST));

		for (int Index = 0; Index < pPluginList->GetItemCount(); ++Index)
		{
			DWORD_PTR ItemData = pPluginList->GetItemData(Index);

			if (ItemData != -1 && ItemData != NULL)
			{
				PluginFramework::PluginInfo *pPluginInfo = reinterpret_cast<PluginFramework::PluginInfo*>(ItemData);
				const string_t &PluginName = pPluginInfo->GetName();

				if (Configure && m_PluginManager.LoadPlugin(PluginName))
				{
					if (m_PluginManager.IsPluginConfigurable(PluginName)
						&& m_PluginManager.ConfigurePlugin(PluginName, LPVOID(m_pCurrentSettings->GetName())))
					{
						m_SettingsManager.Reload();
					}

					m_PluginManager.UnloadPlugin(PluginName);
				}

				m_pCurrentSettings->UpdatePluginList(PluginName);
			}
		}

		RefreshPluginList();
	}
}

void ConfigDlg::UpdateActivePlugins() const
{
	if (m_pCurrentSettings)
	{
		CListCtrl *pPluginList = static_cast<CListCtrl*>(GetDlgItem(IDC_PLUGIN_LIST));

		for (int Index = 0; Index < pPluginList->GetItemCount(); ++Index)
		{
			DWORD_PTR ItemData = pPluginList->GetItemData(Index);

			if (ItemData != -1 && ItemData != NULL)
			{
				PluginFramework::PluginInfo *pPluginInfo = reinterpret_cast<PluginFramework::PluginInfo*>(ItemData);
				bool Active = (pPluginList->GetCheck(Index) == BST_CHECKED);
				const string_t &PluginName = pPluginInfo->GetName();					

				m_pCurrentSettings->UpdatePluginList(PluginName, Active);
			}
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ConfigDlg::OnCreateLink()
{
	CComboBox *pProfiles = static_cast<CComboBox*>(GetDlgItem(IDC_PROFILES_COMBO));

	if (pProfiles != nullptr)
	{
		CString ProfileName;

		pProfiles->GetWindowText(ProfileName);

		if (ProfileName.IsEmpty() == false)
		{
			TCHAR LauncherExe[MAX_PATH] = { '\0' };
			
			if (GetModuleFileName(nullptr, LauncherExe, MAX_PATH))
			{
				string_t LinkArgs, Description, Profile(ProfileName.GetBuffer());
				TCHAR DesktopPath[MAX_PATH] = { '\0' };

				format(Description, _T("Start Final Fantasy XIV : A Realm Reborn with the '%s' profile"), ProfileName);
				Profile = purge<wchar_t>(Profile, _ALPHA | _SPACE | _DIGIT, nullptr, true);
				format(LinkArgs, _T("/Profile %s"), ProfileName);

				if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_DESKTOPDIRECTORY, nullptr, SHGFP_TYPE_CURRENT, DesktopPath)))
				{
					string_t LinkLocation;

					format(LinkLocation, _T("%s\\%s's x14-2 profile.lnk"), DesktopPath, Profile.c_str());

					LauncherApp::CreateLink(LinkLocation.c_str(), LauncherExe,
											m_SettingsManager.GetWorkingDir(),
											LinkArgs.c_str(), Description.c_str(), 1);
				}
			}			
		}
	}
}

void ConfigDlg::OnPathBrowse()
{
	string_t GamePath;

	if (m_SettingsManager.SelectDirectory(GamePath))
	{
		if (m_SettingsManager.CheckGamePath(GamePath))
			m_GamePath.SetWindowText(GamePath.c_str());
		else
			MessageBox(_T("The specified game directory is incorrect ('boot'/'game' missing)."), _T("Error!"), MB_OK | MB_ICONSTOP);
	}
}

void ConfigDlg::OnPathChange()
{
	CButton *pLaunch = static_cast<CButton*>(GetDlgItem(IDOK));
	static bool skip = false;

	if (pLaunch != nullptr)
	{
		CString Path;
		bool Valid;

		m_GamePath.GetWindowText(Path);
		// check the game path
		Valid = m_SettingsManager.CheckGamePath(Path.GetBuffer());
		pLaunch->EnableWindow(Valid);

		if (Valid)
			m_SettingsManager.SetGamePath(Path);
		else if (skip == false)
			skip = true;
		else
			MessageBox(_T("The specified game directory is incorrect ('boot'/'game' missing)."), _T("Error!"), MB_OK | MB_ICONSTOP);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
bool ConfigDlg::OnTargetDrop(TCHAR **pDropPath_in)
{
	if (pDropPath_in != nullptr && *pDropPath_in != nullptr)
	{
		string_t::size_type Pos;
		string_t Path;

		// initialize the path
		Pos = initialize_path(*pDropPath_in, Path, false) - 1;

		while (m_SettingsManager.CheckGamePath(Path) == false && Pos != string_t::npos)
		{
			Pos = Path.rfind('\\', Pos - 1);
			Path = Path.substr(0, Pos + 1);
		}

		if (Pos != string_t::npos)
		{
			size_t CopyCount = Path.size() + 1;

			if (Buffer::Realloc(pDropPath_in, CopyCount))
			{
				_tcscpy_s(*pDropPath_in, CopyCount, Path.c_str());

				return true;
			}
		}
	}

	return false;
}

void ConfigDlg::OnBnClickedTextLabels()
{
	if (m_pCurrentSettings != nullptr)
		LabelDlg(m_SettingsManager, *m_pCurrentSettings, this).DoModal();
}