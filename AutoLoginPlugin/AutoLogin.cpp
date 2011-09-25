/**************************************************************************
	created		:	2011-07-03
	filename	: 	AutoLogin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <SettingsManager.h>
#include <MsHTML.h>

#include "HTMLEventSink.h"
#include "CryptUtils.h"
#include "AutoLogin.h"

#include "AutoLoginSettings.h"
#include "HTMLFormIterator.h"

typedef HRESULT (STDAPICALLTYPE *LPFNOBJECTFROMLRESULT)(LRESULT lResult, REFIID riid, WPARAM wParam, void** ppvObject);

DWORD WINAPI AutoLoginThread(LPVOID pParam)
{
	AutoLogin Login((Windower::AutoLoginSettings*)pParam);

	CoInitialize(NULL);
	Login.MonitorForms();
	CoUninitialize();

	return 0L;
}

AutoLogin::AutoLogin(Windower::AutoLoginSettings *pSettings_in)
	: m_hParentWnd(NULL), m_hIEServer(NULL), m_pLoginForm(NULL), m_pHTMLDoc(NULL),
	  m_pPasswordInput(NULL), m_pFormIterator(NULL), m_pSettings(pSettings_in),
	  m_LoginComplete(false), m_PasswordSet(false), m_pConnectionPoint(NULL),
	  m_dwCookie(0)
{
	m_EventSink.AddRef();

	if (m_pSettings != NULL)
		m_hParentWnd = pSettings_in->GetParentWnd();
}

AutoLogin::~AutoLogin()
{
	m_EventSink.Release();
}

void AutoLogin::MonitorForms()
{
	m_hIEServer = GetIEServerWindow(5000);

	if (m_hIEServer != NULL)
	{
		while (m_LoginComplete == false)
		{
			if (m_pHTMLDoc != NULL)
			{
				// check if the document is ready
				if (IsStatus(_T("complete")))
				{
					// try to auto-complete the login form
					if (m_PasswordSet == false)
						AutoCompleteForm();

					Sleep(100);
				}
				else if (WaitUntilDocumentComplete(10000))
				{
					if (m_pFormIterator != NULL)
					{
						m_pFormIterator->Reset();
						// invalidate the password
						m_pPasswordInput->Release();
						m_pPasswordInput = NULL;
						m_PasswordSet = false;
					}
				}
			}
			else 
				GetHTMLDocument(5000);
		}
		// cleanup
		m_pFormIterator->Release();
		m_pHTMLDoc->Release();

		delete m_pFormIterator;
		m_pFormIterator = NULL;
	}
}

void AutoLogin::OnSubmit()
{
	if (m_dwCookie != 0)
		RemoveEventSink();

	// the password wasn't set by the plugin
	if (m_PasswordSet == false && m_pPasswordInput != NULL)
	{
		BSTR Passwd = SysAllocString(_T(""));
		
		if (SUCCEEDED(m_pPasswordInput->get_value(&Passwd)))
		{
			// we retrieved a password so store it
			// at this point we're not sure if it's valid
			m_FormPassword = Passwd;
		}

		SysFreeString(Passwd);
	}
}

bool AutoLogin::AutoCompleteForm()
{
	IHTMLElement *pCurrentForm = NULL;
	IHTMLElement *pElement = NULL;

	if (m_pFormIterator == NULL)
		m_pFormIterator = new HTMLFormIterator(*m_pHTMLDoc);

	while (m_pFormIterator->End() == false)
	{
		pCurrentForm = m_pFormIterator->Next();

		if (pCurrentForm != NULL)
		{
			// the password input hasn't been found yet
			if (m_pPasswordInput == NULL)
			{
				// look for it in the current form
				pElement = FindChildById(pCurrentForm, _T("passwd"));

				if (pElement != NULL)
				{
					BSTR FormID = SysAllocString(_T(""));

					// retrieve the ID of the form
					pCurrentForm->get_id(&FormID);
					m_CurrentForm = FormID;
					SysFreeString(FormID);

					if (SUCCEEDED(pElement->QueryInterface(IID_IHTMLInputElement, (void**)&m_pPasswordInput)) && m_pPasswordInput != NULL)
					{
						string_t Key;
						long KeyHash;

						// retrieve the key used to encrypt the password
						m_Crypt.GenerateMachineID(Key);
						KeyHash = m_Crypt.Hash(Key);

						// check if the key hashes match
						if (m_pSettings->GetKeyHash() == KeyHash)
						{
							string_t CryptedPassword;

							// retrieve the password from the settings
							m_Crypt.HexToString(m_pSettings->GetPassword(), CryptedPassword);

							if (CryptedPassword.empty() == false)
							{
								string_t Password;

								m_Crypt.Crypt(Key, CryptedPassword, Password);
								m_PasswordSet = SetPasswordInput(m_pPasswordInput, Password.c_str());
							}
						}

						if (m_PasswordSet == false)
						{
							// we've made it so far but failed, give up
							m_PasswordSet = true;
							// reset the config
							m_pSettings->SetKeyHash(0);
							m_pSettings->SetAutoValidate(0);
							m_pSettings->SetPassword(_T(""));

							m_pSettings->Save();

							// since we couldn't set the password, try to retrieve it 
							// by listening to the submit event
							if (m_dwCookie == 0)
								SetEventSink(pCurrentForm);
						}

						m_pPasswordInput->Release();
						m_pPasswordInput = NULL;
					}

					pElement->Release();
				}
			}
		}
	}

	m_pFormIterator->Reset();

	return m_PasswordSet;
}

IHTMLElement* AutoLogin::FindChildById(IHTMLElement* pParent_in, const TCHAR *pID_in)
{
	IHTMLElement *pElement = NULL;

	if (pParent_in != NULL && pID_in != NULL)
	{
		IDispatch* pElemDispatch = NULL;

		if (SUCCEEDED(pParent_in->get_all(&pElemDispatch)) && pElemDispatch != NULL)
		{
			IHTMLElementCollection *pElements = NULL;
			HRESULT hr = pElemDispatch->QueryInterface(IID_IHTMLElementCollection, (void**)&pElements);

			if (SUCCEEDED(hr) && pElements != NULL)
			{
				IDispatch* pInputDispatch = NULL;
				BSTR EltID = SysAllocString(_T(""));
				long nLength = 0;
				VARIANT Index;

				pElements->get_length(&nLength);
				Index.vt = VT_I4;

				for (long i = 0; i < nLength; ++i)
				{
					pElement = NULL;
					Index.lVal = i;

					if (SUCCEEDED(pElements->item(Index, Index, &pInputDispatch)) && pInputDispatch != NULL)
					{
						if (SUCCEEDED(pInputDispatch->QueryInterface(IID_IHTMLElement, (void**)&pElement)) && pElement != NULL)
						{
							pElement->get_id(&EltID);

							if (EltID != NULL && _tcscmp(EltID, pID_in) == 0)
							{
								pInputDispatch->Release();
								break;
							}
							else
							{
								// not the element we seek, release it
								pElement->Release();
							}
						}

						pInputDispatch->Release();
					}
				}
				// cleanup
				SysFreeString(EltID);
				pElements->Release();
			}

			pElemDispatch->Release();
		}
	}

	return pElement;
}

bool AutoLogin::SetPasswordInput(IHTMLInputElement *pElement_in, const TCHAR *pPassword_in)
{
	bool Result = false;

	if (pPassword_in != NULL && pElement_in != NULL)
	{
		BSTR Passwd = SysAllocString(pPassword_in);

		if ((Result = SUCCEEDED(m_pPasswordInput->put_value(Passwd))) == false)
		{
			m_pPasswordInput->Release();
			m_pPasswordInput = NULL;
		}

		SysFreeString(Passwd);
	}

	return Result;
}

HWND AutoLogin::GetIEServerWindow(long Timeout_in)
{
	HWND Result = NULL;

	while (Timeout_in >= 0 && Result == NULL)
	{
		Result = FindWindowEx(m_hParentWnd, NULL, IE_SERVER_CLASSNAME, NULL);

		if (Result == NULL)
		{
			Timeout_in -= 250;
			Sleep(250);
		}
	}

	return Result;
}

bool AutoLogin::WaitUntilDocumentComplete(long Timeout_in)
{
	bool Result = false;

	while (Timeout_in >= 0 && Result == false)
	{
		Result = IsStatus(_T("complete"));

		if (Result == false)
		{
			Timeout_in -= 250;
			Sleep(250);
		}
	}

	return Result;
}

FARPROC AutoLogin::GetObjectFromLParamAddr()
{
	HINSTANCE hInst = ::LoadLibrary(_T("OleAcc.dll"));
	FARPROC pResult = NULL;

	if (hInst != NULL)
	{
		pResult = ::GetProcAddress(hInst, "ObjectFromLresult");

		::FreeLibrary(hInst);
	}

	return pResult;
}

bool AutoLogin::GetHTMLDocument(long Timeout_in)
{
	bool Result = false;

	if (m_pHTMLDoc == NULL && m_hIEServer != NULL)
	{
		LPFNOBJECTFROMLRESULT fnObjectFromLRESULT;

		fnObjectFromLRESULT = (LPFNOBJECTFROMLRESULT)GetObjectFromLParamAddr();

		if (fnObjectFromLRESULT != NULL)
		{
			UINT nMsg = ::RegisterWindowMessage(_T("WM_HTML_GETOBJECT"));
			LRESULT lRes = 0;

			while (Timeout_in >= 0 && Result == false)
			{
				::SendMessageTimeout(m_hIEServer, nMsg, 0L, 0L, SMTO_ABORTIFHUNG, 200, (DWORD*)&lRes);
				Result = SUCCEEDED((*fnObjectFromLRESULT)(lRes, IID_IHTMLDocument2, 0, (void**)&m_pHTMLDoc));

				if (Result == false)
				{
					Timeout_in -= 250;
					Sleep(250);
				}
			}
		}
	}

	return Result;
}

bool AutoLogin::IsStatus(const TCHAR *pStatus_in)
{
	return (pStatus_in != NULL && UpdateDocumentState() 
		 && m_DocumentState.compare(pStatus_in) == 0);
}

bool AutoLogin::UpdateDocumentState()
{
	BSTR DocState = SysAllocString(_T(""));
	bool Result = false;

	if (SUCCEEDED(m_pHTMLDoc->get_readyState(&DocState)))
	{
		m_DocumentState = DocState;
		Result = true;
	}

	SysFreeString(DocState);

	return Result;
}

bool AutoLogin::SetEventSink(IHTMLElement *pForm_in)
{
	// try to attach an onsubmit event
	IConnectionPointContainer* pCPC = NULL;
	bool Result = false;

	// Check that this is a connectable object.
	if (SUCCEEDED(pForm_in->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC)) && pCPC != NULL)        
	{
		// Find the connection point.
		if (SUCCEEDED(pCPC->FindConnectionPoint(DIID_HTMLFormElementEvents2, &m_pConnectionPoint)) && m_pConnectionPoint != NULL)
		{
			// pUnk is the IUnknown interface pointer for your event sink
			if (SUCCEEDED(m_pConnectionPoint->Advise(static_cast<IUnknown*>(&m_EventSink), &m_dwCookie)))
			{
				// Successfully advised
				m_EventSink.SetAutoLogin(this);

				Result = true;
			}

			m_pConnectionPoint->Release();
		}

		pCPC->Release();        
	}

	return Result;
}

bool AutoLogin::RemoveEventSink()
{
	if (m_dwCookie != 0 && m_pConnectionPoint != NULL)
	{
		m_pConnectionPoint->Unadvise(m_dwCookie);
		m_pConnectionPoint->Release();
		m_pConnectionPoint = NULL;
		m_dwCookie = 0;

		return true;
	}

	return false;
}