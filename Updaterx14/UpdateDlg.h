/**************************************************************************
	created		:	2013-04-18
	filename	: 	UpdateDlg.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __UPDATE_DLG_H__
#define __UPDATE_DLG_H__

#ifndef _UPDATER_X14_SDK_INCLUDE_
	#error Please include the global header file 'Updaterx14.h'
#endif

namespace PluginFramework
{
	class PluginManager;
}

namespace Windower
{
	class SettingsManager;
}

class WindowerUpdater;

class UPDATER_API UpdateDlg : public CDialog, public Updater::IThreadProgress
{
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

	UpdateDlg(Windower::SettingsManager *pSettingsManager, CWnd* pParent = NULL);
	~UpdateDlg();

	virtual void OnProgress(unsigned long Completed_in,
							unsigned long Total_in,
							const TCHAR *pFeedbackMsg_in);
	virtual void PumpMessages();
	virtual void OnFinish();
	virtual void OnCancel();
	virtual void OnStart();

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStartClick();
	afx_msg void OnPaint();		

	class DummyServices : public PluginFramework::IPluginServices
	{
	public:
		explicit DummyServices(const PluginFramework::VersionInfo &Version_in, const string_t& WorkingDir_in)
			: PluginFramework::IPluginServices(Version_in), m_WorkingDir(WorkingDir_in) {}
	private:
		virtual bool InvokeService(const string_t&, const string_t&, PluginFramework::ServiceParam &) const { return true; }
		virtual bool UnsubscribeService(const string_t&, const string_t&, PluginFramework::IPlugin*) const { return true; }
		virtual bool SubscribeService(const string_t&, const string_t&, PluginFramework::IPlugin*) const { return true; }
		virtual const TCHAR* GetConfigFile() const { return m_WorkingDir.c_str(); }

		string_t m_WorkingDir;
	};

	PluginFramework::PluginManager *m_pPluginManager;
	Windower::SettingsManager *m_pSettingsManager;
	CProgressCtrl *m_pProgressCtrl;
	WindowerUpdater *m_pUpdater;
	DummyServices *m_pServices;
	CString m_ProgressText;
	CString m_StatusText;	
	bool m_bStarted;
	HICON m_hIcon;
};

#endif//__UPDATEDLG_H__
