/**************************************************************************
	created		:	2013-05-04
	filename	: 	EditLinkTarget.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __EDIT_LINK_TARGET_H__
#define __EDIT_LINK_TARGET_H__

typedef bool (CWnd::*fnOnDrop)(TCHAR **pDropPath_in);

class EditLinkTarget : public CEdit
{
	DECLARE_DYNAMIC(EditLinkTarget)
	
public:
	EditLinkTarget();
	~EditLinkTarget();

	void SetCallback(fnOnDrop pCallback_in, CWnd *pObj_in);

	void SetTooltip(const CString &Text_in);
	BOOL PreTranslateMessage(MSG* pMsg_in);

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDropFiles(HDROP hDropInfo);

	//! Tooltip control
	CToolTipCtrl* m_pToolTip;
	fnOnDrop m_pfnCallback;
	CWnd *m_pCallbackObj;
};

#endif//__EDIT_LINK_TARGET_H__
