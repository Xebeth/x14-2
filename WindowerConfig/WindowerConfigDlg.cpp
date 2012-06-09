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
		ON_CBN_SELCHANGE(IDC_RESX_COMBO, &WindowerConfigDlg::OnResolutionChange)
		ON_CBN_SELCHANGE(IDC_RESY_COMBO, &WindowerConfigDlg::OnResolutionChange)

		ON_CBN_KILLFOCUS(IDC_PROFILES_COMBO, &WindowerConfigDlg::OnProfileNameChange)
		ON_CBN_EDITCHANGE(IDC_RESX_COMBO, &WindowerConfigDlg::OnResolutionChange)
		ON_CBN_EDITCHANGE(IDC_RESY_COMBO, &WindowerConfigDlg::OnResolutionChange)		

		ON_BN_CLICKED(IDC_DELETE_PROFILE, &WindowerConfigDlg::OnDeleteProfile)
		ON_BN_CLICKED(IDC_NEW_PROFILE, &WindowerConfigDlg::OnNewProfile)
		ON_BN_CLICKED(IDC_VSYNC, &WindowerConfigDlg::OnVSyncChange)

		ON_NOTIFY(NM_DBLCLK, IDC_PLUGIN_LIST, OnConfigure)
		
		ON_BN_CLICKED(IDOK, &WindowerConfigDlg::OnSave)
	END_MESSAGE_MAP()

	WindowerConfigDlg::WindowerConfigDlg(SettingsManager *pSettingsManager,CWnd* pParent)
		: CDialog(WindowerConfigDlg::IDD, pParent), m_pSettingsManager(pSettingsManager)
	{
		m_pServices = new DummyServices(__PLUGIN_FRAMEWORK_VERSION__);
		m_pPluginManager = new PluginManager(m_pServices);

		// m_pPluginManager->BlacklistPlugin(_T("745E1230-0C81-4220-B099-3A3392EFA03A"));

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
			FillSupportedRes();

			GetDlgItem(IDC_DELETE_PROFILE)->EnableWindow(ItemCount > 1);
		}

		if (m_pPluginManager != NULL && m_pPluginManager->ListPlugins(m_pSettingsManager->GetPluginsAbsoluteDir()) > 0U)
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
					
					m_pPluginManager->ConfigurePlugin(PluginName, (LPVOID)m_pCurrentSettings->GetName());
					m_pPluginManager->UnloadPlugin(PluginName);

					break;
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

	void WindowerConfigDlg::OnProfileNameChange()
	{
		CComboBox *pProfiles = static_cast<CComboBox*>(GetDlgItem(IDC_PROFILES_COMBO));

		if (m_pCurrentSettings != NULL && pProfiles != NULL)
		{
			CString DisplayName, ProfileName;

			pProfiles->GetWindowText(DisplayName);
			ProfileName.Format(_T("%s%s"), PROFILE_PREFIX, DisplayName);

			if (ProfileName.Compare(m_pCurrentSettings->GetName()) != 0)
			{
				// delete the current combo item
				pProfiles->DeleteString(m_CurrentSel);
				// add the item back
				m_CurrentSel = pProfiles->AddString(DisplayName);
				pProfiles->SetItemData(m_CurrentSel, (DWORD_PTR)m_pCurrentSettings);
				// select the new item in the combo
				pProfiles->SetCurSel(m_CurrentSel);
				// update the profile name
				m_pCurrentSettings->SetName(ProfileName);
			}
		}
	}

	void WindowerConfigDlg::OnResolutionChange()
	{
		CComboBox *pResX	 = static_cast<CComboBox*>(GetDlgItem(IDC_RESX_COMBO));
		CComboBox *pResY	 = static_cast<CComboBox*>(GetDlgItem(IDC_RESY_COMBO));

		if (m_pCurrentSettings != NULL && pResX != NULL && pResY != NULL)
		{
			CString CurrentValue;
			int SelectedIndex;
			LONG ResX, ResY;

			SelectedIndex = pResX->GetCurSel();
			if (SelectedIndex == -1)
				pResX->GetWindowText(CurrentValue);
			else
				pResX->GetLBText(SelectedIndex, CurrentValue);
			ResX = _ttol(CurrentValue);
			SelectedIndex = pResY->GetCurSel();
			if (SelectedIndex == -1)
				pResY->GetWindowText(CurrentValue);
			else
				pResY->GetLBText(SelectedIndex, CurrentValue);
			ResY = _ttol(CurrentValue);

			m_pCurrentSettings->SetResolution(ResX, ResY);
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

	void WindowerConfigDlg::OnProfilesChange()
	{
		CComboBox *pProfiles = static_cast<CComboBox*>(GetDlgItem(IDC_PROFILES_COMBO));
		CComboBox *pResX	 = static_cast<CComboBox*>(GetDlgItem(IDC_RESX_COMBO));
		CComboBox *pResY	 = static_cast<CComboBox*>(GetDlgItem(IDC_RESY_COMBO));
		CButton *pVSync		 = static_cast<CButton*>(GetDlgItem(IDC_VSYNC));

		if (m_pCurrentSettings != NULL)
			UpdateActivePlugins();

		m_pCurrentSettings = NULL;

		if (pProfiles != NULL && pResX != NULL && pResY != NULL && pVSync != NULL)
		{
			m_CurrentSel = pProfiles->GetCurSel();

			if (m_CurrentSel >= 0)
			{
				m_pCurrentSettings = reinterpret_cast<WindowerProfile*>(pProfiles->GetItemData(m_CurrentSel));

				if (m_pCurrentSettings != NULL)
				{
					TCHAR StrX[8], StrY[8];

					m_pSettingsManager->SetDefaultProfile(m_pCurrentSettings->GetName());

					_ltot_s(m_pCurrentSettings->GetResX(), StrX, 10);
					_ltot_s(m_pCurrentSettings->GetResY(), StrY, 10);

					pResX->SetWindowText(StrX);
					pResY->SetWindowText(StrY);

					pVSync->SetCheck(m_pCurrentSettings->GetVSync() ? BST_CHECKED : BST_UNCHECKED);

					pResX->FindStringExact(0, StrX);
					pResY->FindStringExact(0, StrY);

					RefreshPluginList();
				}
			}
		}
	}

	void WindowerConfigDlg::FillSupportedRes()
	{
		CComboBox *pResX = static_cast<CComboBox*>(GetDlgItem(IDC_RESX_COMBO));
		CComboBox *pResY = static_cast<CComboBox*>(GetDlgItem(IDC_RESY_COMBO));

		if (pResX != NULL && pResY != NULL && m_pCurrentSettings != NULL)
		{
			LPDIRECT3D9 pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
			int DisplayModesCount, iX = -1, iY = -1;
			D3DDISPLAYMODE DisplayMode;
			bool EnumSuccess = false;
			TCHAR StrX[8], StrY[8];

			if (pDirect3D != NULL)
			{
				if ((DisplayModesCount = pDirect3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8)) > 0)
				{
					int CurrentX, CurrentY, x, y;

					CurrentX = m_pCurrentSettings->GetResX();
					CurrentY = m_pCurrentSettings->GetResY();

					// enumerate the display modes supported by the graphics card
					for (int i = 0; i < DisplayModesCount; i++)
					{
						if(pDirect3D->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &DisplayMode) == D3D_OK)
						{
							if (EnumSuccess == false)
								EnumSuccess = true;

							_ltot_s(DisplayMode.Width,  StrX, 10);
							_ltot_s(DisplayMode.Height, StrY, 10);

							// avoid duplicates
							if (pResX->FindString(0, StrX) == CB_ERR)
							{
								x = pResX->AddString(StrX);
								// save the index of the current value
								if (DisplayMode.Width == CurrentX)
									iX = x;
							}
							// avoid duplicates
							if (pResY->FindString(0, StrY) == CB_ERR)
							{
								y = pResY->AddString(StrY);
								// save the index of the current value
								if (DisplayMode.Height == CurrentY)
									iY = y;
							}
						}
						else
							break;
					}
				}

				_ltot_s(m_pCurrentSettings->GetResX(), StrX, 10);
				_ltot_s(m_pCurrentSettings->GetResY(), StrY, 10);

				// set the current resolution in the combo boxes
				if (iX >= 0 && EnumSuccess)
					pResX->SetCurSel(iX);
				else
					pResX->SetCurSel(pResX->AddString(StrX));

				if (iY >= 0 && EnumSuccess)
					pResY->SetCurSel(iY);
				else
					pResY->SetCurSel(pResY->AddString(StrY));

				pDirect3D->Release();
			}
		}
	}

	void WindowerConfigDlg::OnSave()
	{
		UpdateActivePlugins();

		if (m_pSettingsManager != NULL)
			m_pSettingsManager->Save();

		OnOK();
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
			CString NewName = PROFILE_PREFIX _T("New Profile");
			TCHAR *pDisplayName = NewName.GetBuffer() + PROFILE_PREFIX_LENGTH;
			WindowerProfile *pNewSettings = NULL;

			while(pProfiles->FindString(0, pDisplayName) != CB_ERR)
			{
				GenerateNewName(NewName);
				pDisplayName = NewName.GetBuffer() + PROFILE_PREFIX_LENGTH;
			}

			if (m_pCurrentSettings != NULL)
			{
				UpdateActivePlugins();
				pNewSettings = m_pSettingsManager->CreateProfile(NewName, *m_pCurrentSettings);
			}

			if (pNewSettings != NULL)
			{
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
						if (m_pPluginManager->IsPluginConfigurable(PluginName))
							m_pPluginManager->ConfigurePlugin(PluginName, (LPVOID)m_pCurrentSettings->GetName());

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
}