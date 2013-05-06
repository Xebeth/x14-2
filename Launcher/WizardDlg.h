/**************************************************************************
	created		:	2013-04-18
	filename	: 	WizardDlg.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __WIZARD_DLG_H__
#define __WIZARD_DLG_H__

namespace PluginFramework
{
	class PluginManager;
}

namespace Windower
{
	class WindowerUpdater;
	class PluginSettings;
}

class WizardFinishPage;
class WizardStartPage;
class UpdaterDlg;

class WizardDlg : public CPropertySheet
{
	enum { IDD = IDD_UPDATE_DLG };
public:
	enum eWizardTasks
	{
		TASK_NONE				= 0x0000U,
		TASK_START_SUMMARY		= 0x0001U,
		TASK_CHECK_UPDATES		= 0x0002U,
		TASK_CONFIGURE_PATH		= 0x0004U,
		TASK_CONFIGURE_PLUGINS	= 0x0008U,
		TASK_FINISH_SUMMARY		= 0x0010U,
		ALL_TASKS_MASK			= 0x001FU,
	};

	WizardDlg(PluginFramework::PluginManager &PluginManager_in,
			  Windower::PluginSettings &Settings_in);
	~WizardDlg();

	const TCHAR* GetProfileName() const;
	UINT CreatePages(UINT Tasks_in);
	void ResetPages();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnWizBack();
	afx_msg void OnWizNext();
	afx_msg void OnWizFinish();	

	PluginFramework::PluginManager &m_PluginManager;
	Windower::PluginSettings &m_Settings;

	WizardFinishPage *m_pFinishPage;
	WizardStartPage *m_pStartPage;
	UpdaterDlg *m_pUpdaterPage;
};


#endif//__WIZARD_DLG_H__