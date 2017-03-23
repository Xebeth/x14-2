/**************************************************************************
	created		:	2013-05-04
	filename	: 	EditLinkTarget.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "LauncherApp.h"
#include "EditLinkTarget.h"

IMPLEMENT_DYNAMIC(EditLinkTarget, CEdit)

BEGIN_MESSAGE_MAP(EditLinkTarget, CEdit)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

//! \brief EditLinkTarget default constructor
EditLinkTarget::EditLinkTarget() : m_pToolTip(nullptr),
	m_pfnCallback(nullptr), m_pCallbackObj(nullptr) {}

EditLinkTarget::~EditLinkTarget()
{
	if (m_pToolTip != nullptr)
		CTooltipManager::DeleteToolTip(m_pToolTip);
}

void EditLinkTarget::OnDropFiles(HDROP hDropInfo)
{
	size_t AllocSize = (DragQueryFile(hDropInfo, 0, nullptr, 0U) + 1) * sizeof(TCHAR);
	TCHAR *pName = nullptr;

	if (Buffer::Realloc(&pName, AllocSize, true))
	{
		// is it a shortcut?
		if (DragQueryFile(hDropInfo, 0, pName, _MAX_PATH) && _tcsstr(pName, _T(".lnk")) != nullptr)
		{
			size_t Length;
			CString LinkTarget;			

			LauncherApp::ResolveLink(m_hWnd, pName, LinkTarget);
			Length = LinkTarget.GetLength() + 1;

			if (Buffer::Realloc(&pName, Length + 1, true))
				_tcscpy_s(pName, Length, LinkTarget);
		}

		if (m_pfnCallback != nullptr && m_pCallbackObj != nullptr
		 && (m_pCallbackObj->*m_pfnCallback)(&pName))
		{
			SetWindowText(pName);
		}

		free(pName);
	}
	
	DragFinish(hDropInfo);	
}

void EditLinkTarget::SetCallback(fnOnDrop pCallback_in, CWnd *pObj_in)
{
	if (pCallback_in != nullptr && pObj_in != nullptr)
	{
		m_pfnCallback = pCallback_in;
		m_pCallbackObj = pObj_in;
	}
}

/*! \brief (MFC)Used to translate window messages before they are dispatched to the
		   TranslateMessage and DispatchMessage Windows functions.
	\param[in] pMsg_in : Pointer to a MSG structure that contains the message to process.
	\return Non-zero if the message was translated and should not be dispatched;
			0 if the message was not translated and should be dispatched.
*/
BOOL EditLinkTarget::PreTranslateMessage(MSG* pMsg_in)
{
	if (m_pToolTip != nullptr)
	{
		switch(pMsg_in->message)
		{
		case WM_MOUSEMOVE:
			if (m_pToolTip->GetSafeHwnd() != nullptr)
				m_pToolTip->RelayEvent(pMsg_in);
			break;
		}
	}

	return CEdit::PreTranslateMessage(pMsg_in);
}

/*! \brief Sets the text of the button and its tooltip
	\param[in] Text_in : the text of the tooltip
*/
void EditLinkTarget::SetTooltip(const CString &Text_in)
{
	// destroy the previous tooltip
	if (m_pToolTip != nullptr)
		CTooltipManager::DeleteToolTip(m_pToolTip);
	// create the tooltip
	CTooltipManager::CreateToolTip(m_pToolTip, this, AFX_TOOLTIP_TYPE_BUTTON);
	// set the tooltip text
	if (m_pToolTip != nullptr)
		m_pToolTip->AddTool(this, Text_in);
}