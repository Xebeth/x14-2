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
#include "WindowerSettings.h"

namespace Windower
{
	//! \brief Plugin property sheet used to display the configuration dialog
	class PluginPropertySheet : public CPropertySheet
	{
		DECLARE_DYNCREATE(PluginPropertySheet)
		friend class ConfigurablePlugin;
	public:
		PluginPropertySheet(const TCHAR *pTitle_in, WindowerProfile *pSettings_in)
			: CPropertySheet(pTitle_in), m_pSettings(pSettings_in)
		{ m_psh.dwFlags |= (PSH_NOAPPLYNOW); }
		PluginPropertySheet() : m_pSettings(NULL), m_bSettingsChanged(false)
		{ m_psh.dwFlags |= (PSH_NOAPPLYNOW); }

		virtual bool Save()
		{
			bool Result = false;

			if (m_pSettings != NULL)
			{
				PluginPropertyPage *pPage = NULL;
				int PageCount = GetPageCount();
				string_t Feedback;

				Result = true;

				for (int PageIndex = 0; PageIndex < PageCount; ++PageIndex)
				{
					pPage = reinterpret_cast<PluginPropertyPage*>(GetPage(PageIndex));

					if (pPage != NULL)
					{
						// check if the page is valid then save
						if (pPage->IsPageValid(&Feedback))
							m_bSettingsChanged |= pPage->Commit();
						else
							Result = false;
						// update the feedback
						if (Feedback.empty() == false)
							m_Feedback += Feedback;
					}
				}

				return Result;
			}

			return false;
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
			m_Feedback.clear();

			if (Save() == false)
			{
				string_t ErrorMsg = _T("The following error(s) were reported:");

				if (m_Feedback.empty() == false)
				{
					ErrorMsg += m_Feedback;

					MessageBox(ErrorMsg.c_str(), _T("The configuration has not be saved!"), MB_OK | MB_ICONERROR);
				}
			}
			else
				Default();
		}

		//! settings of the plugin
		WindowerProfile *m_pSettings;
		//! flag specifying if the settings have changed
		bool m_bSettingsChanged;
		//! feedback from page validation
		string_t m_Feedback;
	};

	IMPLEMENT_DYNCREATE(PluginPropertySheet, CPropertySheet)
	BEGIN_MESSAGE_MAP(PluginPropertySheet, CPropertySheet)
		ON_COMMAND(IDOK, &PluginPropertySheet::OnSave)
	END_MESSAGE_MAP()

	ConfigurablePlugin::ConfigurablePlugin(PluginFramework::IPluginServices *pServices_in)
		: PluginFramework::IPlugin(pServices_in), m_pConfigDlg(NULL),
		  m_pConfigPage(NULL), m_pSettings(new WindowerProfile())
	{
		// create the settings
		if (pServices_in != NULL && pServices_in->LoadSettings(m_pSettings))
		{
			// set the sound file path from the settings
			OnSettingsChanged();
		}
	}

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
		
		if (m_pSettings != NULL)
		{
			delete m_pSettings;
			m_pSettings = NULL;
		}		
	}

	bool ConfigurablePlugin::Configure(PluginFramework::IPlugin *pInstance_in, const LPVOID pUserData_in)
	{
		if (pInstance_in != NULL)
		{
			ConfigurablePlugin *pConfigPlugin = static_cast<ConfigurablePlugin*>(pInstance_in);

			if (pConfigPlugin->m_pSettings != NULL)
			{
				// create the property page if needed
				if (pConfigPlugin->m_pConfigPage == NULL)
					pConfigPlugin->m_pConfigPage = pConfigPlugin->GetPropertyPage();
				// create the property sheet if needed
				if (pConfigPlugin->m_pConfigDlg == NULL && pConfigPlugin->m_pConfigPage != NULL)
				{
					pConfigPlugin->m_pConfigDlg = new PluginPropertySheet(_T("Plugin configuration"),
																		  pConfigPlugin->m_pSettings);
					// set up the property sheet
					pConfigPlugin->m_pConfigDlg->AddPage(pConfigPlugin->m_pConfigPage);				
				}
				// display the property sheet
				if (pConfigPlugin->m_pConfigDlg != NULL)
				{
					pConfigPlugin->m_pConfigDlg->DoModal();

					if (pConfigPlugin->m_pConfigDlg->SettingsChanged())
					{
						pConfigPlugin->OnSettingsChanged();

						return IPlugin::SaveSettings(pConfigPlugin->m_pSettings);
					}
				}
			}
		}	

		return false;
	}
}