/**************************************************************************
	created		:	2013-04-18
	filename	: 	WizardDlg.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include "resource.h"

#include <PluginPropertyPage.h>
#include "BaseWizardPage.h"

#include "WizardFinishPage.h"
#include "WizardStartPage.h"

#include "WizardDlg.h"

BEGIN_MESSAGE_MAP(WizardDlg, CPropertySheet)
	ON_BN_CLICKED(ID_WIZNEXT, OnWizNext)
	ON_BN_CLICKED(ID_WIZBACK, OnWizBack)
	ON_BN_CLICKED(ID_WIZFINISH, OnWizFinish)
END_MESSAGE_MAP()

WizardDlg::WizardDlg(PluginFramework::PluginManager &PluginManager_in,
					 Windower::SettingsManager &SettingsManager_in)
	: CPropertySheet(_T("x14-2 configuration wizard")),
	  m_PluginManager(PluginManager_in), m_SettingsManager(SettingsManager_in), m_pSettings(new Windower::WindowerProfile()),
	  m_pFinishPage(nullptr), m_pStartPage(nullptr), m_pUpdaterPage(nullptr)
{
	m_psh.hInstance = AfxGetInstanceHandle();

	SettingsManager_in.LoadSettings(m_pSettings);
}

WizardDlg::~WizardDlg()
{
	UINT PageCount = GetPageCount();
	CPropertyPage *pPage;

	if (m_pSettings != nullptr)
	{
		delete m_pSettings;
		m_pSettings = nullptr;
	}

	for (UINT PageIndex = 0U; PageIndex < PageCount; ++PageIndex)
	{
		pPage = GetPage(PageIndex);

		if (pPage != nullptr)
			delete pPage;
	}
}

UINT WizardDlg::CreatePages(UINT Tasks_in)
{
	UINT Result = 0U;

	if (Tasks_in != TASK_NONE)
	{
		SetWizardMode();

		m_psh.pszbmHeader = MAKEINTRESOURCE(IDB_BANNER_ICON);
		m_psh.dwFlags |= PSH_WIZARD97 | PSH_HEADER;

		CString StartSummary = _T("The wizard will accomplish the following tasks:\n");

		// add the start page if needed
		if ((Tasks_in & TASK_START_SUMMARY) == TASK_START_SUMMARY)
		{
			m_pStartPage = new WizardStartPage(m_SettingsManager, m_pSettings);
			AddPage(m_pStartPage);
			AfxInitRichEdit();
			++Result;
		}
/*
		// add the updater page if needed		
		if ((Tasks_in & TASK_CHECK_UPDATES) == TASK_CHECK_UPDATES)
		{
			StartSummary += _T("  - Check for updates\n");
			m_pUpdaterPage = new UpdaterDlg(m_pSettings);
			AddPage(m_pUpdaterPage);
			++Result;
		}
*/
		// add the plugin pages
		if ((Tasks_in & TASK_CONFIGURE_PLUGINS) == TASK_CONFIGURE_PLUGINS)
		{
			const PluginFramework::RegisteredPlugins& Plugins = m_PluginManager.GetRegisteredPlugins();
			PluginFramework::RegisteredPlugins::const_iterator PluginIt, EndIt = Plugins.cend();
			Windower::ConfigurablePlugin *pConfigPlugin;
			Windower::PluginPropertyPage *pPluginPage;
			PluginFramework::IPlugin *pPlugin;	
			CString PluginsSummary;
			string_t PageTitle;
			
			for (PluginIt = Plugins.cbegin(); PluginIt != EndIt; ++PluginIt)
			{
				pPlugin = m_PluginManager.LoadPlugin(PluginIt->second.GetName());

				if (pPlugin != nullptr && pPlugin->IsConfigurable())
				{
					pConfigPlugin = static_cast<Windower::ConfigurablePlugin*>(pPlugin);
					pPluginPage = pConfigPlugin->GetPropertyPage();

					if (pPluginPage != nullptr)
					{
						if ((pPlugin->GetCompatibilityFlags() & PLUGIN_COMPATIBILITY_NO_WIZARD) == 0)
						{
							PluginsSummary.AppendFormat(_T("      - %s\n"), PluginIt->first.c_str());
							pPluginPage->SetFlags(TASK_CONFIGURE_PLUGINS);
							pPluginPage->m_psp.dwFlags |= PSP_HIDEHEADER;
							pPluginPage->SetSettings(m_pSettings);
							AddPage(pPluginPage);
							++Result;
						}
						else
							delete pPluginPage;
					}
				}
			}

			if (PluginsSummary.IsEmpty() == false)
				StartSummary.AppendFormat(_T("  - Configure the plugins:\n%s"), PluginsSummary.GetBuffer());
		}
		// add the finish page if needed
		if ((Tasks_in & TASK_FINISH_SUMMARY) == TASK_FINISH_SUMMARY)
		{
			m_pFinishPage = new WizardFinishPage(m_pSettings);
			AddPage(m_pFinishPage);
			++Result;
		}

		if (m_pStartPage != nullptr && StartSummary.IsEmpty() == false)
			m_pStartPage->SetSummary(StartSummary);
	}

	return Result;
}

void WizardDlg::OnWizNext()
{
	Windower::PluginPropertyPage *pPage = static_cast<Windower::PluginPropertyPage*>(GetActivePage());

	if (pPage != nullptr)
	{
		if (pPage->IsPageValid(nullptr))
		{
			if (m_pSettings != nullptr)
			{
				const string_t &Name = pPage->GetPluginName();

				if (Name.empty() == false)
					m_pSettings->UpdatePluginList(Name);
			}

			pPage->Commit();
			Default();
		}

		if (pPage == m_pStartPage && m_pStartPage->ProfileHasChanged())
			ResetPages();		
	}
}

void WizardDlg::OnWizBack()
{
	Windower::PluginPropertyPage *pPage = static_cast<Windower::PluginPropertyPage*>(GetActivePage());

	if (pPage != nullptr && pPage->IsPageValid(nullptr))
		pPage->Commit();

	Default();
}

void WizardDlg::ResetPages() const
{
	Windower::PluginPropertyPage *pPage;
	UINT PageCount = GetPageCount();

	for (UINT PageIndex = 0U; PageIndex < PageCount; ++PageIndex)
	{
		pPage = static_cast<Windower::PluginPropertyPage*>(GetPage(PageIndex));

		if (pPage != nullptr)
			pPage->Revert();
	}
}

void WizardDlg::OnWizFinish()
{
	m_SettingsManager.SaveSettings(m_pSettings);

	if (!UpdateData(TRUE))
	{
		TRACE(traceAppMsg, 0, "UpdateData failed during dialog termination.\n");
		// the UpdateData routine will set focus to correct item
		return;
	}
	EndDialog(IDOK);
}

const TCHAR* WizardDlg::GetProfileName() const
{ return m_pSettings ? m_pSettings->GetName() : nullptr; }