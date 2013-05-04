/**************************************************************************
	created		:	2011-07-07
	filename	: 	VisualTabDialog.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2012)
	purpose		:	Base class for tabbed dialogs
**************************************************************************/
#include "stdafx.h"
#include <map>

// Visual dialog
#include "VisualDialog.h"
#include "VisualTabCtrl.h"
#include "VisualTabDialog.h"

#include "TabHeader.h"
#include "VisualTab.h"

IMPLEMENT_DYNAMIC(VisualTabDialog, VisualDialog)
	BEGIN_MESSAGE_MAP(VisualTabDialog, VisualDialog)
END_MESSAGE_MAP()

/*! \brief VisualTabDialog constructor
	\param[in] FirstTabHeaderID_in : the resource ID of the first tab header
	\param[in] LastTabHeaderID_in : the resource ID of the last tab header
	\param[in] FirstTabPageID_in : the resource ID of the first tab page
	\param[in] LastTabPageID_in : the resource ID of the last tab page
	\param[in] TemplateID_in : the resource ID of a dialog box template
	\param[in] pParentWnd_in : a pointer to the parent window of the dialog
*/
VisualTabDialog::VisualTabDialog(UINT FirstTabHeaderID_in, UINT LastTabHeaderID_in,
							 UINT FirstTabPageID_in, UINT LastTabPageID_in,  
							 UINT TemplateID_in, CWnd *pParentWnd_in)
	: VisualDialog(TemplateID_in, pParentWnd_in), m_TabCount(0),
	  m_FirstTabHeaderResID(FirstTabHeaderID_in),
	  m_LastTabHeaderResID(LastTabHeaderID_in),
	  m_FirstTabPageResID(FirstTabPageID_in),
	  m_LastTabPageResID(LastTabPageID_in)
{
	m_pTabCtrl = new VisualTabCtrl(this);
	// make sure the number of tab headers and pages is the same
	ASSERT((m_LastTabHeaderResID - m_FirstTabHeaderResID) == (m_LastTabPageResID - m_FirstTabPageResID));
}

VisualTabDialog::~VisualTabDialog()
{
	if (m_pTabCtrl != NULL)
	{
		delete m_pTabCtrl;
		m_pTabCtrl = NULL;
	}
}

/*! \brief Called by the framework to exchange and validate dialog data
	\param[in,out] pDX_in_out : pointer to a CDataExchange object
*/
void VisualTabDialog::DoDataExchange(CDataExchange* pDX_in_out)
{
	const VisualTabCtrl::Tabs &Tabs = m_pTabCtrl->GetTabs();
	VisualTabCtrl::Tabs::const_iterator Iter;

	// base class data exchange
	VisualDialog::DoDataExchange(pDX_in_out);

	for (Iter = Tabs.begin(); Iter != Tabs.end(); ++Iter)
		DDX_Control(pDX_in_out, Iter->second->GetHeaderResID(), *Iter->second->GetHeader());
}

/*! Member function called in response to the WM_INITDIALOG message
	\return TRUE (non-zero) if the dialog lets Windows set the focus on its first child
			FALSE if the dialog explicitly set the focus on one of its children
*/
BOOL VisualTabDialog::OnInitDialog()
{
	// create the tab headers
	UINT TabHeaderID, TabPageID;

	// check if the container for the pages has been set
	ASSERT(m_HeaderSize.cx != 0 && m_HeaderSize.cy != 0);
	ASSERT(m_ContainerRect.IsRectEmpty() == FALSE);

	m_pTabCtrl->m_ContainerRect = m_ContainerRect;
	m_pTabCtrl->m_HeaderSize = m_HeaderSize;

	for (TabHeaderID = m_FirstTabHeaderResID; TabHeaderID <= m_LastTabHeaderResID; ++TabHeaderID)
	{
		TabPageID = m_FirstTabPageResID + m_TabCount;
		// create a new tab
		m_pTabCtrl->InsertTab(m_TabCount, TabHeaderID, TabPageID);
		// create the default page
		if (m_TabCount == 0)
			CreatePage(m_pTabCtrl, m_TabCount);

		++m_TabCount;
	}
	// call the base class method after the controls have been created because 
	// the framework calls UpdateData(FALSE) in the base dialog initialization
	VisualDialog::OnInitDialog();

	m_pTabCtrl->SetActiveTab(0);

	return TRUE;
}

void VisualTabDialog::OnBnClicked(UINT nID_in)
{
	m_pTabCtrl->SetActiveTabByHeaderResID(nID_in);
}