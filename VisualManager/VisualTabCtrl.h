/**************************************************************************
	created		:	2011-07-08
	filename	: 	VisualTabCtrl.h
	author		:	Xebeth`
	copyright	:	North Edge (2012)
	purpose		:	Visual tab control
**************************************************************************/
#ifndef _VISUAL_MANAGER_SDK_INCLUDE_
	#error Please include the global header file 'VisualManager.h'
#endif

#ifndef __VISUAL_TAB_CTRL_H__
#define __VISUAL_TAB_CTRL_H__

class VisualTabDialog;
class TabHeader;
class VisualTab;
class TabPage;

/*! \brief Visual tab control */
class APPVIMGR_API VisualTabCtrl
{
	friend class VisualTabDialog;
public:
	/*! map of tab headers 
		key:	zero-based tab index
		value:	pointer to a tab object
	*/
	typedef std::map<UINT, VisualTab*> Tabs;

	explicit VisualTabCtrl(VisualTabDialog *pParent_in);
	virtual ~VisualTabCtrl();

	const VisualTab* GetTab(UINT TabIndex_in) const;
	bool SetPage(UINT TabIndex_in, TabPage *pPage_in);
	/*! \brief Retrieves the current tab object
		\return a pointer to the current tab object
	*/
	const VisualTab* GetActiveTab(void) const { return m_pCurrentTab; }
	UINT GetActiveTabIndex(void) const;
	bool SetActiveTabByHeaderResID(UINT HeaderResID_in);
	bool SetActiveTab(UINT TabIndex_in);

	bool InsertTab(UINT TabIndex_in, UINT TabHeaderID_in, UINT TabPageID_in, const TCHAR *pHeaderText_in = NULL);
	bool AddTab(UINT TabHeaderID_in, UINT PageHeaderID_in, const TCHAR *pHeaderText_in);
	bool RemoveTab(UINT TabIndex_in);

	bool UpdateTabText(UINT TabIndex_in, const CString &Text_in);

	/*! \brief Retrieves the number of tabs in the control
		\return the number of tabs in the control
	*/
	UINT GetTabCount(void) const { return m_TabCount; }

	void ResizeHeaders(UINT Height_in);

protected:
	bool CreateHeaderWindow(TabHeader *pHeader_in, UINT TabHeaderID_in, const TCHAR * pHeaderText_in);
	UINT GetIndexFromHeaderResID(UINT HeaderResID_in);
	VisualTab* TabAt(UINT TabIndex_in) const;
	TabPage* GetPage(UINT TabIndex_in);
	/*! \brief Retrieves the tabs of the control
		\return a const reference on the map of tabs
	*/
	const Tabs& GetTabs(void) { return m_Tabs; }

	//! Index of the tab currently selected
	UINT m_CurrentTabIndex;
	//! Current tab
	VisualTab *m_pCurrentTab;
	//! Area of the page content
	CRect m_ContainerRect;
	//! Default size for the header
	CSize m_HeaderSize;
	//! Associative array containing the tabs of the dialog
	Tabs m_Tabs;
	//! Number of tabs in the control
	UINT m_TabCount;
	//! Parent dialog of the control
	VisualTabDialog *m_pParentWnd;
};

#endif//__VISUAL_TAB_CTRL_H__