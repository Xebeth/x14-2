/**************************************************************************
	created		:	2011-10-29
	filename	: 	WindowerConfigDlg.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Main dialog
**************************************************************************/
#ifndef __WINDOWER_CONFIG_DLG_H__
#define __WINDOWER_CONFIG_DLG_H__

namespace PluginFramework
{
	class PluginManager;
	class ServiceParam;
	class PluginInfo;	
	class IPlugin;
}

namespace Windower
{
	//! \brief Main dialog
	class WindowerConfigDlg : public CDialog
	{
		enum { IDD = IDD_CONFIG_DIALOG };
	public:

		enum ePluginListColumns
		{
			LIST_COL_NAME = 0,
			LIST_COL_VERSION,
			LIST_COL_FRAMEWORK,
			LIST_COL_AUTHOR,
			LIST_COL_COMPAT,
			LIST_COL_DESC,
			LIST_COL_PATH,
			LIST_COL_COUNT
		};

		WindowerConfigDlg(SettingsManager *pSettingsManager, CWnd* pParent = NULL);
		~WindowerConfigDlg();

		void InsertPlugin(CListCtrl *pListCtrl_in_out, const PluginFramework::PluginInfo &PluginInfo_in);

		afx_msg void OnConfigure(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnProfileRename();
		afx_msg void OnProfilesChange();

		afx_msg void OnLanguageChange();
		
		afx_msg void OnDeleteProfile();
		afx_msg void OnVSyncChange();
		afx_msg void OnPathBrowse();
		afx_msg void OnPathChange();
		afx_msg void OnNewProfile();
		afx_msg void OnLaunch();
		afx_msg void OnSave();

	protected:
		class DummyServices : public PluginFramework::IPluginServices
		{
		public:
			explicit DummyServices(const PluginFramework::VersionInfo &Version_in, const string_t& WorkingDir_in)
				: PluginFramework::IPluginServices(Version_in), m_WorkingDir(WorkingDir_in) {}
		private:
			virtual bool InvokeService(const string_t&, const string_t&, const PluginFramework::ServiceParam&) const { return true; }
			virtual bool UnsubscribeService(const string_t&, const string_t&, PluginFramework::IPlugin*) const { return true; }
			virtual bool SubscribeService(const string_t&, const string_t&, PluginFramework::IPlugin*) const { return true; }
			virtual const TCHAR* GetConfigFile() const { return m_WorkingDir.c_str(); }

			string_t m_WorkingDir;
		};

		HICON m_hIcon;
		PluginFramework::PluginManager *m_pPluginManager;
		SettingsManager *m_pSettingsManager;
		WindowerProfile *m_pCurrentSettings;
		DummyServices *m_pServices;
		int m_CurrentSel;

		void GenerateNewName(CString &NewName_in_out);
		void UpdateActivePlugins();
		void ActivateAllPlugins();		
		void RefreshPluginList();

		// Generated message map functions
		virtual BOOL OnInitDialog();
		afx_msg void OnPaint();
		afx_msg HCURSOR OnQueryDragIcon();
		DECLARE_MESSAGE_MAP()		
	};
}

#endif//__WINDOWER_CONFIG_DLG_H__