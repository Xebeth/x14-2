/**************************************************************************
	created		:	2011-07-08
	filename	: 	VisualTab.h
	author		:	Xebeth`
	copyright	:	North Edge (2012)
	purpose		:	Container class for Visual tabs (TabHeader + TabPage)
**************************************************************************/
#ifndef _VISUAL_MANAGER_SDK_INCLUDE_
	#error Please include the global header file 'VisualManager.h'
#endif

#ifndef __VISUAL_TAB_H__
#define __VISUAL_TAB_H__

class TabHeader;
class TabPage;

/*! \brief Container class for Visual tabs */
class APPVIMGR_API VisualTab
{
public:
	VisualTab(UINT TabIndex_in, UINT TabHeaderID_in, UINT TabPageID_in);
	~VisualTab();

	TabHeader* GetHeader(void) const { return m_pTabHeader; }
	TabPage* GetPage(void) const { return m_pTabPage; }

	void SetHeader(TabHeader *pHeader_in) { m_pTabHeader = pHeader_in; }
	void SetPage(TabPage *pPage_in) { m_pTabPage = pPage_in; }

	UINT GetHeaderResID(void) const { return m_HeaderResID; }
	UINT GetPageResID(void) const { return m_PageResID; }
	UINT GetTabIndex(void) const { return m_TabIndex; }

	void UpdateSize(void);

protected:
	//! Pointer to the tab header
	TabHeader *m_pTabHeader;
	//! Resource ID of the tab header
	UINT m_HeaderResID;
	//! Pointer to the tab page
	TabPage *m_pTabPage;
	//! Resource ID of the tab page
	UINT m_PageResID;
	//! Index of the tab
	UINT m_TabIndex;
};

#endif//__VISUAL_TAB_H__