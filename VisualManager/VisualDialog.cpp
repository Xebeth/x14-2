/**************************************************************************
	created		:	2011-07-07
	filename	: 	VisualDialog.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2012)
	purpose		:	CDialogEx derived class used as a base class for all the dialogs in the application
					It applies the main window theme to dialogs using the visual manager
**************************************************************************/
#include "stdafx.h"

#include "VisualMgr.h"

#include "VisualDialogImpl.h"		// implementation of the dialog
#include "VisualDialog.h"

#include "VisualDialog.h"

#include "setdpi.h"

IMPLEMENT_DYNAMIC(VisualDialog, CDialogEx)

BEGIN_MESSAGE_MAP(VisualDialog, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_NCCALCSIZE()
	ON_WM_NCACTIVATE()
	ON_WM_NCPAINT()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_NCMOUSEMOVE()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_ACTIVATE()
	ON_MESSAGE (WM_SETTEXT, &VisualDialog::OnSetText)
END_MESSAGE_MAP()

CSetDPI * VisualDialog::m_pDPI = NULL;

/*! \brief VisualDialog constructor
	\param[in] TemplateID_in : The resource ID of a dialog box template
	\param[in] pParentWnd_in : a pointer to the parent window of the dialog
*/
VisualDialog::VisualDialog(UINT TemplateID_in, CWnd* pParentWnd_in)
	: CDialogEx(TemplateID_in, pParentWnd_in), m_pImageList(NULL),
	  m_IconResID(0), m_hTransparentBrush(NULL), m_IconBackground(-1),
	  m_hIcon(NULL), m_IconBorder(-1), m_DefaultButton(IDOK), // 1 => IDOK
	  m_FrameHeight(GetSystemMetrics(SM_CYDLGFRAME)),
	  m_FrameWidth(GetSystemMetrics(SM_CXDLGFRAME)),
	  m_pImpl(new VisualDialogImpl(*this)),
	  m_bIsToolWindow(false)
{
	m_hTransparentBrush = (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
	m_hBkgColor = (HBRUSH)CBrush(VisualManager::GetInstance()->GetDialogBackgroundColor()).Detach();
}

//! VisualDialog destructor
VisualDialog::~VisualDialog()
{
	m_TitleFont.DeleteObject();

	if (m_hTransparentBrush != NULL)
	{
		DeleteObject(m_hTransparentBrush);
		m_hTransparentBrush = NULL;
	}

	if (m_pImpl != NULL)
	{
		delete m_pImpl;
		m_pImpl = NULL;
	}

	if (m_pDPI != NULL)
	{
		delete m_pDPI;
		m_pDPI = NULL;
	}

	if (m_pImageList != NULL)
	{
		m_pImageList->DeleteImageList();
		delete m_pImageList;
		m_pImageList = NULL;
	}
}

/*! \brief Creates an image list for the tabs and loads the images from the resource
	\returns true if the images were loaded successfully; false otherwise
*/
bool VisualDialog::CreateImageList(UINT ResID_in, UINT IconCount_in, CSize IconSize_in, UINT ColorRes_in)
{
	if (m_pImageList == NULL)
		m_pImageList = new CImageList;

	return CreateImageList(*m_pImageList, ResID_in, IconCount_in, IconSize_in, ColorRes_in);
}

bool VisualDialog::CreateImageList(CImageList &ImageList_in_out, UINT ResID_in, UINT IconCount_in, CSize IconSize_in, UINT ColorRes_in)
{
	CPngImage Images;

	if (Images.Load(ResID_in))
	{
		// create the image list
		ImageList_in_out.Create(IconSize_in.cx, IconSize_in.cy, ColorRes_in, IconCount_in, IconCount_in);
		// fill the image list
		ImageList_in_out.Add(&Images, 0UL);

		return (ImageList_in_out.GetImageCount() == IconCount_in);
	}

	return false;
}
/*! Member function called in response to the WM_INITDIALOG message
	\return TRUE (non-zero) if the dialog lets Windows set the focus on its first child
			FALSE if the dialog explicitly set the focus on one of its children
*/
BOOL VisualDialog::OnInitDialog(void)
{
	DWORD dwStyle = GetStyle();
	LOGFONT DialogFontLog;

	// set the background of the icon to white
	if (m_IconBackground == -1)
		m_IconBackground = RGB(255U, 255U, 255U);
	// set the icon border to gray
	if (m_IconBorder == -1)
		m_IconBorder = RGB(64U, 64U, 64U);

	// load the icon of the dialog
	if (m_hIcon == NULL && m_IconResID > 0)
		m_hIcon = AfxGetApp()->LoadIcon(m_IconResID);
	// set it
	if (m_hIcon != NULL)
	{
		SetIcon(m_hIcon, FALSE);
		SetIcon(m_hIcon, TRUE);
	}

	VisualManager::DisableDwmNcRendering(m_hWnd);

	m_bIsToolWindow = ((GetExStyle() & WS_EX_TOOLWINDOW) == WS_EX_TOOLWINDOW);

	if ((dwStyle & WS_MAXIMIZEBOX) == WS_MAXIMIZEBOX)
	{
		// remove the maximize and minimize button from the caption bar
		ModifyStyle(WS_MAXIMIZEBOX, NULL, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
		// @TODO : support maximizing
		#pragma warn("TODO: support maximizing -- the maximize button will be disabled")
	}
	// initialize the caption buttons
	m_pImpl->InitializeCaptionButtons();
	// retrieve the initial title of the dialog
	GetWindowText(m_DialogTitle);
	// update the internal rectangles used to draw the dialog
	UpdateWindowRect();
	// move the window to adjust the change in border size
	SetWindowPos(NULL, -1, -1, m_WindowRect.Width() - m_FrameWidth, m_WindowRect.Height() - m_FrameHeight,
				 SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE | SWP_FRAMECHANGED |SWP_DRAWFRAME);
	if (m_TitleFont.GetSafeHandle() == NULL)
	{
		// create a bold version of the default font
		GetFont()->GetLogFont(&DialogFontLog);
		// make the default font bold
		DialogFontLog.lfWeight = FW_BOLD;

		m_TitleFont.CreateFontIndirect(&DialogFontLog);
	}
	
	// call the base class initialization function
	CDialogEx::OnInitDialog();

	LRESULT MsgResult = SendMessage(DM_GETDEFID);

	// set the correct default button for the dialog
	if (MsgResult != 0UL)
	{
		// From MSDN: [...] the low-order word contains the control identifier
		CButton *pDefButton = static_cast<CButton*>(GetDlgItem(LOWORD(MsgResult)));

		SetDefaultButton(pDefButton);
	}
	else
	{
		CButton *pDefButton = static_cast<CButton*>(GetDlgItem(IDOK));

		SetDefaultButton(pDefButton);
	}

	return TRUE;
}

//! Updates the various areas composing the dialog window (frame, caption, client)
void VisualDialog::UpdateWindowRect(void)
{
	DWORD dwExStyle = GetExStyle();

	GetWindowRect(m_WindowRect);
	GetClientRect(m_ClientRect);

	m_ClientRect.bottom -= m_FrameHeight - 1;
	m_ClientRect.right -= m_FrameWidth - 1;
	
	m_FrameRect.top = m_FrameRect.left = 0;
	m_FrameRect.right = m_WindowRect.Width();
	m_FrameRect.bottom = m_WindowRect.Height();

	m_CaptionRect = m_FrameRect;

	if (m_bIsToolWindow)
		m_CaptionRect.bottom = GetSystemMetrics(SM_CYSMCAPTION) + 10;
	else
		m_CaptionRect.bottom = GetSystemMetrics(SM_CYCAPTION) + 10;
}

/*! \brief Member function called when the dialog background needs erasing (for example, when resized)
		   The dialog delegates this event to its implementation
	\param[in] pDC_in_out : a pointer to a device-context object
	\return TRUE (non-zero) if the background was erased; FALSE otherwise
*/
BOOL VisualDialog::OnEraseBkgnd(CDC* pDC_in_out)
{
	return m_pImpl->OnEraseBkgnd(pDC_in_out, m_ClientRect);
}

/*! \brief Member function called when a child control is about to be drawn
		   Returns a handle to the brush that is to be used for painting the control background
	\param[in] pDC_in_out : contains a pointer to the display context for the child window (may be temporary)
	\param[in] pWnd_in_out : contains a pointer to the control asking for the color (may be temporary)
	\param[in] nControlID_in : the type of the control
	\return a handle to the brush used to draw the control background
*/
HBRUSH VisualDialog::OnCtlColor(CDC* pDC_in_out, CWnd* pWnd_in_out, UINT nControlID_in)
{
	switch (nControlID_in)
	{
		case CTLCOLOR_STATIC:
		{
			TCHAR CtlClass[64] = { '\0' };

			// set the background mode of the drawing context to transparent
			pDC_in_out->SetBkMode(TRANSPARENT);
			// retrieve the class name of the control (for checkbox controls)
			::GetClassName(pWnd_in_out->GetSafeHwnd(), CtlClass, 64);
			// Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message;
			// instead, they send the WM_CTLCOLORSTATIC message (MSDN)
			if (_tcsicmp(CtlClass, _T("Static")) == 0)
			{
				// enable the sub-classes to override the static colors
				return OnStaticColor(pDC_in_out, pWnd_in_out, pWnd_in_out->GetDlgCtrlID());
			}
			else
			{
				// background color
				return m_hBkgColor;
			}
		}
		break;
	}

	return CDialogEx::OnCtlColor(pDC_in_out, pWnd_in_out, nControlID_in);
}

/*! \brief Member function called when the size and position of the client area needs to be calculated
		   The dialog delegates this event to its implementation
	\param[in] bCalcValidRects_in : Specifies whether the application should specify 
				which part of the client area contains valid information
				Windows will copy the valid information to the specified area 
				within the new client area. If this parameter is TRUE, 
				the application should specify which part of the client 
				area is valid.
	\param[in] pCalcSizeParams_in : pointer to a NCCALCSIZE_PARAMS data structure that contains
									information used to calculate the new size and position of 
									the CWnd rectangle (including client area, borders, 
									caption, scroll bars, and so on).
*/
void VisualDialog::OnNcCalcSize(BOOL bCalcValidRects_in, NCCALCSIZE_PARAMS FAR* pCalcSizeParams_in)
{
	if (m_pImpl->OnNcCalcSize(bCalcValidRects_in, pCalcSizeParams_in) == FALSE)
		CDialogEx::OnNcCalcSize(bCalcValidRects_in, pCalcSizeParams_in);
}


/*! \brief Member function called when the non-client area needs to be painted
		   The dialog delegates this event to its implementation
*/
void VisualDialog::OnNcPaint(void)
{
	if (m_pImpl->OnNcPaint() == FALSE)
		CDialogEx::OnNcPaint();
}

/*! \brief Member function called when the non-client area needs to be changed to indicate an active or inactive state
		   The dialog delegates this event to the visual manager
	\param[in] bActive_in : Specifies when a caption bar or icon needs to be changed
							TRUE if an active caption or icon is to be drawn
							FALSE for an inactive caption or icon
	\return TRUE (non-zero) if Windows should proceed with default processing;
			FALSE to prevent the caption bar or icon from being deactivated
*/
BOOL VisualDialog::OnNcActivate(BOOL bActive_in)
{
	m_bIsActive = bActive_in;

	// use the visual manager handler because the CDialogEx one displays
	// the default caption when the window is inactive
	return CMFCVisualManager::GetInstance()->OnNcActivate(this, bActive_in);
}

/*! \brief Member function called for the CWnd object that contains the cursor 
		   to capture the mouse input) every time the mouse is moved
		   The dialog delegates this event to its implementation
	\param[in] MousePos_in : Contains the x- and y-coordinates of the cursor
							 These coordinates are always screen coordinates
	\return the mouse hit-test value (winuser.h line 2251)
*/
LRESULT VisualDialog::OnNcHitTest(CPoint MousePos_in)
{
	UINT nHit = m_pImpl->OnNcHitTest(MousePos_in);

	if (nHit == HTNOWHERE)
		return CDialogEx::OnNcHitTest(MousePos_in);
	else
		return nHit;
}

/*! \brief Member function called when the user releases the 
		   left mouse button while the cursor is within a non-client area
		   The dialog delegates this event to its implementation 
		   and then passes it to the base class
	\param[in] nHitTest_in : the mouse hit-test value
	\param[in] MousePos_in : Contains the x- and y-coordinates of the cursor
							 These coordinates are always screen coordinates
*/
void VisualDialog::OnNcLButtonUp(UINT nHitTest_in, CPoint MousePos_in)
{
	m_pImpl->OnLButtonUp(MousePos_in);
	CDialogEx::OnNcLButtonUp(nHitTest_in, MousePos_in);
}

/*! \brief Member function called when the mouse cursor moves
		   The dialog delegates this event to its implementation 
		   and then passes it to the base class
	\param[in] nHitTest_in : Indicates whether various virtual keys are down
	\param[in] MousePos_in : Contains the x- and y-coordinates of the cursor
							 These coordinates are always screen coordinates
*/
void VisualDialog::OnNcMouseMove(UINT nHitTest_in, CPoint MousePos_in)
{
	m_pImpl->OnNcMouseMove(nHitTest_in, MousePos_in);
	CDialogEx::OnNcMouseMove(nHitTest_in, MousePos_in);
}

/*! \brief Member function called when the cursor is moved within a non-client area
		   left mouse button while the cursor is within a non-client area
		   The dialog delegates this event to its implementation 
		   and then passes it to the base class
	\param[in] nHitTest_in : the mouse hit-test value
	\param[in] MousePos_in : Contains the x- and y-coordinates of the cursor
							 These coordinates are always screen coordinates
*/
void VisualDialog::OnMouseMove(UINT nFlags_in, CPoint MousePos_in)
{
	m_pImpl->OnMouseMove(MousePos_in);
	CDialogEx::OnMouseMove(nFlags_in, MousePos_in);
}

/*! \brief Member function called when the user presses the left mouse button
		   while the cursor is within a non-client area
		   The dialog delegates this event to its implementation 
		   and then passes it to the base class
	\param[in] nHitTest_in : the mouse hit-test value
	\param[in] MousePos_in : Contains the x- and y-coordinates of the cursor
							 These coordinates are always screen coordinates
*/
void VisualDialog::OnNcLButtonDown(UINT nHitTest_in, CPoint MousePos_in)
{
	m_pImpl->OnLButtonDown(MousePos_in);
	CDialogEx::OnNcLButtonDown(nHitTest_in, MousePos_in);
}

/*! \brief Member function called after the window's size has changed
	\param[in] nSizeType_in : specifies the type of resizing requested
	\param[in] Width_in : specifies the new width of the client area
	\param[in] Height_in : specifies the new height of the client area
*/
void VisualDialog::OnSize(UINT nSizeType_in, int Width_in, int Height_in)
{
	CDialogEx::OnSize(nSizeType_in, Width_in, Height_in);

	UpdateWindowRect();

	m_pImpl->InitializeCaptionButtons();
}

/*! \brief Member function called when the text of the dialog is set
	\param[in] wParam : unused
	\param[in] lParam : pointer to a null-terminated string that is the window text
	\return TRUE if the text is set
	\sa WM_SETTEXT message
*/
LRESULT VisualDialog::OnSetText(WPARAM wParam, LPARAM lParam)
{
	m_DialogTitle = reinterpret_cast<TCHAR*>(lParam);

	return DefWindowProc(WM_SETTEXT, wParam, lParam);
}

/*! \brief Sets the text of a control and invalidates it's rectangle to force a redraw if necessary
	\param[in] ControlID_in : the ID of the control
	\param[in] Text_in : the new text of the control
	\param[in] Invalidate_in : flag specifying if the control area needs to be redrawn
	\param[in] ForceUpdate_in : flag specifying if the parent window has to be updated immediately
*/
void VisualDialog::UpdateControlText(UINT ControlID_in, const CString &Text_in,
								   bool Invalidate_in, bool ForceUpdate_in)
{
	CWnd *pControl = GetDlgItem(ControlID_in);

	if (pControl != NULL)
	{
		CString ControlText;

		pControl->GetWindowText(ControlText);

		// check if the title has changed
		if (ControlText.Compare(Text_in) != 0)
		{
			if (Invalidate_in)
			{
				CRect ControlRect;

				// invalidate only the area covering the control
				pControl->GetWindowRect(ControlRect);
				ScreenToClient(ControlRect);
				InvalidateRect(ControlRect);
			}

			pControl->SetWindowText(Text_in);

			if (Invalidate_in && ForceUpdate_in)
			{
				// force the dialog to redraw
				UpdateWindow();
			}
		}
	}
}

/*! \brief Sets the default button for the dialog
	\param[in] ButtonResID_in : the resource ID of the new default button
 */
void VisualDialog::SetDefaultButton(UINT ButtonResID_in)
{
	
	// retrieve the current default button
	LRESULT MsgResult = SendMessage(DM_GETDEFID);

	if (MsgResult != 0L)
	{
		// From MSDN: [...] the low-order word contains the control identifier
		UINT ResID = LOWORD(MsgResult);

		if (ResID != ButtonResID_in)
		{
			CButton *pNewDefault = static_cast<CButton*>(GetDlgItem(ButtonResID_in));
			CButton *pCurrentDefault = static_cast<CButton*>(GetDlgItem(ResID));

			// set the ID of the new default button for the dialog
			SendMessage(DM_SETDEFID, ButtonResID_in, 0);

			if (pNewDefault != NULL && pCurrentDefault != NULL)
			{
				// make the button the new default one
				SetDefaultButton(pNewDefault, true);
				// remove the default status to the previous button
				SetDefaultButton(pCurrentDefault, false);
			}
		}
	}
}

/*! \brief Sets the specified button as the default button or not
	\param[in] pButton_in : the button to modify
	\param[in] Default_in : flag specifying if the button is made default
	\return true if the button was changed; false otherwise
*/
bool VisualDialog::SetDefaultButton(CButton *pButton_in, bool Default_in)
{
	if (pButton_in != NULL && ::IsWindow(pButton_in->GetSafeHwnd()))
	{
		if (IsMFCButton(pButton_in) == false)
		{
			if (Default_in)
			{
				pButton_in->SetButtonStyle(BS_DEFPUSHBUTTON);
				m_DefaultButton = 0U;
			}
			else
				pButton_in->SetButtonStyle(BS_PUSHBUTTON);
		}
		else
		{
			if (Default_in)
				m_DefaultButton = pButton_in->GetDlgCtrlID();

			pButton_in->Invalidate();
		}

		return true;
	}

	return false;
}

/*! \brief Checks if the specified button is a CMFCButton
	\param[in] pButton_in : the button to check
	\return true if the button is a CMFCButton; false otherwise
*/
bool VisualDialog::IsMFCButton(const CButton *pButton_in)
{
	if (pButton_in != NULL)
		return (DYNAMIC_DOWNCAST(CMFCButton, pButton_in) != NULL);

	return false;
}

/*! \brief Member function called when a CWnd object is being activated or deactivated
	\param[in] nState_in : specifies whether the CWnd is being activated or deactivated
	\param[in] pWndOther_in : the CWnd being activated or deactivated
	\param[in] bMinimized_in : specifies the minimized state of the CWnd being activated or deactivated
*/
void VisualDialog::OnActivate(UINT nState_in, CWnd* pWndOther_in, BOOL bMinimized_in)
{
	if (m_pImpl != NULL && m_pImpl->OnActivate(nState_in, pWndOther_in, bMinimized_in) == FALSE)
		CDialogEx::OnActivate(nState_in, pWndOther_in, bMinimized_in);
}

/*! \brief Sets the dialog to be displayed at a fixed DPI resolution
	\param[in] DialogTplID_in : the dialog template ID
	\param[in] DPI_in : the DPI resolution
	\param[in] ForceAttach_in : flag specifying if the DPI resizing should be reapplied
*/
void VisualDialog::SetFixedDPI(HWND hWnd_in, UINT DialogTplID_in, int DPI_in, bool ForceAttach_in)
{
	if ((m_pDPI == NULL || ForceAttach_in) && VisualManager::IsDPI(DPI_in) == false)
	{
		if (m_pDPI == NULL)
			m_pDPI = new CSetDPI;

		m_pDPI->Attach(AfxFindResourceHandle(MAKEINTRESOURCE(DialogTplID_in), RT_DIALOG),
					   hWnd_in, DialogTplID_in, DPI_in, false);

		CWnd::FromHandle(hWnd_in)->CenterWindow();
	}
}