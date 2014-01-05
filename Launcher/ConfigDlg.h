/**************************************************************************
	created		:	2011-10-29
	filename	: 	ConfigDlg.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Main dialog
**************************************************************************/
#ifndef __CONFIG_DLG_H__
#define __CONFIG_DLG_H__

namespace PluginFramework
{
	class PluginManager;
	class ServiceParam;
	class PluginInfo;	
	class IPlugin;
}

//! \brief Main dialog
class ConfigDlg : public CDialogEx
{
	enum { IDD = IDD_CONFIG_DIALOG };
public:
	enum eIconIndex
	{
		ICON_CREATE_LINK = 0U,
		ICON_COUNT
	};

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

	ConfigDlg(PluginFramework::PluginManager &PluginManager_in,
			  Windower::SettingsManager &SettingsManager_in,
			  CWnd* pParent_in = NULL);

	void InsertPlugin(CListCtrl *pListCtrl_in_out, const PluginFramework::PluginInfo &PluginInfo_in);

	bool OnTargetDrop(TCHAR **pDropPath_in);

	afx_msg void OnConfigure(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnProfileRename();
	afx_msg void OnProfilesChange();

	afx_msg void OnBnClickedTextLabels();
	afx_msg void OnAutoUpdateChange();
	afx_msg void OnDeleteProfile();
	afx_msg void OnVSyncChange();
	afx_msg void OnCreateLink();
	afx_msg void OnPathBrowse();
	afx_msg void OnPathChange();
	afx_msg void OnNewProfile();
	afx_msg void OnSave();

protected:
	void DoDataExchange(CDataExchange* pDX_in);
	// Generated message map functions
	BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

	void GenerateNewName(CString &NewName_in_out);
	void UpdateActivePlugins();
	void ActivateAllPlugins();		
	void RefreshPluginList();
	void OnCancel();
	void OnOK();

	PluginFramework::PluginManager &m_PluginManager;
	Windower::WindowerProfile *m_pCurrentSettings;
	Windower::SettingsManager &m_SettingsManager;
	CImageListButton m_LinkButton;
	EditLinkTarget m_GamePath;
	CImageList m_ImageList;
	int m_CurrentSel;
	HICON m_hIcon;	
};

#endif//__CONFIG_DLG_H__