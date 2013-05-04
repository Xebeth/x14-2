/**************************************************************************
	created		:	2011-07-08
	filename	: 	TabHeader.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2012)
	purpose		:	Tab header for tabbed dialogs
**************************************************************************/
#include "stdafx.h"
#include "TabHeader.h"
#include "VisualMgr.h"

IMPLEMENT_DYNAMIC(TabHeader, CMFCButton)
BEGIN_MESSAGE_MAP(TabHeader, CMFCButton)
END_MESSAGE_MAP()

//! \brief TabHeader default constructor
TabHeader::TabHeader() : CMFCButton()
{
	// retrieve the color from the visual manager

	// load the colors through the visual manager
	VisualManager *pManager = VisualManager::GetInstance();

	if (pManager != NULL)
	{
		m_InactiveBackground = pManager->GetTabDialogHeaderInactiveBackgroundColor();
		m_ActiveBackground = pManager->GetTabDialogHeaderActiveBackgroundColor();
		m_PushedBackground = pManager->GetTabDialogHeaderPushedBackgroundColor();
		m_InactiveBorder = pManager->GetTabDialogHeaderInactiveBackgroundColor();
		m_ActiveBorder = pManager->GetTabDialogHeaderActiveBackgroundColor();
		m_PushedBorder = pManager->GetTabDialogHeaderPushedBorderColor();
		m_Arrow = pManager->GetTabDialogHeaderArrowColor();
	}
	else
		ASSERT(FALSE);

	// make the button a checkbox so that only one tab is pushed at a time
	m_bRadioButton = TRUE;
	m_bAutoCheck = TRUE;
}

/*! \brief Member function called when the visual aspect of an owner-drawn button has changed
	\param[in] lpDrawItemStruct_in : pointer to a DRAWITEMSTRUCT structure containing 
									 information about the item to be drawn
*/
void TabHeader::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct_in)
{
	ASSERT(lpDrawItemStruct_in != NULL);
	ASSERT(lpDrawItemStruct_in->CtlType == ODT_BUTTON);
	// retrieve the drawing context
	CDC* pDCPaint = CDC::FromHandle(lpDrawItemStruct_in->hDC);
	ASSERT_VALID(pDCPaint);
	CFont *pOldFont = NULL, *pSelectedFont = &m_InactiveFont;
	CRect rectDraw(lpDrawItemStruct_in->rcItem);
	CRect rectText(lpDrawItemStruct_in->rcItem);
	COLORREF BackgroundColor, BorderColor;
	DWORD dwDrawFlags = DT_WORDBREAK;
	CMemDC memDC(*pDCPaint, this);
	CDC* pDC = &memDC.GetDC();
	CDrawingManager dm(*pDC);
	bool bCentered = false;

	//   IsPressed()					 IsPushed()   IsChecked()
	if ((m_bPushed && m_bHighlighted) || m_bPushed || m_bChecked)
	{
		BackgroundColor = m_PushedBackground;
		BorderColor = m_PushedBorder;

		// if the button is pressed its text is centered and bold
		if (m_bChecked)
		{
			bCentered = true;
			rectText.left = 30;
			pSelectedFont = &m_ActiveFont;
		}
		else
			rectText.left = 10;
	}
	else 
	{
		BackgroundColor = m_InactiveBackground;
		BorderColor = m_InactiveBorder;
		rectText.left = 10;
	}
	// select the font for drawing
	pOldFont = pDC->SelectObject(pSelectedFont);
	// draw the border of the button
	dm.DrawRect(rectDraw, BackgroundColor, BorderColor);
	// draw a line with the color of the pushed background
	dm.DrawLine(rectDraw.right - 1, rectDraw.top + 1, rectDraw.right - 1,
			    rectDraw.bottom - 2, m_PushedBackground);
	// set the background color as transparent
	pDC->SetBkMode(TRANSPARENT);
	// retrieve the text of the button
	CString strTitle;
	GetWindowText(strTitle);
	// if the button is highlighted
	if (m_bHighlighted)
	{
		CRect rectHighlight(lpDrawItemStruct_in->rcItem);
		// use the drawing manager to lighten its color
		rectHighlight.DeflateRect(0, 0, 1, 0);
		dm.HighlightRect(rectHighlight);
	}
	// place the text to be horizontally centered
	rectText.top += (LONG)((rectText.Height() - 14) * 0.5) + 1;
	// draw the text
	pDC->DrawText(strTitle, rectText, dwDrawFlags);
	// draw an arrow if the text is centered
	if (bCentered)
	{
		// retrieve the size of the text in pixels
		CSize TextRect = pDC->GetTextExtent(strTitle);
		// compute the position of the arrow
		int x = rectText.left + TextRect.cx + 5;
		int y = rectText.top + 5;
		// draw the arrow
		dm.DrawLine(++x, y, x, y + 6, m_Arrow);
		dm.DrawLine(++x, y + 1, x, y + 5, m_Arrow);
		dm.DrawLine(++x, y + 2, x, y + 4, m_Arrow);
		pDC->SetPixel(++x, y + 3, m_Arrow);
	}
}


/*! \brief Sets the font used to draw the text of the item
	\param[in] pFont_in : the new font
*/
void TabHeader::InitFont(CFont* pFont_in)
{
	if (pFont_in != NULL)
	{
		LOGFONT FontLog;

		// retrieve the font of the parent window
		pFont_in->GetLogFont(&FontLog);
		// set the inactive text font
		FontLog.lfHeight = 14L;
		m_InactiveFont.CreateFontIndirect(&FontLog);
		// set the active text font
		FontLog.lfWeight = FW_BOLD;
		m_ActiveFont.CreateFontIndirect(&FontLog);
	}
}