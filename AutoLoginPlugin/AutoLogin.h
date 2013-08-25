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
	class WindowerProfile;
}

class HTMLFormIterator;

struct IHTMLInputElement;
struct IConnectionPoint;
struct IHTMLFormElement;
struct IHTMLDocument2;
struct IHTMLElement;

typedef struct _ThreadData
{
	Windower::WindowerProfile *m_pSettings;
	HWND m_hParentWnd;
} ThreadData;

/*! \brief Monitors the forms during the login process
		   and automatically fills the password field
*/
class AutoLogin
{
public:
	AutoLogin(Windower::WindowerProfile &Settings_in, HWND hParentWnd_in);
	~AutoLogin();

	void MonitorForms();

protected:
	bool SetInputValue(IHTMLInputElement *pInput_in_out, const TCHAR *pValue_in);
	bool AutoCompleteForm();

	IHTMLElement* FindChildById(IHTMLElement* pParent_in, const TCHAR *pID_in);
	bool WaitUntilDocumentComplete(IHTMLDocument2 *pDoc_in, long Timeout_in);
	bool IsStatus(IHTMLDocument2 *pDoc_in, const TCHAR *pStatus_in);
	bool UpdateDocumentState(IHTMLDocument2 *pDoc_in);

	IHTMLDocument2* GetIFrameDocument(long Timeout_in);
	LPFNOBJECTFROMLRESULT GetObjectFromLParamAddr();
	HWND GetIEServerWindow(long Timeout_in);
	bool GetHTMLDocument(long Timeout_in);	
	void ResetForms();

	//! the HTML document of the iFrame
	IHTMLDocument2 *m_pIFrameDoc;
	//! the HTML document of the iFrame
	IHTMLDocument2 *m_pPageDoc;

	//! the settings of the AutoLogin plugin
	Windower::WindowerProfile &m_Settings;
	//! an iterator of HTML forms
	HTMLFormIterator *m_pFormIterator;
	//! the state of the document
	string_t m_DocumentState;
	//! flag specifying if the password field is set
	bool m_PasswordSet;
	//! flag specifying if the username field is set
	bool m_UserSet;
	//! flag specifying if the auto-submit flag has been enforced
	bool m_AutoSubmitted;
	//! flag specifying if forms are monitored
	bool m_bLoop;
	//! the handle on the parent window
	HWND m_hParentWnd;
	//! the handle on the IE server window
	HWND m_hIEServer;
};

#endif//__AUTO_LOGIN_H__