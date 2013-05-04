/**************************************************************************
	created		:	2011-03-22
	filename	: 	VisualDialogImpl.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2012)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "VisualMgr.h"

#include "VisualDialog.h"
#include "VisualDialogImpl.h"
#include "VisualDialog.h"

/*! \brief VisualDialogImpl constructor
	\param[in] Dialog_in : a reference on the dialog using this implementation
*/
VisualDialogImpl::VisualDialogImpl(VisualDialog& Dialog_in)
	: CDialogImpl(Dialog_in), m_Dialog(Dialog_in), m_pFocusedCtrl(NULL),
	  m_nHitSysButton(0), m_nHotSysButton(0)
{
	m_nHitSysButton = HTNOWHERE;
	// load the colors through the visual manager
	VisualManager *pManager = VisualManager::GetInstance();

	if (pManager != NULL)
	{
		m_clrInactiveCaptionLight = pManager->GetDialogInactiveCaptionLightColor();
		m_clrInactiveCaptionDark = pManager->GetDialogInactiveCaptionDarkColor();
		m_clrActiveCaptionLight = pManager->GetDialogActiveCaptionLightColor();
		m_clrActiveCaptionDark = pManager->GetDialogActiveCaptionDarkColor();
		m_clrInactiveCaptionText = pManager->GetDialogInactiveCaptionTextColor();
		m_clrActiveCaptionText = pManager->GetDialogActiveCaptionTextColor();
		m_clrCaptionBorder = pManager->GetDialogCaptionBorderColor();
		m_clrBackground = pManager->GetDialogBackgroundColor();
		m_clrFrame = pManager->GetDialogFrameColor();
	}
	else
		ASSERT(FALSE);

	m_rectRedraw.SetRectEmpty();
}

//! VisualDialogImpl destructor
VisualDialogImpl::~VisualDialogImpl()
{
	while (m_lstCaptionSysButtons.IsEmpty() == FALSE)
		delete m_lstCaptionSysButtons.RemoveHead();
}

/*! \brief Member function called when a CWnd object is being activated or deactivated
	\param[in] nState_in : specifies whether the CWnd is being activated or deactivated
	\param[in] pWndOther_in : the CWnd being activated or deactivated
	\param[in] bMinimized_in : specifies the minimized state of the CWnd being activated or deactivated
	\return TRUE if the message was handled; FALSE otherwise
	\remarks this is required to prevent the focus from returning to the first control in the tab order
			 instead of the previously focused control when the dialog is re-activated
*/
BOOL VisualDialogImpl::OnActivate(UINT nState_in, CWnd* pWndOther_in, BOOL bMinimized_in)
{
	switch(nState_in)
	{
		// the window is activated, restore the focus to the saved control
		case WA_CLICKACTIVE:
		case WA_ACTIVE:
			if (m_pFocusedCtrl != NULL && ::IsWindow(m_pFocusedCtrl->GetSafeHwnd()))
				m_pFocusedCtrl->SetFocus();

			m_pFocusedCtrl = NULL;

			return TRUE;
		break;
		// the window is deactivated, save the control with the focus
		case WA_INACTIVE:
			m_pFocusedCtrl = CWnd::GetFocus();

			return TRUE;
		break;
	}

	return FALSE;
}

/*! \brief Retrieves a pointer to the caption button
		   depending on the specified mouse hit-test
	\param[in] nHitTest_in : the mouse hit-test
	\return a pointer to the caption buttons under the mouse 
			or NULL if the mouse isn't over any of the caption buttons
	\sa VisualDialogImpl::OnNcHitTest
*/
CMFCCaptionButtonEx* VisualDialogImpl::GetSysButton(UINT nHitTest_in)
{
	for (POSITION pos = m_lstCaptionSysButtons.GetHeadPosition(); pos != NULL;)
	{
		CMFCCaptionButtonEx* pButton = (CMFCCaptionButtonEx*)m_lstCaptionSysButtons.GetNext(pos);
		ASSERT_VALID(pButton);

		if (pButton->m_nHit == nHitTest_in)
			return pButton;
	}

	return NULL;
}

/*! \brief Keeps track of the caption buttons of the caption under the cursor
	\param[in] MousePos_in : Contains the x- and y-coordinates of the cursor
							 These coordinates are always screen coordinates
*/
void VisualDialogImpl::OnTrackCaptionButtons(CPoint MousePos_in)
{
	if (CMFCPopupMenu::GetActiveMenu() != NULL)
		return;

	// retrieve the caption buttons under the cursor when the left mouse button is pressed
	CMFCCaptionButtonEx* pBtn = GetSysButton(OnNcHitTest(MousePos_in));
	UINT nHot = m_nHotSysButton;

	// a button was found and it's enabled
	if (pBtn != NULL && pBtn->m_bEnabled)
	{
		// update the index of hot button
		m_nHotSysButton = pBtn->GetHit();
		pBtn->m_bFocused = TRUE;
		pBtn->m_bPushed = FALSE;
	}
	else
	{
		// the mouse is not over any of the caption buttons
		m_nHotSysButton = HTNOWHERE;
	}

	// if the highlighted button has changed
	if (m_nHotSysButton != nHot)
	{
		// retrieve the previously hot button
		CMFCCaptionButtonEx* pBtnOld = GetSysButton(nHot);

		if (pBtn != NULL && pBtn != pBtnOld)
		{
			// redraw the caption button currently hot (active style)
			RedrawCaptionButton(pBtn);
		}

		if (pBtnOld != NULL)
		{
			pBtnOld->m_bFocused = FALSE;
			// redraw it (inactive style)
			RedrawCaptionButton(pBtnOld);
		}
	}
}

/*! \brief Stops keeping track of the caption buttons of the caption under the cursor */
void VisualDialogImpl::StopCaptionButtonsTracking(void)
{
	// if a button was under the mouse while the left button was clicked
	if (m_nHitSysButton != HTNOWHERE)
	{
		// retrieve it
		CMFCCaptionButtonEx* pBtn = GetSysButton(m_nHitSysButton);
		m_nHitSysButton = HTNOWHERE;

		if (pBtn != NULL)
		{
			// redraw it
			pBtn->m_bPushed = FALSE;
			RedrawCaptionButton(pBtn);
		}
	}

	// if a button was hot when tracking stopped
	if (m_nHotSysButton != HTNOWHERE)
	{
		// retrieve it
		CMFCCaptionButtonEx* pBtn = GetSysButton(m_nHotSysButton);
		m_nHotSysButton = HTNOWHERE;

		if (pBtn != NULL)
		{
			// redraw it
			pBtn->m_bFocused = FALSE;
			RedrawCaptionButton(pBtn);
		}
	}
}

/*! \brief Redraw the area of a caption covered by a caption buttons
	\param[in] pSysBtn_in : a pointer to the caption buttons being redrawn
*/
void VisualDialogImpl::RedrawCaptionButton(CMFCCaptionButtonEx* pSysBtn_in)
{
	if (pSysBtn_in == NULL)
		return;

	ASSERT_VALID(pSysBtn_in);

	// redraw the area of the caption corresponding to the button client area
	m_rectRedraw = pSysBtn_in->GetRect();
	m_Dialog.SendMessage(WM_NCPAINT);
	m_rectRedraw.SetRectEmpty();
	// update the dialog
	m_Dialog.UpdateWindow();
}

/*! \brief Function called by the dialog for the CWnd object that contains the cursor 
				  (or the CWnd object that used the SetCapture member function 
				  to capture the mouse input) every time the mouse is moved
	\param[in] MousePos_in : Contains the x- and y-coordinates of the cursor
							 These coordinates are always screen coordinates
	\return the mouse hit-test value (winuser.h line 2251)
*/
UINT VisualDialogImpl::OnNcHitTest(CPoint MousePos_in)
{
	// if the visual manager doesn't implement owner-drawn captions
	// we don't have to keep track of the caption bar and its buttons
	if (IsOwnerDrawCaption() == FALSE)
		return HTNOWHERE;

	// convert the mouse position from screen to client coordinates
	m_Dialog.ScreenToClient(&MousePos_in);
	// initialize the area corresponding to the dialog frame
	const CSize szSystemBorder(::GetSystemMetrics(SM_CXSIZEFRAME),
							   ::GetSystemMetrics(SM_CYSIZEFRAME));
	// retrieve the height of the frame
	int cyOffset = szSystemBorder.cy;
	// if the dialog is not minimized
	if (m_Dialog.IsIconic() == FALSE)
	{
		// add the height of the caption
		cyOffset += ::GetSystemMetrics(SM_CYCAPTION);
	}
	// shift the mouse position based on the offset that was just computed
	MousePos_in.Offset(szSystemBorder.cx, cyOffset);

	// iterate through the caption buttons
	// and check if the mouse cursor is over any of them
	for (POSITION pos = m_lstCaptionSysButtons.GetHeadPosition(); pos != NULL;)
	{
		CMFCCaptionButtonEx* pButton = (CMFCCaptionButtonEx*)m_lstCaptionSysButtons.GetNext(pos);
		ASSERT_VALID(pButton);

		// if the current button is under the mouse, return its hit-test
		if (pButton->GetRect().PtInRect(MousePos_in))
			return pButton->m_nHit;
	}

	// check if the mouse is inside the caption area
	if (m_Dialog.m_CaptionRect.PtInRect(MousePos_in))
	{
		// check if the dialog has a system menu
		if ((m_Dialog.GetStyle() & WS_SYSMENU) == WS_SYSMENU)
		{
			// compute the area corresponding to the system menu
			CRect rectSysMenu = m_Dialog.m_CaptionRect;
			rectSysMenu.right = rectSysMenu.left + ::GetSystemMetrics(SM_CYSMCAPTION) + 2 * szSystemBorder.cx;
			// check if the mouse is inside it
			if (rectSysMenu.PtInRect(MousePos_in))
				return HTSYSMENU;
		}
		
		return HTCAPTION;
	}

	return HTNOWHERE;
}

/*! \brief Initializes the caption buttons depending on the style of the dialog
*/
void VisualDialogImpl::InitializeCaptionButtons(void)
{
	// fill the list of buttons if it's empty
	if (m_lstCaptionSysButtons.IsEmpty())
	{
		// retrieve the style of the dialog
		const DWORD dwStyle = m_Dialog.GetStyle();
		// retrieve the system menu
		CMenu* pSysMenu = m_Dialog.GetSystemMenu(FALSE);
		HMENU hSysMenu = NULL;

		// check if the system menu is valid
		if (pSysMenu != NULL && ::IsMenu(pSysMenu->m_hMenu))
		{
			hSysMenu = pSysMenu->GetSafeHmenu();

			if (::IsMenu(hSysMenu) == FALSE ||(dwStyle & WS_SYSMENU) == 0)
				hSysMenu = NULL;
		}

		if (hSysMenu != NULL)
		{
			// create the close button
			m_lstCaptionSysButtons.AddTail(new CMFCCaptionButtonEx(AFX_HTCLOSE));
			// if the dialog has the maximize style
			if ((dwStyle & WS_MAXIMIZEBOX) == WS_MAXIMIZEBOX)
			{
				// create the maximize button
				m_lstCaptionSysButtons.AddTail(new CMFCCaptionButtonEx(AFX_HTMAXBUTTON));
			}
			// if the dialog has the minimize style
			if ((dwStyle & WS_MINIMIZEBOX) == WS_MINIMIZEBOX)
			{
				// create the minimize button
				m_lstCaptionSysButtons.AddTail(new CMFCCaptionButtonEx(AFX_HTMINBUTTON));
			}
		}
	}
	// retrieve the size of the buttons from the visual manager
	CSize sizeButton = CMFCVisualManager::GetInstance()->GetNcBtnSize(TRUE);
	CRect CaptionRect =  m_Dialog.m_CaptionRect;
	// make sure the button fits in the caption bar
	sizeButton.cy = min(sizeButton.cy, CaptionRect.Height() - 2);
	sizeButton.cx++;
	// compute the position of the last button
	int x = CaptionRect.right - sizeButton.cx - 8;
	int y = CaptionRect.top + max(0, (CaptionRect.Height() - sizeButton.cy) / 2);
	// iterate through the caption buttons
	for (POSITION pos = m_lstCaptionSysButtons.GetHeadPosition(); pos != NULL;)
	{
		// retrieve the current button
		CMFCCaptionButtonEx* pButton = (CMFCCaptionButtonEx*)m_lstCaptionSysButtons.GetNext(pos);
		ASSERT_VALID(pButton);
		// set its client area
		pButton->SetRect(CRect(CPoint(x, y), sizeButton));
		// move to the position of the next button
		x -= sizeButton.cx;
	}
}

//! \brief Function called by the dialog when the non-client area needs to be painted
BOOL VisualDialogImpl::OnNcPaint(void)
{	
	CRect DrawCaptionRect(m_Dialog.m_CaptionRect);
	CWindowDC WindowDC(&m_Dialog);

	CDrawingManager dm(WindowDC);
	COLORREF LightColor, DarkColor;
	CRect ExcludeRect(m_Dialog.m_ClientRect);
	CRect DrawFrameRect(m_Dialog.m_FrameRect);

	// exclude the client area from drawing when drawing the non-client area
	ExcludeRect.DeflateRect(1, m_Dialog.m_CaptionRect.bottom, -1, -m_Dialog.m_CaptionRect.bottom);
	WindowDC.ExcludeClipRect(ExcludeRect);

	// transparent background
	WindowDC.SetBkMode(TRANSPARENT);
	// colors for the caption gradient
	if (m_Dialog.IsActive())
	{
		LightColor = m_clrActiveCaptionLight;
		DarkColor = m_clrActiveCaptionDark;
	}
	else
	{
		LightColor = m_clrInactiveCaptionLight;
		DarkColor = m_clrInactiveCaptionDark;
	}
	// draw the dialog frame
	dm.DrawRect(DrawFrameRect, -1, m_clrFrame);
	// draw the caption border
	dm.DrawRect(DrawCaptionRect, -1, m_clrCaptionBorder);
	DrawCaptionRect.DeflateRect(1, 1, 1, 1);
	// draw the caption gradient
	dm.FillGradient(DrawCaptionRect, DarkColor, LightColor);
	// draw the icon
	if (m_Dialog.m_hIcon != NULL)
	{
		CSize szIcon(::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		CRect rectIcon(0, 0, DrawCaptionRect.bottom, DrawCaptionRect.bottom);

		long x = DrawCaptionRect.left + max(0, (rectIcon.Width()  - szIcon.cx) / 2);
		long y = DrawCaptionRect.top  + max(0, (rectIcon.Height() - szIcon.cy) / 2);

		if (m_Dialog.m_IconBackground != -1 || m_Dialog.m_IconBorder != -1)
		{
			CRect BackgroundRect(x - 1, y - 1, x + szIcon.cx + 1, y + szIcon.cy + 1);

			if (m_Dialog.m_IconBorder != -1)
				BackgroundRect.InflateRect(1, 1, 1, 1);

			dm.DrawRect(BackgroundRect, m_Dialog.m_IconBackground, m_Dialog.m_IconBorder);
		}

		::DrawIconEx(WindowDC.GetSafeHdc(), x, y, m_Dialog.m_hIcon, szIcon.cx, szIcon.cy, 0, NULL, DI_NORMAL);

		DrawCaptionRect.left += rectIcon.Width();
	}
	else
		DrawCaptionRect.left += 8;

	// draw the dialog title
	DrawNcText(&WindowDC, DrawCaptionRect, m_Dialog.m_DialogTitle, TRUE);
	// draw the caption buttons
	int xButtonsRight = DrawCaptionRect.right;
	const DWORD dwStyle = m_Dialog.GetStyle();
	CSize szSysBorder(1, 1);
	BOOL bMaximized = (dwStyle & WS_MAXIMIZE) == WS_MAXIMIZE;

	// iterate through the caption buttons
	for (POSITION pos = m_lstCaptionSysButtons.GetHeadPosition(); pos != NULL;)
	{
		// retrieve the current button
		CMFCCaptionButtonEx* pButton = (CMFCCaptionButtonEx*)m_lstCaptionSysButtons.GetNext(pos);
		ASSERT_VALID(pButton);
		CMFCVisualManager::AFX_BUTTON_STATE State;
		UINT HitTest = pButton->GetHit();
		UINT nButton = 0;

		// retrieve its ButtonState_in
		if (pButton->m_bPushed && pButton->m_bFocused)
			State = CMFCVisualManager::ButtonsIsPressed;
		else if (pButton->m_bFocused)
			State = CMFCVisualManager::ButtonsIsHighlighted;
		else
			State = CMFCVisualManager::ButtonsIsRegular;
		// retrieve the index of the button from its hit-test
		switch(HitTest)
		{
			// close button
			case AFX_HTCLOSE:
				nButton = SC_CLOSE;
			break;
			// maximize button
			case AFX_HTMAXBUTTON:
				nButton = (dwStyle & WS_MAXIMIZE) == WS_MAXIMIZE ? SC_RESTORE : SC_MAXIMIZE;
			break;
			// minimize button
			case AFX_HTMINBUTTON:
				nButton = (dwStyle & WS_MINIMIZE) == WS_MINIMIZE ? SC_RESTORE : SC_MINIMIZE;
			break;
		}
		// retrieve the client area of the button
		CRect rectBtn(pButton->GetRect());
		// resize it depending on whether the dialog is maximized or not
		if (bMaximized)
			rectBtn.OffsetRect(szSysBorder.cx, szSysBorder.cy);
		else
			rectBtn.OffsetRect(0, -szSysBorder.cy - 1);
		// draw the current button
		DrawNcBtn(&WindowDC, rectBtn, nButton, State, FALSE, m_Dialog.m_bIsActive, FALSE);
		// move to the position of the next button
		xButtonsRight = min(xButtonsRight, pButton->GetRect().left);
	}

	m_Dialog.ReleaseDC(&WindowDC);

	return TRUE;
}

/*! \brief Function called by the dialog when the mouse cursor moves
	\param[in] nHitTest_in : Indicates whether various virtual keys are down
	\param[in] MousePos_in : Contains the x- and y-coordinates of the cursor
							 These coordinates are always screen coordinates
*/
void VisualDialogImpl::OnNcMouseMove(UINT, CPoint MousePos_in)
{
	// if the visual manager doesn't implement owner-drawn captions
	// we don't have to keep track of the caption bar and its buttons
	if (IsOwnerDrawCaption())
	{
		// keep track of the caption buttons when the mouse moves in the caption area
		OnTrackCaptionButtons(MousePos_in);
	}
}

/*! \brief Function called by the dialog when the cursor is moved within a non-client area
		   left mouse button while the cursor is within a non-client area
	\param[in] nHitTest_in : the mouse hit-test value
	\param[in] MousePos_in : Contains the x- and y-coordinates of the cursor
							 These coordinates are always screen coordinates
*/
void VisualDialogImpl::OnMouseMove(CPoint MousePos_in)
{
	// if the visual manager doesn't implement owner-drawn captions
	// we don't have to keep track of the caption bar and its buttons
	if (IsOwnerDrawCaption())
	{
		CPoint ptScreen = MousePos_in;
		// keep track of the caption buttons when the mouse moves
		m_Dialog.ClientToScreen(&ptScreen);
		OnTrackCaptionButtons(ptScreen);
	}
}

/*! \brief Member function called when the user presses the left mouse button
	\param[in] unused
*/
void VisualDialogImpl::OnLButtonDown(CPoint)
{
	if (m_nHotSysButton == HTNOWHERE)
		return;

	CMFCCaptionButtonEx* pBtn = GetSysButton(m_nHotSysButton);

	if (pBtn != NULL)
	{
		m_nHitSysButton = m_nHotSysButton;
		pBtn->m_bPushed = TRUE;
		RedrawCaptionButton(pBtn);
	}
}

/*! \brief Member function called when the user presses the left mouse button
	\param[in] MousePos_in : Contains the x- and y-coordinates of the cursor
							 These coordinates are always screen coordinates
*/
void VisualDialogImpl::OnLButtonUp(CPoint MousePos_in)
{
	// if the visual manager doesn't implement owner-drawn captions
	// we don't have to keep track of the caption bar and its buttons
	if (IsOwnerDrawCaption())
	{
		// check if a button was clicked
		switch (m_nHitSysButton)
		{
			case AFX_HTCLOSE:
			case AFX_HTMAXBUTTON:
			case AFX_HTMINBUTTON:
			{
				// save the old hot & hit buttons
				UINT nHot = m_nHotSysButton;
				UINT nHit = m_nHitSysButton;
				// if the hot and hit buttons are the same
				if (nHot == nHit)
				{
					UINT nSysCmd = 0;
					// retrieve the command from the hit-test
					switch (nHot)
					{
						case AFX_HTCLOSE:
							nSysCmd = SC_CLOSE;
						break;

						case AFX_HTMAXBUTTON:
							nSysCmd = (m_Dialog.GetStyle() & WS_MAXIMIZE) == WS_MAXIMIZE ? SC_RESTORE : SC_MAXIMIZE;
						break;

						case AFX_HTMINBUTTON:
							nSysCmd = m_Dialog.IsIconic() ? SC_RESTORE : SC_MINIMIZE;
						break;
					}
					// invoke the system command
					m_Dialog.PostMessage(WM_SYSCOMMAND, nSysCmd);
				}
			}
		}
		// stop tracking the ButtonState_in of the caption buttons
		StopCaptionButtonsTracking();
	}
}

/*! \brief Function called by the dialog when the dialog background needs erasing (for example, when resized)
	\param[in] pDC_in_out : a pointer to a device-context object
	\return TRUE (non-zero) if the background was erased; FALSE otherwise
*/
BOOL VisualDialogImpl::OnEraseBkgnd(CDC* pDC_in_out, const CRect &EraseRect_in)
{
	CDrawingManager dm(*pDC_in_out);

	dm.DrawRect(EraseRect_in, m_clrBackground, m_clrBackground);

	return TRUE;
}

/*! \brief Function called by the dialog when the size and position of the client area needs to be calculated
	\param[in] bCalcValidRects_in : Specifies whether the application should specify which part of the client
									area contains valid information Windows will copy the valid information 
									to the specified area within the new client area. If this parameter is TRUE,
									the application should specify which part of the client area is valid.
	\param[in] pCalcSizeParams_in : pointer to a NCCALCSIZE_PARAMS data structure that contains information 
									used to calculate the new size and position of the CWnd rectangle
									(including client area, borders, caption, scroll bars, and so on).
*/
BOOL VisualDialogImpl::OnNcCalcSize(BOOL, NCCALCSIZE_PARAMS FAR* pCalcSizeParams)
{
	ASSERT(pCalcSizeParams != NULL);
	BOOL Result;

	if (m_Dialog.IsWindowVisible() == FALSE && afxGlobalData.DwmIsCompositionEnabled())
	{
		pCalcSizeParams->rgrc[0].bottom -= GetSystemMetrics(SM_CYSIZEFRAME);
		pCalcSizeParams->rgrc[0].left += GetSystemMetrics(SM_CYSIZEFRAME);
		pCalcSizeParams->rgrc[0].right -= GetSystemMetrics(SM_CXSIZEFRAME);

		Result = TRUE;
	}
	else
	{
		pCalcSizeParams->rgrc[0].top += m_Dialog.m_FrameHeight + 4;
		pCalcSizeParams->rgrc[0].left -= m_Dialog.m_FrameWidth - 1;
		pCalcSizeParams->rgrc[0].right += m_Dialog.m_FrameWidth + 1;
		pCalcSizeParams->rgrc[0].bottom += m_Dialog.m_FrameHeight + 1;

		Result = ((m_Dialog.GetStyle() & WS_MAXIMIZE) == WS_MAXIMIZE);
	}

	m_Dialog.UpdateWindowRect();

	return Result;
}

/*! \brief Draws the title of the dialog in the caption
	\param[in] pDC_in_out : a pointer to the display context
	\param[in] Rect_in : the size of the caption
	\param[in] DialogTitle_in : the title of the dialog
	\param[in] bActive_in : active state of the caption bar
	\param[in] bTextCenter_in : flag specifying if the text is centered
	\param[in] bIsRTL_in : flag specifying if the text is right to left (Hebrew, etc.)
*/
void VisualDialogImpl::DrawNcText(CDC *pDC_in_out, CRect& Rect_in, const CString& DialogTitle_in,
								BOOL bActive_in, BOOL bTextCenter_in, BOOL bIsRTL_in)
{
	// validate the parameters
	if (DialogTitle_in.IsEmpty() == false && Rect_in.right > Rect_in.left)
	{
		ASSERT_VALID(pDC_in_out);
		// select the font of the dialog
		HFONT OldFont = (HFONT)pDC_in_out->SelectObject(m_Dialog.m_TitleFont.GetSafeHandle());
		// set the style of the text
		DWORD dwTextStyle = DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER | (bIsRTL_in ? DT_RTLREADING : 0);
		// compute the position and size of the title within the caption
		int widthFull = Rect_in.Width();
		int width = pDC_in_out->GetTextExtent(DialogTitle_in).cx;
		// get the x coordinate of the title depending on whether the text is centered or not
		if (bTextCenter_in && width < widthFull)
			Rect_in.left += (widthFull - width) / 2;
		// compute the width of the rectangle
		Rect_in.right = min(Rect_in.left + width + 8, Rect_in.right);
		// validate the title rectangle
		if (Rect_in.right > Rect_in.left)
		{
			// set the background as transparent
			int nOldMode = pDC_in_out->SetBkMode(TRANSPARENT);
			// save the previous color of the drawing context
			COLORREF clrOldText = pDC_in_out->GetTextColor();
			// set the color of the text
			COLORREF clrText = bActive_in ? m_clrActiveCaptionText : m_clrInactiveCaptionText;

			pDC_in_out->SetTextColor(clrText);
			// draw the text
			pDC_in_out->DrawText(DialogTitle_in, Rect_in, dwTextStyle);
			// restore the previous text color
			pDC_in_out->SetTextColor(clrOldText);
			// restore the previous background color
			pDC_in_out->SetBkMode(nOldMode);
		}
		// restore the previous font
		pDC_in_out->SelectObject(OldFont);
	}
}

/*! \brief Draws the icon of a button given its index, state and size
	\param[in] pDC_in_out : a pointer to the display context
	\param[in] Rect_in : the size of the button
	\param[in] nButton_in : the index of the button
	\param[in] ButtonState_in : the state of the button
	\param[in] bSmallButtons_in : the size of the button
	\param[in] bActive_in : the active state of the button
	\param[in] unused for dialogs
*/
void VisualDialogImpl::DrawNcBtn(CDC* pDC_in_out, const CRect& Rect_in, UINT nButton_in,
							   CMFCVisualManager::AFX_BUTTON_STATE ButtonState_in,
							   BOOL bSmallButtons_in, BOOL bActive_in, BOOL)
{
	ASSERT_VALID(pDC_in_out);

	CMFCToolBarImages *pImage = NULL;

	// retrieve the index of the button icon
	int nIndex = bSmallButtons_in ? 1 : 0;

	if (nButton_in == SC_CLOSE)
		pImage = VisualManager::GetInstance()->GetCloseButtonImages() + nIndex;
	else if (nButton_in == SC_MINIMIZE)
		pImage = VisualManager::GetInstance()->GetMinimizeButtonImages() + nIndex;
	else if (nButton_in == SC_MAXIMIZE)
		pImage = VisualManager::GetInstance()->GetMaximizeButtonImages() + nIndex;
	else if (nButton_in == SC_RESTORE)
		pImage = VisualManager::GetInstance()->GetRestoreButtonImages() + nIndex;
	else
		return;
	// retrieve the alignments of the icon for stretching
	CMFCToolBarImages::ImageAlignHorz horz = (afxGlobalData.GetRibbonImageScale() != 1.)
										   ? CMFCToolBarImages::ImageAlignHorzStretch
										   : CMFCToolBarImages::ImageAlignHorzCenter;
	CMFCToolBarImages::ImageAlignVert vert = (afxGlobalData.GetRibbonImageScale() != 1.)
										   ? CMFCToolBarImages::ImageAlignVertStretch
										   : CMFCToolBarImages::ImageAlignVertCenter;
	// retrieve the size of the icon
	CRect rtBtnImage(CPoint(0, 0), pImage->GetImageSize());
	// shift the icon area depending on its active and highlighted state
	if (bActive_in == FALSE)
		rtBtnImage.OffsetRect(0, pImage->GetImageSize().cy * 3);
	else if (ButtonState_in != VisualManager::ButtonsIsRegular)
		rtBtnImage.OffsetRect(0, pImage->GetImageSize().cy * (ButtonState_in == VisualManager::ButtonsIsHighlighted ? 1 : 2));
	// draw the icon
	pImage->DrawEx(pDC_in_out, Rect_in, 0, horz, vert, rtBtnImage);
}
