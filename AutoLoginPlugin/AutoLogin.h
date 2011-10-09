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

namespace Windower
{
	class AutoLoginSettings;
}

class HTMLFormIterator;

struct IHTMLInputElement;
struct IConnectionPoint;
struct IHTMLFormElement;
struct IHTMLDocument2;
struct IHTMLElement;

class AutoLogin
{
public:
	AutoLogin(Windower::AutoLoginSettings *pSettings_in);
	~AutoLogin();

	void MonitorForms();
	void OnSubmit();

protected:
	bool SetPasswordInput(const TCHAR *pPassword_in);
	bool AutoCompleteForm();

	IHTMLElement* FindChildById(IHTMLElement* pParent_in, const TCHAR *pID_in);
	bool WaitUntilDocumentComplete(long Timeout_in);
	bool IsStatus(const TCHAR *pStatus_in);
	bool UpdateDocumentState();
	bool SetEventSink(IHTMLElement *pForm_in);
	bool RemoveEventSink();

	HWND GetIEServerWindow(long Timeout_in);
	bool GetHTMLDocument(long Timeout_in);
	FARPROC GetObjectFromLParamAddr();

	IHTMLInputElement *m_pPasswordInput;
	IHTMLFormElement *m_pLoginForm;
	IHTMLDocument2 *m_pHTMLDoc;

	Windower::AutoLoginSettings *m_pSettings;
	HTMLFormIterator *m_pFormIterator;
	string_t m_DocumentState;
	string_t m_FormPassword;
	string_t m_CurrentForm;
	bool m_LoginComplete;
	bool m_PasswordSet;

	IConnectionPoint* m_pConnectionPoint;
	HTMLEventSink m_EventSink;
	DWORD m_dwCookie;

	HWND m_hParentWnd;
	HWND m_hIEServer;
	
};

#endif//__AUTO_LOGIN_H__