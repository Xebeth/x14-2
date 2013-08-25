/**************************************************************************
	created		:	2013-04-18
	filename	: 	UpdaterDlg.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __UPDATER_DLG_H__
#define __UPDATER_DLG_H__

namespace PluginFramework
{
	class PluginManager;
}

namespace Windower
{
	class SettingsManager;
	class WindowerUpdater;
}
class UpdaterDlg : public BaseWizardPage, public Updater::IThreadProgress
{
	enum { IDD = IDD_UPDATE_DLG };
public:
	enum eUpdateListColumns
	{
		LIST_COL_NAME = 0,
		LIST_COL_CURRENT_FRAMEWORK,
		LIST_COL_NEW_FRAMEWORK,
		LIST_COL_CURRENT_VERSION,
		LIST_COL_NEW_VERSION,		
		LIST_COL_AUTHOR,
		LIST_COL_COMPAT,
		LIST_COL_DESC,
		LIST_COL_URL,
		LIST_COL_COUNT
	};

	explicit UpdaterDlg(Windower::WindowerProfile *pSettings_in);
	~UpdaterDlg();

	void OnProgress(unsigned long Completed_in,
					unsigned long Total_in,
					const TCHAR *pFeedbackMsg_in);
	void PumpMessages();
	void OnFinish();
	void OnCancel();
	void OnStart();

	bool InitializePage();

protected:
	afx_msg void OnStartClick();
	DECLARE_MESSAGE_MAP()

	Windower::WindowerUpdater *m_pUpdater;
	CProgressCtrl *m_pProgressCtrl;	
	CString m_ProgressText;
	CString m_StatusText;	
	bool m_bStarted;
};


#endif//__UPDATER_DLG_H__
