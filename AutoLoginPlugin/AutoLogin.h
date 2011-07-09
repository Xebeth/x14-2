/**************************************************************************
	created		:	2011-07-03
	filename	: 	AutoLogin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#ifndef __AUTO_LOGIN_H__
#define __AUTO_LOGIN_H__

#define IE_SERVER_CLASSNAME _T("Internet Explorer_Server")

DWORD WINAPI AutoLoginThread(LPVOID pParam);

class HTMLFormIterator;
struct IHTMLAnchorElement;
struct IHTMLInputElement;
struct IHTMLFormElement;
struct IHTMLDocument2;
struct IHTMLElement;

class AutoLogin
{
public:
	AutoLogin(HWND hParentWnd_in) : m_hParentWnd(hParentWnd_in), m_hIEServer(NULL),
		m_LoginComplete(false), m_pPasswordInput(NULL), m_pSubmitAnchor(NULL),
		m_pFormIterator(NULL), m_pHTMLDoc(NULL) {}

	void MonitorForms();

protected:
	bool SetPasswordInput(IHTMLElement *pElement_in, const TCHAR *pPassword_in);
	IHTMLElement* FindChildById(IHTMLElement* pParent_in, const TCHAR *pID_in);
	
	bool IsStatus(const TCHAR *pStatus_in);
	bool UpdateDocumentState();
	
	bool WaitUntilDocumentComplete(long Timeout_in);
	HWND GetIEServerWindow(long Timeout_in);
	bool GetHTMLDocument(long Timeout_in);
	FARPROC GetObjectFromLParamAddr();

	bool FillPassword();

	IHTMLAnchorElement *m_pSubmitAnchor;
	IHTMLInputElement *m_pPasswordInput;
	HTMLFormIterator *m_pFormIterator;
	IHTMLDocument2 *m_pHTMLDoc;
	bool m_LoginComplete;
	BSTR m_DocumentState;

	HWND m_hParentWnd;
	HWND m_hIEServer;
	
};

#endif//__AUTO_LOGIN_H__