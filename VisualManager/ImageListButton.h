/**************************************************************************
	created		:	2013-05-03
	filename	: 	ImageListButton.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef _VISUAL_MANAGER_SDK_INCLUDE_
	#error Please include the global header file 'VisualManager.h'
#endif

#ifndef __IMAGE_BUTTON_H__
#define __IMAGE_BUTTON_H__

/*! \brief Button class to overload CButton drawing */
class APPVIMGR_API CImageListButton : public CButton
{
	DECLARE_DYNAMIC(CImageListButton)
public:
	CImageListButton();
	~CImageListButton();

	void SetImageList(CImageList *pImageList_in);
	void SetTooltip(const CString &Text_in);
	void SetImage(int Index_in);

	virtual BOOL PreTranslateMessage(MSG* pMsg_in);

protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct_in);
	virtual void PreSubclassWindow();

	void KillFocus(void);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd *pNewWnd);
	afx_msg void OnMouseLeave(void);	

	// button specific messages
	LRESULT OnSetImage(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetStyle(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetCheck(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	//! original style of the button
	UINT m_OriginalStyle;
	//! flag specifying if the button has the flat style
	bool m_bFlat;
	//! flag specifying if the button is a checkbox
	bool m_bCheckbox;
	//! flag specifying if the button is a radio button
	bool m_bRadio;
	//! flag specifying if the button is a plain button
	bool m_bButton;
	//! flag specifying if the button is the default one
	bool m_bDefault;
	//! flag specifying if the button is disabled
	bool m_bDisabled;
	//! flag specifying if the button is pressed
	bool m_bPressed;
	//! flag specifying if the button is checked
	bool m_bChecked;
	//! flag specifying if the button is highlighted
	bool m_bHighlight;
	//! Image list used to draw the image
	CImageList *m_pImageList;
	//! Index of the current image
	int m_ImageIndex;
	//! Rectangle of the current image
	CRect m_ImageRect;
	//! the background color of the button when disabled
	CBrush *m_pDisabledBrush;
	//! the background color of the dialog
	CBrush *m_pBackgroundBrush;
	//! Tooltip control
	CToolTipCtrl* m_pToolTip;
};

#endif//__IMAGE_BUTTON_H__