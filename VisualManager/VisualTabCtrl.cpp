/**************************************************************************
	created		:	2011-07-08
	filename	: 	VisualTabCtrl.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2012)
	purpose		:	Visual tab control
**************************************************************************/
#include "stdafx.h"
#include <map>

#include "VisualTabCtrl.h"
#include "VisualTab.h"

#include "VisualDialog.h"			// Visual dialog
#include "VisualTabDialog.h"		// Visual tab dialog

#include "TabHeader.h"
#include "TabPage.h"

/*! VisualTabCtrl constructor
	\param[in] pParent_in : the parent dialog of the control
*/
VisualTabCtrl::VisualTabCtrl(VisualTabDialog *pParent_in)
	: m_CurrentTabIndex(0), m_pCurrentTab(NULL), m_HeaderSize(0, 0),
	  m_TabCount(0), m_pParentWnd(pParent_in) {}

//! \brief VisualTabCtrl destructor
VisualTabCtrl::~VisualTabCtrl()
{
	Tabs::iterator TabIter;

	// destroy the tab headers
	for (TabIter = m_Tabs.begin(); TabIter != m_Tabs.end(); ++TabIter)
		delete TabIter->second;

	m_Tabs.clear();
}

/*! \brief Inserts a tab after the last position
	\param[in] TabHeaderID_in : the resource ID of the tab header
	\param[in] TabPageID_in : the resource ID of the tab page
	\return true if the tab was added successfully; false otherwise
*/
bool VisualTabCtrl::AddTab(UINT TabHeaderID_in, UINT TabPageID_in, const TCHAR *pHeaderText_in)
{
	return InsertTab(m_TabCount, TabHeaderID_in, TabPageID_in, pHeaderText_in);
}

/*! \brief Inserts a tab at the specified position
	\param[in] TabIndex_in : the index where the tab has to be inserted
	\param[in] TabHeaderID_in : the resource ID of the tab header
	\param[in] TabPageID_in : the resource ID of the tab page
	\return true if the tab was inserted successfully; false otherwise
*/
bool VisualTabCtrl::InsertTab(UINT TabIndex_in, UINT TabHeaderID_in, UINT TabPageID_in, const TCHAR *pHeaderText_in)
{
	VisualTab *pTab = new VisualTab(TabIndex_in, TabHeaderID_in, TabPageID_in);
	TabHeader *pTabHeader = new TabHeader;

	// create the header window
	if (pHeaderText_in == NULL || CreateHeaderWindow(pTabHeader, TabHeaderID_in, pHeaderText_in))
	{
		// delete the current tab at this index
		if (RemoveTab(TabIndex_in) == false)
			++m_TabCount;
		// initialize the font of the header
		pTabHeader->InitFont(m_pParentWnd->GetFont());
		pTab->SetHeader(pTabHeader);
		// set the new tab
		m_Tabs[TabIndex_in] = pTab;
	}
	else
	{
		ASSERT(FALSE);

		delete pTabHeader;
		pTabHeader = NULL;
		delete pTab;
		pTab = NULL;		
	}

	return (pTab != NULL);
}

/*! \brief Creates the window for the new header
	\param[in] pHeader_in : the header object
	\param[in] TabHeaderID_in : the header resource ID
	\param[in] pHeaderText_in : the text of the header
	\return true if the header window was created; false otherwise
*/
bool VisualTabCtrl::CreateHeaderWindow(TabHeader *pHeader_in, UINT TabHeaderID_in, const TCHAR *pHeaderText_in)
{
	if (pHeader_in != NULL)
	{
		if (::IsWindow(pHeader_in->GetSafeHwnd()) == false)
		{
			DWORD Style = BS_OWNERDRAW | BS_LEFT | BS_PUSHLIKE | BS_FLAT | WS_TABSTOP | WS_VISIBLE | WS_CHILD;
			CRect Rect;

			if (m_TabCount > 0U)
			{
				// retrieve the position and size of the last header
				GetTab(m_TabCount - 1)->GetHeader()->GetWindowRect(Rect);
				m_pParentWnd->ScreenToClient(Rect);
				// shift the rectangle to be below it
				Rect.OffsetRect(0, Rect.Height() + 1);
			}
			else
			{
				Rect.right = m_ContainerRect.left + 1;
				Rect.left = Rect.right - m_HeaderSize.cx;
				Rect.top = m_ContainerRect.top + 1;
				Rect.bottom = Rect.top + m_HeaderSize.cy;
			}

			return (pHeader_in->Create(pHeaderText_in, Style, Rect, m_pParentWnd, TabHeaderID_in) != FALSE);
		}

		// the window already exists => nothing to do
		return true;
	}

	return false;
}

/*! \brief Removes the tab at the specified index
	\param[in] TabIndex_in : the index of the tab to be removed
	\return true if the tab was removed; false otherwise
*/
bool VisualTabCtrl::RemoveTab(UINT TabIndex_in)
{
	Tabs::iterator Iter = m_Tabs.find(TabIndex_in);

	// delete the current tab at this index
	if (Iter != m_Tabs.end())
	{
		delete Iter->second;
		m_Tabs.erase(Iter);

		return true;
	}

	return false;		
}

/*! \brief Retrieves the tab object at the specified index
	\param[in] TabIndex_in : the index of the tab to retrieve
	\return a pointer to the tab if found; NULL otherwise
*/
VisualTab* VisualTabCtrl::TabAt(UINT TabIndex_in) const
{
	Tabs::const_iterator Iter = m_Tabs.find(TabIndex_in);

	if (Iter != m_Tabs.end())
		return Iter->second;
	else
		return NULL;
}

/*! \brief Retrieves the tab object at the specified index
	\param[in] TabIndex_in : the index of the tab to retrieve
	\return a const pointer to the tab if found; NULL otherwise
*/
const VisualTab* VisualTabCtrl::GetTab(UINT TabIndex_in) const
{
	return TabAt(TabIndex_in);
}

/*! \brief Sets the page control of the tab at the specified index
	\param[in] TabIndex_in : the index of the tab whose tab is to be set
	\param[in] pPage_in : a pointer to the tab page to be set
	\return true if the tab page was set; false otherwise
*/
bool VisualTabCtrl::SetPage(UINT TabIndex_in, TabPage *pPage_in)
{
	VisualTab *pTab = TabAt(TabIndex_in);

	if (pTab != NULL)
	{
		pTab->SetPage(pPage_in);

		return true;
	}

	return false;
}

/*! \brief Retrieves the page control of the tab at the specified index
	\param[in] TabIndex_in : the index of the tab whose tab is to be retrieved
	\return a pointer to the tab page if found; NULL otherwise
		    if the tab exists but not the page, it is created
*/
TabPage* VisualTabCtrl::GetPage(UINT TabIndex_in)
{
	VisualTab *pTab = TabAt(TabIndex_in);
	TabPage *pResult = NULL;

	// the page hasn't been instantiated yet
	if (pTab != NULL)
	{
		pResult = pTab->GetPage();

		if (pResult == NULL)
		{
			pResult = m_pParentWnd->CreatePage(this, TabIndex_in);
			// update the internal sizes of the tab
			pTab->UpdateSize();
		}
	}

	return pResult;
}

/*! \brief Set the active tab given its index
	\param[in] TabIndex_in : the index of the tab to activate
	\return true if the tab was made active; false otherwise
*/
bool VisualTabCtrl::SetActiveTab(UINT TabIndex_in)
{
	VisualTab *pTab = TabAt(TabIndex_in);

	if (pTab != NULL)
	{
		TabHeader *pHeader = pTab->GetHeader();

		if (pHeader != NULL && IsWindow(pHeader->GetSafeHwnd()))
		{
			// don't switch tabs if we're already on the one we want
			if (m_CurrentTabIndex != TabIndex_in || m_pCurrentTab == NULL)
			{
				TabPage *pCurrentPage = NULL;

				// uncheck the current header
				if (m_pCurrentTab != NULL)
					m_pCurrentTab->GetHeader()->SetCheck(BST_UNCHECKED);
				// check the new header
				pTab->GetHeader()->SetCheck(BST_CHECKED);
				// hide the current page if it exists
				if (m_pCurrentTab != NULL && (pCurrentPage = m_pCurrentTab->GetPage()) != NULL)
				{
					pCurrentPage->OnDeactivate();
					pCurrentPage->ShowWindow(SW_HIDE);
				}
				// retrieve and display the new one
				if ((pCurrentPage = GetPage(TabIndex_in)) != NULL)
				{
					pCurrentPage->OnActivate();
					pCurrentPage->ShowWindow(SW_SHOW);
				}
				// update the current tab
				m_CurrentTabIndex = TabIndex_in;
				m_pCurrentTab = pTab;
			}

			return true;
		}
	}

	return false;
}

/*! \brief Set the active tab given its header resource ID
	\param[in] HeaderResID_in : the resource ID of the tab header
	\return true if the tab was made active; false otherwise
*/
bool VisualTabCtrl::SetActiveTabByHeaderResID(UINT HeaderResID_in)
{
	return SetActiveTab(GetIndexFromHeaderResID(HeaderResID_in));
}

/*! \brief Retrieves the index of a tab given its header resource ID
	\param[in] HeaderResID_in : the resource ID of the tab header
	\return the index of the tab if found; the current tab index otherwise
*/
UINT VisualTabCtrl::GetIndexFromHeaderResID(UINT HeaderResID_in)
{
	Tabs::const_iterator Iter;

	for (Iter = m_Tabs.begin(); Iter != m_Tabs.end(); ++Iter)
		if (Iter->second->GetHeaderResID() == HeaderResID_in)
			return Iter->first;

	return m_CurrentTabIndex;
}

/*! \brief Updates the text of the header given its index
	\param[in] TabIndex_in : the index of the tab
	\param[in] Text_in : the new text
*/
bool VisualTabCtrl::UpdateTabText(UINT TabIndex_in, const CString &Text_in)
{
	Tabs::iterator TabIt = m_Tabs.find(TabIndex_in);

	if (TabIt != m_Tabs.end())
	{
		VisualTab *pTab = TabIt->second;

		if (pTab != NULL)
		{
			TabHeader *pHeader = pTab->GetHeader();

			if (pHeader)
			{
				pHeader->SetWindowText(Text_in);

				return true;
			}
		}
	}

	return false;
}

/*! \brief Retrieves the current tab index
	\return the current tab index
*/
UINT VisualTabCtrl::GetActiveTabIndex(void) const
{
	if (m_pCurrentTab != NULL)
		return m_pCurrentTab->GetTabIndex();

	return -1;
}

/*! \brief Resizes the headers of the tabs
	\param[in] Height_in : the new height of the tab
*/
void VisualTabCtrl::ResizeHeaders(UINT Height_in)
{
	float MaxHeight = (float)m_ContainerRect.Height();
	UINT TabCount = m_Tabs.size();
	Tabs::iterator Iter;
	TabHeader *pHeader;
	CRect HeaderRect;
	VisualTab *pTab;
	UINT Index;
	int Diff;

	if (Height_in == 0U || Height_in * TabCount > MaxHeight)
		Height_in = (UINT)floorf(MaxHeight / TabCount) - 1;

	for (Iter = m_Tabs.begin(); Iter != m_Tabs.end(); ++Iter)
	{
		pTab = Iter->second;
		pHeader = pTab->GetHeader();
		Index = pTab->GetTabIndex();
		// compute the new position and size
		pHeader->GetWindowRect(HeaderRect);
		pHeader->GetParent()->ScreenToClient(HeaderRect);
		Diff = HeaderRect.Height() - Height_in;
		// shift the header depending on its index
		if (Index > 0)
		{
			HeaderRect.bottom -= Diff * (Index + 1);
			HeaderRect.top -= Diff * Index;
		}
		else
			HeaderRect.bottom -= Diff;
		// move the header
		pHeader->SetWindowPos(NULL, HeaderRect.left, HeaderRect.top,
							  HeaderRect.Width(), HeaderRect.Height(),
							  SWP_NOZORDER | SWP_NOACTIVATE);
		// update the sizes
		pTab->UpdateSize();
	}
}