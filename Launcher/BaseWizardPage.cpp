/**************************************************************************
	created		:	2013-05-05
	filename	: 	BaseWizardPage.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <afxdlgs.h>

#include <PluginPropertyPage.h>

#include "BaseWizardPage.h"
#include "WizardDlg.h"

BaseWizardPage::BaseWizardPage(Windower::WindowerProfile *pSettings_in,
							   const string_t &PluginName_in,
							   UINT TemplateResID_in, UINT IconResID_in)
	: Windower::PluginPropertyPage(pSettings_in, PluginName_in,
								   TemplateResID_in, IconResID_in) {}

BOOL BaseWizardPage::OnSetActive()
{
	WizardDlg *pWizard = static_cast<WizardDlg*>(GetParent());

	if (pWizard != NULL)
		pWizard->SetIcon(m_hIcon, FALSE);

	return __super::OnSetActive();
}

bool BaseWizardPage::InitializePage()
{
	return true;
}

bool BaseWizardPage::IsPageValid(string_t *pFeedback_out) const
{
	return true;
}

bool BaseWizardPage::Commit()
{
	return true;
}

void BaseWizardPage::Revert() {}