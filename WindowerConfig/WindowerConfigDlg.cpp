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

#include "WindowerConfigDlg.h"

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
		ON_BN_CLICKED(IDOK, &WindowerConfigDlg::OnSave)
	END_MESSAGE_MAP()

	WindowerConfigDlg::WindowerConfigDlg(SettingsManager *pSettingsManager,CWnd* pParent)
		: CDialog(WindowerConfigDlg::IDD, pParent), m_pSettingsManager(pSettingsManager)
	{
		m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		m_pCurrentSettings = NULL;
		m_CurrentSel = 0;
	}

	// WindowerConfigDlg message handlers

	BOOL WindowerConfigDlg::OnInitDialog()
	{
		CComboBox *pProfiles = (CComboBox*)GetDlgItem(IDC_PROFILES_COMBO);
		CDialog::OnInitDialog();

		// Set the icon for this dialog.  The framework does this automatically
		//  when the application's main window is not a dialog
		SetIcon(m_hIcon, TRUE);			// Set big icon
		SetIcon(m_hIcon, FALSE);		// Set small icon

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

					ItemCount = pProfiles->AddString(pName);
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

		return TRUE;  // return TRUE  unless you set the focus to a control
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
		CComboBox *pProfiles = (CComboBox*)GetDlgItem(IDC_PROFILES_COMBO);

		if (m_pCurrentSettings != NULL && pProfiles != NULL)
		{
			CString ProfileName;

			pProfiles->GetWindowText(ProfileName);

			if (ProfileName.Compare(m_pCurrentSettings->GetName()) != 0)
			{
				// delete the current combo item
				pProfiles->DeleteString(m_CurrentSel);
				// add the item back
				m_CurrentSel = pProfiles->AddString(ProfileName);
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
		CComboBox *pResX	 = (CComboBox*)GetDlgItem(IDC_RESX_COMBO);
		CComboBox *pResY	 = (CComboBox*)GetDlgItem(IDC_RESY_COMBO);

		if (m_pCurrentSettings != NULL && pResX != NULL && pResY != NULL)
		{
			CString CurrentValue;
			LONG ResX, ResY;

			pResX->GetWindowText(CurrentValue);
			ResX = _ttol(CurrentValue);
			pResY->GetWindowText(CurrentValue);
			ResY = _ttol(CurrentValue);

			m_pCurrentSettings->SetResolution(ResX, ResY);
		}
	}

	void WindowerConfigDlg::OnProfilesChange()
	{
		CComboBox *pProfiles = (CComboBox*)GetDlgItem(IDC_PROFILES_COMBO);
		CComboBox *pResX	 = (CComboBox*)GetDlgItem(IDC_RESX_COMBO);
		CComboBox *pResY	 = (CComboBox*)GetDlgItem(IDC_RESY_COMBO);
		CButton *pVSync		 = (CButton*)GetDlgItem(IDC_VSYNC);

		m_pCurrentSettings = NULL;

		if (pProfiles != NULL && pResX != NULL && pResY != NULL && pVSync != NULL)
		{
			m_CurrentSel = pProfiles->GetCurSel();

			if (m_CurrentSel >= 0)
			{
				m_pCurrentSettings = (WindowerProfile*)pProfiles->GetItemData(m_CurrentSel);

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
				}
			}
		}
	}

	void WindowerConfigDlg::FillSupportedRes()
	{
		CComboBox *pResX = (CComboBox*)GetDlgItem(IDC_RESX_COMBO);
		CComboBox *pResY = (CComboBox*)GetDlgItem(IDC_RESY_COMBO);

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
		CComboBox *pProfiles = (CComboBox*)GetDlgItem(IDC_PROFILES_COMBO);

		if (pProfiles != NULL && m_pCurrentSettings != NULL)
		{
			CString NewName = _T("New Profile");
			WindowerProfile *pNewSettings;

			while(pProfiles->FindString(0, NewName) != CB_ERR)
				GenerateNewName(NewName);

			pNewSettings = m_pSettingsManager->CreateProfile(NewName, *m_pCurrentSettings);
			m_pCurrentSettings->SetName(NewName);

			if (pNewSettings != NULL)
			{
				m_CurrentSel = pProfiles->AddString(NewName.GetBuffer());
				m_pCurrentSettings = pNewSettings;

				pProfiles->SetItemData(m_CurrentSel, (DWORD_PTR)pNewSettings);
				pProfiles->SetCurSel(m_CurrentSel);
				pProfiles->SetEditSel(-1, -1);
				pProfiles->SetFocus();
			}

			GetDlgItem(IDC_DELETE_PROFILE)->EnableWindow(pProfiles->GetCount() > 1);
		}	
	}

	void WindowerConfigDlg::OnDeleteProfile()
	{
		CComboBox *pProfiles = (CComboBox*)GetDlgItem(IDC_PROFILES_COMBO);

		if (pProfiles != NULL && m_pCurrentSettings != NULL)
		{
			if (pProfiles->GetCount() > 1)
			{
				m_pSettingsManager->DeleteProfile(m_pCurrentSettings->GetName());
				pProfiles->DeleteString(m_CurrentSel);
				// set the new selection
				pProfiles->SetCurSel((m_CurrentSel == 0) ? 0 : --m_CurrentSel);
				// update the data
				OnProfilesChange();
			}
		}

		GetDlgItem(IDC_DELETE_PROFILE)->EnableWindow(pProfiles->GetCount() > 1);
	}

	void WindowerConfigDlg::OnVSyncChange()
	{
		CButton *pVSync = (CButton*)GetDlgItem(IDC_VSYNC);

		if (m_pCurrentSettings != NULL && pVSync != NULL)
		{
			m_pCurrentSettings->SetVSync(pVSync->GetCheck() == BST_CHECKED);
		}
	}
}