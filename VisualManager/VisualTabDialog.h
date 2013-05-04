/**************************************************************************
	created		:	2011-07-07
	filename	: 	VisualTabDialog.h
	author		:	Xebeth`
	copyright	:	North Edge (2012)
	purpose		:	Base class for tabbed dialogs
**************************************************************************/
#ifndef _VISUAL_MANAGER_SDK_INCLUDE_
	#error Please include the global header file 'VisualManager.h'
#endif

#ifndef __VISUAL_TAB_DIALOG_H__
#define __VISUAL_TAB_DIALOG_H__

// declares the button handling method for a range of tab headers resource IDs
// the resource IDs of the tab headers for a dialog must be continuous
#define TAB_HEADERS_HANDLER(FirstID, LastID) ON_CONTROL_RANGE(BN_CLICKED, FirstID, LastID, &VisualTabDialog::OnBnClicked)

class VisualTabCtrl;
class TabPage;

/*! \brief Base class for tabbed dialogs */
class APPVIMGR_API VisualTabDialog : public VisualDialog
{
	// MFC runtime class macro
	DECLARE_DYNAMIC(VisualTabDialog)
public:
	VisualTabDialog(UINT FirstTabHeaderID_in, UINT LastTabHeaderID_in,
					UINT FirstTabPageID_in, UINT LastTabPageID_in,  
					UINT TemplateID_in, CWnd *pParentWnd_in = NULL);
	virtual ~VisualTabDialog();

	/*! \brief Creates a page in a tab control at the specified index
		\param[in] pTabCtrl_in : the tab control
		\param[in] TabIndex_in : the tab index
		\return a pointer to the new page
	*/
	virtual TabPage* CreatePage(VisualTabCtrl *pTabCtrl_in, UINT TabIndex_in) =0;
	virtual BOOL OnInitDialog();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX_in_out);
	afx_msg virtual void OnBnClicked(UINT nID_in);

	//! Pointer to the tab control of the dialog
	VisualTabCtrl* m_pTabCtrl;
	//! The resource ID of the first tab header
	UINT m_FirstTabHeaderResID;
	//! The resource ID of the last tab header
	UINT m_LastTabHeaderResID;
	//! The resource ID of the first tab page
	UINT m_FirstTabPageResID;
	//! The resource ID of the last tab page
	UINT m_LastTabPageResID;
	//! Number of tabs in the dialog
	UINT m_TabCount;
	//! Container
	CRect m_ContainerRect;
	//! Header size
	CSize m_HeaderSize;
};

#endif//__VISUAL_TAB_DIALOG_H__