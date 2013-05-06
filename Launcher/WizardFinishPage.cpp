/**************************************************************************
	created		:	2013-05-05
	filename	: 	WizardFinishPage.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include <afxdlgs.h>

#include <PluginPropertyPage.h>

#include "BaseWizardPage.h"
#include "WizardFinishPage.h"
#include "WizardDlg.h"

BEGIN_MESSAGE_MAP(WizardFinishPage, BaseWizardPage)

END_MESSAGE_MAP()

/*! \brief TellDetectConfigDlg default constructor
	\param[in] : the parent window of the dialog
 */
WizardFinishPage::WizardFinishPage(Windower::PluginSettings *pSettings_in)
	: BaseWizardPage(pSettings_in, WizardFinishPage::IDD, IDR_MAINFRAME)
{
	m_PageFlags = WizardDlg::TASK_FINISH_SUMMARY;
}