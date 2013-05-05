/**************************************************************************
	created		:	2013-05-04
	filename	: 	ConfigurablePlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Plugin with configuration screen
**************************************************************************/
#include "stdafx.h"
#include <afxdlgs.h>

#include "PluginPropertyPage.h"
#include "ConfigurablePlugin.h"
#include "PluginSettings.h"

namespace Windower
{
	//! \brief Plugin property sheet used to display the configuration dialog
	class PluginPropertySheet : public CPropertySheet
	{
		DECLARE_DYNCREATE(PluginPropertySheet)
	public:
		PluginPropertySheet(const TCHAR *pTitle_in, Settings::SettingsIniFile *pSettings_in)
			: CPropertySheet(pTitle_in), m_pSettings(pSettings_in) {}
		PluginPropertySheet() : m_pSettings(NULL), m_bSettingsChanged(false) {}

		virtual bool Save()
		{
			PluginPropertyPage *pPage = NULL;
			int PageCount = GetPageCount();

			for (int PageIndex = 0; PageIndex < PageCount; ++PageIndex)
			{
				pPage = reinterpret_cast<PluginPropertyPage*>(GetPage(PageIndex));

				if (pPage != NULL)
					m_bSettingsChanged |= pPage->Save();
			}
			

			return m_bSettingsChanged;
		}

		bool SettingsChanged() const { return m_bSettingsChanged; }

	protected:
		DECLARE_MESSAGE_MAP()

		BOOL OnInitDialog()
		{
			BOOL Result = CPropertySheet::OnInitDialog();
			CPropertyPage *pFirstPage = GetPage(0);

			if (pFirstPage != NULL)
			{
				SetIcon(pFirstPage->GetIcon(FALSE), FALSE);
				SetIcon(pFirstPage->GetIcon(TRUE), TRUE);
			}

			SetDlgItemText(IDCANCEL, _T("Cancel"));
			SetDlgItemText(IDOK, _T("Save"));

			return Result;
		}

		virtual void OnSave()
		{
			Default();
			Save();
		}

		//! settings of the plugin
		Settings::SettingsIniFile *m_pSettings;
		//! flag specifying if the settings have changed
		bool m_bSettingsChanged;
	};

	IMPLEMENT_DYNCREATE(PluginPropertySheet, CPropertySheet)
	BEGIN_MESSAGE_MAP(PluginPropertySheet, CPropertySheet)
		ON_COMMAND(IDOK, &PluginPropertySheet::OnSave)
	END_MESSAGE_MAP()

	ConfigurablePlugin::ConfigurablePlugin(PluginFramework::IPluginServices *pServices_in)
		: PluginFramework::IPlugin(pServices_in), m_pConfigDlg(NULL),
		  m_pConfigPage(NULL), m_pSettings(NULL) {}

	//! \brief ConfigurablePlugin destructor
	ConfigurablePlugin::~ConfigurablePlugin()
	{
		if (m_pConfigPage != NULL)
		{
			delete m_pConfigPage;
			m_pConfigPage = NULL;
		}

		if (m_pConfigDlg != NULL)
		{
			delete m_pConfigDlg;
			m_pConfigDlg = NULL;
		}

		m_pSettings = NULL;
	}

	bool ConfigurablePlugin::Configure(PluginFramework::IPlugin *pInstance_in, const LPVOID pUserData_in)
	{
		if (pInstance_in != NULL)
		{
			ConfigurablePlugin *pConfigPlugin = static_cast<ConfigurablePlugin*>(pInstance_in);
			const TCHAR *pProfileName = reinterpret_cast<const TCHAR*>(pUserData_in);

			// set the default section of the settings
			if (pConfigPlugin->m_pSettings != NULL && pProfileName != NULL)
				pConfigPlugin->m_pSettings->SetCurrentSection(pProfileName);
			// create the property page if needed
			if (pConfigPlugin->m_pConfigPage == NULL)
				pConfigPlugin->m_pConfigPage = pConfigPlugin->GetPropertyPage();
			// create the property sheet if needed
			if (pConfigPlugin->m_pConfigDlg == NULL && pConfigPlugin->m_pConfigPage != NULL)
			{
				pConfigPlugin->m_pConfigDlg = new PluginPropertySheet(_T("Plugin configuration"),
																	   pConfigPlugin->m_pSettings);
				// set up the property sheet
				pConfigPlugin->m_pConfigDlg->m_psh.dwFlags |= (PSH_NOAPPLYNOW);
				pConfigPlugin->m_pConfigDlg->AddPage(pConfigPlugin->m_pConfigPage);				
			}
			// display the property sheet
			if (pConfigPlugin->m_pConfigDlg != NULL)
			{
				pConfigPlugin->m_pConfigDlg->DoModal();

				if (pConfigPlugin->m_pConfigDlg->SettingsChanged())
				{
					pConfigPlugin->OnSettingsChanged();

					return true;
				}
			}
		}	

		return false;
	}
}