/**************************************************************************
	created		:	2011-03-22
	filename	: 	VisualDialogImpl.h
	author		:	Xebeth`
	copyright	:	North Edge (2012)
	purpose		:	Implementation of the VisualDialog
**************************************************************************/
#ifndef _VISUAL_MANAGER_SDK_INCLUDE_
	#error Please include the global header file 'VisualManager.h'
#endif

#ifndef __VISUAL_DIALOG_IMPL_H__
#define __VISUAL_DIALOG_IMPL_H__

/*! \brief Implementation of the VisualDialog */
class VisualDialogImpl : public CDialogImpl
{
	// VisualDialogImpl is tightly coupled with its dialog class
	friend class VisualDialog;
public:
	explicit VisualDialogImpl(VisualDialog &Dialog_in);
	~VisualDialogImpl();

protected:

	/*! \brief Checks the current visual manager implements owner-drawn captions
		\return TRUE if all the frame windows in the application have owner-draw captions; FALSE otherwise
	*/
	BOOL IsOwnerDrawCaption(void) { return CMFCVisualManager::GetInstance()->IsOwnerDrawCaption(); }
	void DrawNcText(CDC *pDC_in_out, CRect& Rect_in, const CString& DialogTitle_in, BOOL bActive_in,
					BOOL bCenterText_in = FALSE, BOOL bIsRTL_in = FALSE);
	void DrawNcBtn(CDC* pDC_in_out, const CRect& Rect_in, UINT nButton_in,
				   CMFCVisualManager::AFX_BUTTON_STATE ButtonState_in,
				   BOOL bSmallButtons_in, BOOL bActive_in, BOOL bMDI_in = FALSE);
	BOOL OnNcCalcSize(BOOL bCalcValidRects_in, NCCALCSIZE_PARAMS FAR* pCalcSizeParams_in);
	BOOL OnActivate(UINT nState_in, CWnd* pWndOther_in, BOOL bMinimized_in);
	BOOL OnEraseBkgnd(CDC* pDC_in_out, const CRect &EraseRect_in);
	void OnNcMouseMove(UINT nHitTest_in, CPoint MousePos_in);
	UINT OnNcHitTest(CPoint MousePos_in);
	void OnMouseMove(CPoint MousePos_in);
	void OnLButtonDown(CPoint MousePos_in);
	void OnLButtonUp(CPoint MousePos_in);
	BOOL OnNcPaint(void);

	CMFCCaptionButtonEx* GetSysButton(UINT nHitTest_in);

	void OnTrackCaptionButtons(CPoint MousePos_in);
	void StopCaptionButtonsTracking(void);
	void RedrawCaptionButton(CMFCCaptionButtonEx* pBtn_in);
	void InitializeCaptionButtons(void);

	//! light gradient color of the inactive caption
	COLORREF m_clrInactiveCaptionLight;
	//! dark gradient color of the inactive caption
	COLORREF m_clrInactiveCaptionDark;
	//! light gradient color of the active caption
	COLORREF m_clrActiveCaptionLight;
	//! dark gradient color of the active caption
	COLORREF m_clrActiveCaptionDark;
	//! color of the text of the active caption 
	COLORREF m_clrActiveCaptionText;
	//! color of the text of the inactive caption 
	COLORREF m_clrInactiveCaptionText;
	//! color of the caption border
	COLORREF m_clrCaptionBorder;
	//! color of the background
	COLORREF m_clrBackground;
	//! color of the frame
	COLORREF m_clrFrame;
	//! a list of caption system buttons (maximize, minimize, close, etc.)
	CObList  m_lstCaptionSysButtons;
	//! index of the system button under the mouse cursor when it's pressed (hot)
	UINT m_nHotSysButton;
	//! index of the system button under the mouse cursor when it's released (hit)
	UINT m_nHitSysButton;
	//! area of the caption that needs to be redrawn
	CRect m_rectRedraw;
	//! a reference on the dialog using this implementation
	VisualDialog &m_Dialog;
	//! pointer to the control with the focus
	CWnd *m_pFocusedCtrl;
};

#endif//__VISUAL_DIALOG_IMPL_H__