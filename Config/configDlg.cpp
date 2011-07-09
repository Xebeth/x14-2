// configDlg.cpp : implementation file
//

#include "stdafx.h"
#include <vector>
#include <d3d9.h>

#include "SimpleIni.h"

#include "SettingsInterface.h"
#include "WindowerSettings.h"
#include "SettingsIniFile.h"

#include "SettingsManager.h"

#include "config.h"
#include "configDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

CconfigDlg::CconfigDlg(SettingsManager *pSettingsManager,CWnd* pParent /*=NULL*/)
	: CDialog(CconfigDlg::IDD, pParent), m_pSettingsManager(pSettingsManager)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pCurrentSettings = NULL;
}

CconfigDlg::~CconfigDlg()
{
	if (m_pCurrentSettings != NULL)
	{
		delete m_pCurrentSettings;
		m_pCurrentSettings = NULL;
	}
}

void CconfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CconfigDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_PROFILES_COMBO, &CconfigDlg::OnProfilesChange)
	ON_BN_CLICKED(IDOK, &CconfigDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_NEW_PROFILE, &CconfigDlg::OnBnClickedNewProfile)
	ON_BN_CLICKED(IDC_TEST_RES, &CconfigDlg::OnBnClickedTestRes)
END_MESSAGE_MAP()


// CconfigDlg message handlers

BOOL CconfigDlg::OnInitDialog()
{
	CComboBox *pProfiles = (CComboBox*)GetDlgItem(IDC_PROFILES_COMBO);
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if (pProfiles != NULL)
	{
		const TCHAR *pDefault = m_pSettingsManager->GetDefaultProfile();
		SettingsConstIterator pEnd = m_pSettingsManager->End();
		int DefaultItem = 0, ItemCount = 0;
		SettingsIterator pIter;

		for (pIter = m_pSettingsManager->Begin(); pIter != pEnd; pIter++)
		{
			const TCHAR *pName = (*pIter)->GetName();

			pProfiles->AddString(pName);
			pProfiles->SetItemData(ItemCount, (DWORD_PTR)*pIter);

			if (_tcsicmp(pName, pDefault) == 0)
				DefaultItem = ItemCount;

			ItemCount++;
		}

		pProfiles->SetCurSel(DefaultItem);
		OnProfilesChange();

		FillSupportedRes();
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CconfigDlg::OnPaint()
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
HCURSOR CconfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CconfigDlg::OnProfilesChange()
{
	CComboBox *pProfiles = (CComboBox*)GetDlgItem(IDC_PROFILES_COMBO);
	CComboBox *pResX	 = (CComboBox*)GetDlgItem(IDC_RESX_COMBO);
	CComboBox *pResY	 = (CComboBox*)GetDlgItem(IDC_RESY_COMBO);

	WindowerSettings *pSettings = NULL;

	if (pProfiles != NULL && pResX != NULL && pResY != NULL)
	{
		pSettings = (WindowerSettings*)pProfiles->GetItemData(pProfiles->GetCurSel());

		if (pSettings != NULL)
		{
			TCHAR StrX[8], StrY[8];
			int iX, iY;

			m_pSettingsManager->SetDefaultProfile(pSettings->GetName());
			_ltot_s(pSettings->GetResX(), StrX, 10);
			_ltot_s(pSettings->GetResY(), StrY, 10);
			
			if (m_pCurrentSettings != NULL)
				delete m_pCurrentSettings;

			m_pCurrentSettings = new WindowerSettings(pSettings);

			pResX->SetWindowText(StrX);
			pResY->SetWindowText(StrY);

			if ((iX = pResX->FindString(0, StrX)) != CB_ERR)
				pResX->SetCurSel(iX);
			if ((iY = pResY->FindString(0, StrY)) != CB_ERR)
				pResY->SetCurSel(iY);
		}
	}
}

void CconfigDlg::FillSupportedRes()
{
	CComboBox *pResX	 = (CComboBox*)GetDlgItem(IDC_RESX_COMBO);
	CComboBox *pResY	 = (CComboBox*)GetDlgItem(IDC_RESY_COMBO);

	if (pResX != NULL && pResY != NULL && m_pCurrentSettings != NULL)
	{
		LPDIRECT3D9 pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
		D3DDISPLAYMODE DisplayMode;
		bool EnumSuccess = false;
		TCHAR StrX[8], StrY[8];
		int DisplayModesCount;

		if (pDirect3D != NULL)
		{
			if ((DisplayModesCount = pDirect3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8)) > 0)
			{
				int CurrentX, CurrentY, iX = 0, iY = 0;

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
							int x = pResX->AddString(StrX);
							// save the index of the current value
							if (DisplayMode.Width == CurrentX)
								iX = x;
						}
						// avoid duplicates
						if (pResY->FindString(0, StrY) == CB_ERR)
						{
							int y = pResY->AddString(StrY);
							// save the index of the current value
							if (DisplayMode.Height == CurrentY)
								iY = y;
						}
					}
					else
						break;
				}
				// set the current resolution in the combo boxes
				pResX->SetCurSel(iX);
				pResY->SetCurSel(iY);
			}
			// default values if the enumeration failed
			if (EnumSuccess == false && m_pCurrentSettings != NULL)
			{
				_ltot_s(m_pCurrentSettings->GetResX(), StrX, 10);
				_ltot_s(m_pCurrentSettings->GetResY(), StrY, 10);

				pResX->SetCurSel(pResX->AddString(StrX));
				pResY->SetCurSel(pResY->AddString(StrY));
			}

			pDirect3D->Release();
		}
	}
}

void CconfigDlg::OnBnClickedOk()
{
	CComboBox *pProfiles = (CComboBox*)GetDlgItem(IDC_PROFILES_COMBO);
	CComboBox *pResX	 = (CComboBox*)GetDlgItem(IDC_RESX_COMBO);
	CComboBox *pResY	 = (CComboBox*)GetDlgItem(IDC_RESY_COMBO);

	WindowerSettings *pSettings = NULL;

	if (pProfiles != NULL && pResX != NULL && pResY != NULL)
	{
		pSettings = (WindowerSettings*)pProfiles->GetItemData(pProfiles->GetCurSel());

		if (pSettings != NULL)
		{
			CString CurrentValue;
			LONG ResX, ResY;

			pResX->GetLBText(pResX->GetCurSel(), CurrentValue);
			ResX = _ttol(CurrentValue);
			pResY->GetLBText(pResY->GetCurSel(), CurrentValue);
			ResY = _ttol(CurrentValue);

			pSettings->SetResolution(ResX, ResY);
			pSettings->SetVSync(FALSE);

			m_pSettingsManager->Save();

			OnOK();
		}
	}
}

void CconfigDlg::OnBnClickedNewProfile()
{
	CComboBox *pProfiles = (CComboBox*)GetDlgItem(IDC_PROFILES_COMBO);

	if (pProfiles != NULL && m_pCurrentSettings != NULL)
	{
		const TCHAR *pNewProfile = L"New Profile";
		WindowerSettings *pNewSettings;
		int iNew;

		if (pProfiles->FindString(0, pNewProfile) == CB_ERR)
		{
			m_pCurrentSettings->SetName(pNewProfile);

			pNewSettings = m_pSettingsManager->CreateProfile(pNewProfile, m_pCurrentSettings);

			if (pNewSettings != NULL)
			{
				iNew = pProfiles->AddString(pNewProfile);

				pProfiles->SetItemData(iNew, (DWORD_PTR)pNewSettings);
				pProfiles->SetCurSel(iNew);
			}
		}
	}	
}

void CconfigDlg::OnBnClickedTestRes()
{
	if (m_pCurrentSettings != NULL)
	{
		LPDIRECT3D9 pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);

		if (pDirect3D != NULL)
		{
			BOOL VSync = m_pCurrentSettings->GetVSync();
			D3DPRESENT_PARAMETERS PresentParams;
			LPDIRECT3DDEVICE9 pDevice = NULL;
			D3DDISPLAYMODE DisplayMode;
			HWND hWnd = GetSafeHwnd();

			DisplayMode.Width = m_pCurrentSettings->GetResX();
			DisplayMode.Height = m_pCurrentSettings->GetResY();
			DisplayMode.Format = D3DFMT_X8R8G8B8;
			DisplayMode.RefreshRate = 0;

			PresentParams.Windowed = TRUE;
			PresentParams.AutoDepthStencilFormat = D3DFMT_D16;
			PresentParams.EnableAutoDepthStencil = TRUE;
			PresentParams.BackBufferCount = 1;
			PresentParams.BackBufferWidth = DisplayMode.Width;
			PresentParams.BackBufferHeight = DisplayMode.Height;
			PresentParams.BackBufferFormat = DisplayMode.Format;
			PresentParams.FullScreen_RefreshRateInHz = 0;
			PresentParams.hDeviceWindow = hWnd;
			PresentParams.PresentationInterval = VSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_DEFAULT;
			PresentParams.SwapEffect = VSync ? D3DSWAPEFFECT_COPY : D3DSWAPEFFECT_DISCARD;
			PresentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
			PresentParams.MultiSampleQuality = 0;
			PresentParams.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL | D3DPRESENTFLAG_DEVICECLIP;

			if (pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &PresentParams, &pDevice) == D3D_OK)
			{
				MessageBox(L"The current settings are valid.", L"Display mode test", MB_ICONINFORMATION);
				pDevice->Release();
				pDirect3D->Release();
			}
			else
			{
				MessageBox(L"The current settings are invalid!", L"Display mode test", MB_ICONERROR);
			}
		}
	}
}
