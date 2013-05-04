/**************************************************************************
	created		:	2013-05-03
	filename	: 	TabPage.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include "TabPage.h"

#include "VisualMgr.h"

#include "setdpi.h"

IMPLEMENT_DYNAMIC(TabPage, CDialogBar)
BEGIN_MESSAGE_MAP(TabPage, CDialogBar)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

/*! \brief TabPage constructor
	\param[in] Width_in : the width of the tab
	\param[in] Height_in : the height of the tab
	\param[in] TabIndex_in : the index of the tab
*/
TabPage::TabPage(UINT Width_in, UINT Height_in, UINT TabIndex_in)
	: m_TabIndex(TabIndex_in), m_MandatoryID(0), m_HeaderHeight(0),
	  m_ContainerRect(0, 0, Width_in, Height_in), m_hBkgColor(NULL),
	  m_hTransparentBrush(NULL), m_pDPI(NULL)
{
	// load the colors through the visual manager
	VisualManager *pManager = VisualManager::GetInstance();

	if (pManager != NULL)
	{
		m_Background = pManager->GetTabDialogPageBackgroundColor();
		m_Border = pManager->GetTabDialogPageBorderColor();
	}
	else
		ASSERT(FALSE);

	// create the brush used for transparent statics
	m_hTransparentBrush = (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
	m_hBkgColor = (HBRUSH)CBrush(m_Background).Detach();
}

//! TabPage destructor
TabPage::~TabPage()
{
	MandatoryMarks::const_iterator MarkIt;

	for (MarkIt = m_Marks.begin(); MarkIt != m_Marks.end(); ++MarkIt)
		delete MarkIt->second;

	m_Marks.clear();

	if (m_hTransparentBrush != NULL)
	{
		DeleteObject(m_hTransparentBrush);
		m_hTransparentBrush = NULL;
	}

	if (m_pDPI != NULL)
	{
		delete m_pDPI;
		m_pDPI = NULL;
	}
}

/*! \brief Member function called when the dialog background needs erasing (for example, when resized)
		   The dialog delegates this event to its implementation
	\param[in] pDC_in_out : a pointer to a device-context object
	\return TRUE (non-zero) if the background was erased; FALSE otherwise
*/
BOOL TabPage::OnEraseBkgnd(CDC *pDC_in_out)
{
	CDrawingManager dm(*pDC_in_out);

	// draw the background and border of the tab
	dm.DrawRect(m_ContainerRect, m_Background, m_Border);
	// draw a line on the left side of the active border to connect with the header
	dm.DrawLine(0, m_ActiveTabBorder.top, 0, m_ActiveTabBorder.bottom, m_Background);

	return TRUE;
}

//! \brief Computes the area of the active border based on the index of the tab
void TabPage::RecalcActiveTabBorder(void)
{
	m_ActiveTabBorder.right = m_ActiveTabBorder.left = 0;
	m_ActiveTabBorder.top = m_TabIndex * m_HeaderHeight + 1;
	m_ActiveTabBorder.bottom = m_ActiveTabBorder.top + m_HeaderHeight - 4;
}

/*! \brief Member function called when a child control is about to be drawn
		   Returns a handle to the brush that is to be used for painting the control background
	\param[in] pDC_in_out : contains a pointer to the display context for the child window (may be temporary)
	\param[in] pWnd_in_out : contains a pointer to the control asking for the color (may be temporary)
	\param[in] nControlID_in : the type of the control
	\return a handle to the brush used to draw the control background
*/
HBRUSH TabPage::OnCtlColor(CDC* pDC_in_out, CWnd* pWnd_in_out, UINT nControlID_in)
{
	ASSERT(pWnd_in_out != NULL);

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
				if (pWnd_in_out->GetDlgCtrlID() == m_MandatoryID)
					pDC_in_out->SetTextColor(MANDATORY_FIELD_COLOR);

				return m_hTransparentBrush;
			}
			else
			{
				// background color
				return m_hBkgColor;
			}
		}
		break;
	}

	return CDialogBar::OnCtlColor(pDC_in_out, pWnd_in_out, nControlID_in);
}

/*! \brief Creates a tab page sub-dialog
	\param[in] pParentWnd_in : the parent dialog of the page
	\param[in] ContainerRect_in : rectangle representing the area of the page
	\param[in] HeaderHeight_in : height of the tab header corresponding to the page
	\param[in] nIDTemplate_in : the dialog resource ID of the page
	\return true if the page was created successfully; false otherwise
*/
bool TabPage::Create(CWnd *pParentWnd_in, const CRect &ContainerRect_in, 
					 UINT HeaderHeight_in, UINT nIDTemplate_in)
{
	m_HeaderHeight = HeaderHeight_in;

	// create the control
	if (CDialogBar::Create(pParentWnd_in, nIDTemplate_in, WS_CHILD, nIDTemplate_in))
	{
		// hide the page
		ShowWindow(SW_HIDE);
		// force the dialog bar to use a fixed DPI resolution
		SetFixedDPI(nIDTemplate_in, DLG_FIXED_DPI);
		// move the page at the top-left corner of the container
		SetWindowPos(NULL, ContainerRect_in.left + 1, ContainerRect_in.top + 1,
					 ContainerRect_in.Width() - 1, ContainerRect_in.Height() - 1,
					 SWP_NOACTIVATE);
		// compute the border for the active tab
		RecalcActiveTabBorder();
		OnCreate();
		// show the page
		ShowWindow(SW_SHOWNA);

		return true;
	}

	return false;
}

/*! \brief Sets a mandatory mark for the field label specified by its resource ID
	\param[in] FieldLabelID_in : the resource ID of the field label
	\param[in] Mandatory_in : flag specifying if the field is mandatory
*/
void TabPage::SetMandatory(UINT FieldLabelID_in, bool Mandatory_in)
{
	// set the resource ID for the mandatory mark in the derived class
	ASSERT(m_MandatoryID != 0);

	if (::IsWindow(m_hWnd) && m_MandatoryID != 0)
	{
		MandatoryMarks::const_iterator MarkIt = m_Marks.find(FieldLabelID_in);

		// the field is to be marked as mandatory
		if (Mandatory_in && MarkIt == m_Marks.end())
		{
			CWnd *pLabel = GetDlgItem(FieldLabelID_in);

			if (pLabel != NULL)
			{
				CStatic *pMark = new CStatic;
				CRect WindowRect, MarkRect;

				pLabel->GetWindowRect(WindowRect);
				ScreenToClient(WindowRect);

				MarkRect = WindowRect;
				MarkRect.left -= 10;
				MarkRect.right = MarkRect.left + 8;

				if (pMark->Create(_T("*"), WS_CHILD | WS_VISIBLE | SS_CENTER, MarkRect, this, m_MandatoryID))
				{
					pMark->SetFont(GetFont());
					m_Marks[FieldLabelID_in] = pMark;
				}
				else
					delete pMark;
			}
		}
		// the field is no longer mandatory
		else if (Mandatory_in == false && MarkIt != m_Marks.end())
		{
			// destroy the window
			::DestroyWindow(MarkIt->second->GetSafeHwnd());
			// destroy the object
			delete MarkIt->second;
			m_Marks.erase(MarkIt);
		}
	}
}

/*! \brief Sets the dialog to be displayed at afixed DPI resolution
	\param[in] DialogTplID_in : the dialog template ID
	\param[in] DPI_in : the DPI resolution
*/
void TabPage::SetFixedDPI(UINT DialogTplID_in, int DPI_in)
{
	if (m_pDPI == NULL && VisualManager::IsDPI(DPI_in) == false)
	{
		m_pDPI = new CSetDPI;

		m_pDPI->Attach(AfxFindResourceHandle(MAKEINTRESOURCE(DialogTplID_in), RT_DIALOG),
					   m_hWnd, DialogTplID_in, DPI_in, true);
	}
}