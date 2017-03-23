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
		PluginPropertySheet() : m_pSettings(nullptr), m_bSettingsChanged(false)
		{ m_psh.dwFlags |= (PSH_NOAPPLYNOW); }

		virtual bool Save()
		{
			bool Result = false;

			if (m_pSettings != nullptr)
			{
				PluginPropertyPage *pPage = nullptr;
				int PageCount = GetPageCount();
				string_t Feedback;

				Result = true;

				for (int PageIndex = 0; PageIndex < PageCount; ++PageIndex)
				{
					pPage = reinterpret_cast<PluginPropertyPage*>(GetPage(PageIndex));

					if (pPage != nullptr)
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

			if (pFirstPage != nullptr)
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
		: PluginFramework::IPlugin(pServices_in), m_pConfigDlg(nullptr),
		  m_pConfigPage(nullptr), m_pSettings(new WindowerProfile())
	{
		// create the settings
		if (pServices_in != nullptr)
			pServices_in->LoadSettings(m_pSettings);
	}

	//! \brief ConfigurablePlugin destructor
	ConfigurablePlugin::~ConfigurablePlugin()
	{
		if (m_pConfigPage != nullptr)
		{
			delete m_pConfigPage;
			m_pConfigPage = nullptr;
		}

		if (m_pConfigDlg != nullptr)
		{
			delete m_pConfigDlg;
			m_pConfigDlg = nullptr;
		}
		
		if (m_pSettings != nullptr)
		{
			delete m_pSettings;
			m_pSettings = nullptr;
		}		
	}

	bool ConfigurablePlugin::Configure(PluginFramework::IPlugin *pInstance_in, const LPVOID pUserData_in)
	{
		if (pInstance_in != nullptr)
		{
			ConfigurablePlugin *pConfigPlugin = static_cast<ConfigurablePlugin*>(pInstance_in);

			if (pConfigPlugin->m_pSettings != nullptr)
			{
				// create the property page if needed
				if (pConfigPlugin->m_pConfigPage == nullptr)
					pConfigPlugin->m_pConfigPage = pConfigPlugin->GetPropertyPage();
				// create the property sheet if needed
				if (pConfigPlugin->m_pConfigDlg == nullptr && pConfigPlugin->m_pConfigPage != nullptr)
				{
					pConfigPlugin->m_pConfigDlg = new PluginPropertySheet(_T("Plugin configuration"),
																		  pConfigPlugin->m_pSettings);
					// set up the property sheet
					pConfigPlugin->m_pConfigDlg->AddPage(pConfigPlugin->m_pConfigPage);				
				}
				// display the property sheet
				if (pConfigPlugin->m_pConfigDlg != nullptr)
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