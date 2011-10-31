/**************************************************************************
	created		:	2011-07-03
	filename	: 	AutoLogin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Monitors the forms during the login process
					and automatically fills the password field
**************************************************************************/
#ifndef __AUTO_LOGIN_H__
#define __AUTO_LOGIN_H__

//! the class name of the IE server
#define IE_SERVER_CLASSNAME _T("Internet Explorer_Server")
//! function pointer on ObjectFromLresult defined in OleAcc.dll
typedef HRESULT (STDAPICALLTYPE *LPFNOBJECTFROMLRESULT)(LRESULT lResult, REFIID riid, WPARAM wParam, LPVOID* ppvObject);

DWORD WINAPI AutoLoginThread(LPVOID pUserData_in);

// forward declarations
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

/*! \brief Monitors the forms during the login process
		   and automatically fills the password field
*/
class AutoLogin
{
public:
	explicit AutoLogin(Windower::AutoLoginSettings &Settings_in);
	~AutoLogin();

	void MonitorForms();

protected:
	bool SetPasswordInput(const TCHAR *pPassword_in);
	bool AutoCompleteForm();

	IHTMLElement* FindChildById(IHTMLElement* pParent_in, const TCHAR *pID_in);
	bool WaitUntilDocumentComplete(long Timeout_in);
	bool IsStatus(const TCHAR *pStatus_in);
	bool UpdateDocumentState();

	LPFNOBJECTFROMLRESULT GetObjectFromLParamAddr();
	HWND GetIEServerWindow(long Timeout_in);
	bool GetHTMLDocument(long Timeout_in);	

	//! the password field
	IHTMLInputElement *m_pPasswordInput;
	//! the HTML document
	IHTMLDocument2 *m_pHTMLDoc;

	//! the settings of the AutoLogin plugin
	Windower::AutoLoginSettings &m_Settings;
	//! an iterator of HTML forms
	HTMLFormIterator *m_pFormIterator;
	//! the state of the document
	string_t m_DocumentState;
	//! flag specifying if the login process is complete
	bool m_LoginComplete;
	//! flag specifying if the password field is set
	bool m_PasswordSet;
	//! the handle on the parent window
	HWND m_hParentWnd;
	//! the handle on the IE server window
	HWND m_hIEServer;
};

#endif//__AUTO_LOGIN_H__