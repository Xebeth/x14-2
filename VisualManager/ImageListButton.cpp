/**************************************************************************
	created		:	2013-05-03
	filename	: 	ImageListButton.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "VisualMgr.h"

#include "ImageListButton.h"
#include "VisualDialog.h"

IMPLEMENT_DYNAMIC(CImageListButton, CButton)
BEGIN_MESSAGE_MAP(CImageListButton, CButton)
	ON_MESSAGE(BM_SETSTYLE, OnSetStyle)
	ON_MESSAGE(BM_SETIMAGE, OnSetImage)
	ON_MESSAGE(BM_SETCHECK, OnSetCheck)
	ON_WM_MOUSELEAVE()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

//! \brief CImageListButton default constructor
CImageListButton::CImageListButton() : m_bFlat(false), m_OriginalStyle(0U), m_bChecked(false),
	m_bDisabled(false), m_bCheckbox(false), m_bButton(true), m_bDefault(false), 
	m_bRadio(false), m_bPressed(false), m_bHighlight(false), m_pToolTip(NULL),
	m_pImageList(NULL), m_ImageIndex(0), m_pDisabledBrush(NULL)
{
	VisualManager *pViMgr = VisualManager::GetInstance();

	if (pViMgr != NULL)
	{
		m_pBackgroundBrush = new CBrush(pViMgr->GetDialogBackgroundColor());
		m_pDisabledBrush = new CBrush(pViMgr->GetDisabledButtonColor());
	}
}

//! \brief CImageListButton destructor
CImageListButton::~CImageListButton()
{
	if (m_pDisabledBrush != NULL)
	{
		delete m_pDisabledBrush;
		m_pDisabledBrush = NULL;
	}

	if (m_pBackgroundBrush != NULL)
	{
		delete m_pBackgroundBrush;
		m_pBackgroundBrush = NULL;
	}

	CTooltipManager::DeleteToolTip(m_pToolTip);
}

//! Called by the framework to allow other necessary subclassing to occur before the window is subclassed
void CImageListButton::PreSubclassWindow()
{
	UINT Style;

	Style = GetButtonStyle();

	// Set initial control type
	m_OriginalStyle = (Style & BS_TYPEMASK);

	// Check if this is a radio button
	m_bRadio = ((Style & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON) ||
			   ((Style & BS_RADIOBUTTON) == BS_RADIOBUTTON);
	// Check if this is a checkbox
	m_bCheckbox = ((Style & BS_CHECKBOX) == BS_CHECKBOX);
	// Check if this is flag
	m_bFlat = ((Style & BS_FLAT) == BS_FLAT);

	// You should not set the Owner Draw before this call
	// (don't use the resource editor "Owner Draw" or
	// ModifyStyle(0, BS_OWNERDRAW) before calling PreSubclassWindow() )
	ASSERT(m_OriginalStyle != BS_OWNERDRAW);

	// Switch to owner-draw
	ModifyStyle(BS_TYPEMASK, BS_OWNERDRAW, SWP_FRAMECHANGED);
}

/*! \brief (MFC)Used to translate window messages before they are dispatched to the
		   TranslateMessage and DispatchMessage Windows functions.
	\param[in] pMsg_in : Pointer to a MSG structure that contains the message to process.
	\return Non-zero if the message was translated and should not be dispatched;
			0 if the message was not translated and should be dispatched.
*/
BOOL CImageListButton::PreTranslateMessage(MSG* pMsg_in)
{
	switch(pMsg_in->message)
	{
	case WM_MOUSEMOVE:
		if (m_pToolTip->GetSafeHwnd() != NULL)
			m_pToolTip->RelayEvent(pMsg_in);
		break;
	}

	return CButton::PreTranslateMessage(pMsg_in);
}

/*! \brief Sets the text of the button and its tooltip
	\param[in] Text_in : the text of the tooltip
*/
void CImageListButton::SetTooltip(const CString &Text_in)
{
	// set the button text
	SetWindowText(Text_in);
	// destroy the previous tooltip
	if (m_pToolTip != NULL)
		CTooltipManager::DeleteToolTip(m_pToolTip);
	// create the tooltip
	CTooltipManager::CreateToolTip(m_pToolTip, this, AFX_TOOLTIP_TYPE_BUTTON);
	// set the tooltip text
	if (m_pToolTip != NULL)
		m_pToolTip->AddTool(this, Text_in);
}

/*! \brief Member function called when the visual aspect of an owner-drawn button has changed
	\param[in] lpDrawItemStruct_in : pointer to a DRAWITEMSTRUCT structure containing 
									 information about the item to be drawn
*/
void CImageListButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct_in)
{
	bool NoBorder = (m_bHighlight == false && m_bPressed == false && m_bChecked == false);
	CRect ButtonRect(lpDrawItemStruct_in->rcItem);
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct_in->hDC);
	VisualManager *pViMgr = VisualManager::GetInstance();	

	// draw the background of the dialog
	pDC->FillRect(ButtonRect, m_pBackgroundBrush);

	m_bDisabled = ((lpDrawItemStruct_in->itemState & ODS_DISABLED) == ODS_DISABLED);
	m_bPressed = ((lpDrawItemStruct_in->itemState & ODS_SELECTED) == ODS_SELECTED);
	m_bDefault = ((lpDrawItemStruct_in->itemState & ODS_DEFAULT) == ODS_DEFAULT);

	// false => handle highlighting after the image has been drawn
	pViMgr->DrawButtonFrame(pDC, ButtonRect, m_bDefault,
							m_bPressed, false, m_bDisabled,
							m_bChecked, m_bFlat, NoBorder);

	if (m_pImageList != NULL && m_ImageRect.IsRectEmpty() == FALSE)
	{
		ButtonRect.OffsetRect(-ButtonRect.left, -ButtonRect.top);
		// center the image
		CPoint Pos ((ButtonRect.right - m_ImageRect.right) / 2,
					(ButtonRect.bottom - m_ImageRect.bottom) / 2);
		UINT Style = (ILD_TRANSPARENT);

		if (m_bDisabled)
		{
			CSize RectSize = m_ImageRect.Size();
			CBitmap Image;

			// create the destination image
			if (Image.CreateCompatibleBitmap(pDC, RectSize.cx, RectSize.cy))
			{
				CBitmap *pBmpOld;
				CDC MemDC;

				MemDC.CreateCompatibleDC(pDC);
				// select the destination bitmap into the DC we created above
				pBmpOld = MemDC.SelectObject(&Image);
				// fill the image with the disabled color
				MemDC.FillRect(m_ImageRect, m_pDisabledBrush);
				// draw the image onto the destination bitmap
				if (m_pImageList->Draw(&MemDC, m_ImageIndex, CPoint(0, 0), Style))
				{
					HBITMAP hImage = (HBITMAP)Image.GetSafeHandle();

					// select the previous bitmap (must be done before modifying the image again)
					MemDC.SelectObject(pBmpOld);
					// convert the image to gray scale
					VisualManager::MapToGray(pDC, hImage);
					// draw the image to the device context
					pDC->DrawState(Pos, RectSize, hImage, DST_BITMAP);
				}
			}
		}
		else
		{
			m_pImageList->Draw(pDC, m_ImageIndex, Pos, Style);
		}
	}

	if (m_bHighlight && m_bPressed == false)
	{
		CDrawingManager DM(*pDC);

		DM.HighlightRect(ButtonRect);
	}

	// only draw the text if no image was set
	if (m_pImageList == NULL)
	{
		CString Text;
	
		GetWindowText(Text);
	
		if (Text.IsEmpty() == false)
		{
			pViMgr->DrawButtonText(pDC, Text, afxGlobalData.clrBtnText, ButtonRect,
								   CMFCButton::ALIGN_CENTER, m_bDisabled);
		}
	}
}

LRESULT CImageListButton::OnSetStyle(WPARAM wParam, LPARAM lParam)
{
	// Can't change control type after owner-draw is set.
	// Let the system process changes to other style bits
	// and redrawing, while keeping owner-draw style
	return DefWindowProc(BM_SETSTYLE, (wParam & ~BS_TYPEMASK) | BS_OWNERDRAW, lParam);
}

LRESULT CImageListButton::OnSetImage(WPARAM wParam, LPARAM lParam)
{
	// if lParam is NULL, it means GetIcon/GetBitmap/GetCursor
	if (lParam == 0L || wParam == IMAGE_CURSOR)
		return DefWindowProc(BM_SETIMAGE, wParam, lParam);

	return 0L;
}

/*! \brief Sets the image list used to render the button image
	\param[in] pImageList_in : the new image list
*/
void CImageListButton::SetImageList(CImageList *pImageList_in)
{
	m_pImageList = pImageList_in;

	if (pImageList_in != NULL)
	{
		int Count = pImageList_in->GetImageCount();
		long NewIndex = m_ImageIndex;
		IMAGEINFO Info;

		if (NewIndex >= Count)
			NewIndex = Count - 1;

		if (pImageList_in->GetImageInfo(m_ImageIndex, &Info))
			m_ImageRect = Info.rcImage;

		if (NewIndex != m_ImageIndex)
			SetImage(NewIndex);
	}
	else 
		m_ImageIndex = 0;
}

/*! \brief Sets the index of the current image
	\param[in] Index_in : the new index
 */
void CImageListButton::SetImage(int Index_in)
{
	if (m_ImageIndex != Index_in)
	{
		if (m_pImageList != NULL)
		{
			int Count = m_pImageList->GetImageCount();
			IMAGEINFO Info;

			if (Index_in >= Count)
				Index_in = Count -1;
			else if (Index_in < 0)
				Index_in = 0;

			m_ImageIndex = Index_in;

			if (m_pImageList->GetImageInfo(m_ImageIndex, &Info))
			{
				m_ImageRect = Info.rcImage;
				m_ImageRect.OffsetRect(-m_ImageRect.left, -m_ImageRect.top);
			}

			Invalidate();
		}
		else
			m_ImageIndex = 0;
	}
}

void CImageListButton::OnMouseMove(UINT nFlags, CPoint point)
{
	CButton::OnMouseMove(nFlags, point);

	ClientToScreen(&point);
	CWnd* wndUnderMouse = WindowFromPoint(point);

	if (wndUnderMouse && wndUnderMouse->m_hWnd == m_hWnd)
	{
		if (m_bHighlight == false)
		{
			TRACKMOUSEEVENT csTME;

			m_bHighlight = true;
			Invalidate();

			csTME.cbSize = sizeof(csTME);
			csTME.dwFlags = TME_LEAVE;
			csTME.hwndTrack = m_hWnd;
			::_TrackMouseEvent(&csTME);
		}
	}
	else
		m_bHighlight = false;
}

void CImageListButton::OnMouseLeave(void)
{
	CButton::OnMouseLeave();

	KillFocus();
}

void CImageListButton::OnKillFocus(CWnd *pNewWnd)
{
	CButton::OnKillFocus(pNewWnd);

	KillFocus();
}

void CImageListButton::KillFocus(void)
{
	if (m_bHighlight == true)
	{
		m_bHighlight = false;
		Invalidate();
	}
}

LRESULT CImageListButton::OnSetCheck(WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_bCheckbox || m_bRadio);

	bool PrevState = m_bChecked;

	switch (wParam)
	{
	case BST_CHECKED:
	case BST_INDETERMINATE:	// Indeterminate state is handled like checked state
		m_bChecked = true;
		break;
	default:
		m_bChecked = false;
		break;
	}

	if (m_bChecked != PrevState)
		Invalidate();

	return 0;
}