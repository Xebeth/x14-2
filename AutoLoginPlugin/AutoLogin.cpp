/**************************************************************************
	created		:	2011-07-03
	filename	: 	AutoLogin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Monitors the forms during the login process
					and automatically fills the password field
**************************************************************************/
#include "stdafx.h"
#include <SettingsManager.h>
#include <MsHTML.h>

#include "CryptUtils.h"
#include "AutoLogin.h"

#include "AutoLoginSettings.h"
#include "HTMLFormIterator.h"

/*! \brief global function used to start the working thread
	\param[in] pUserData_in : thread user data holding a pointer to the settings
	\return the thread exit code (0 on success; -1 otherwise)
*/
DWORD WINAPI AutoLoginThread(LPVOID pUserData_in)
{
	if (pUserData_in != NULL)
	{
		AutoLogin Login(*((Windower::AutoLoginSettings*)pUserData_in));

		Login.MonitorForms();

		return 0L;
	}

	return -1;
}

/*! \brief AutoLogin constructor
	\param[in] Settings_in : the settings of the AutoLogin plugin
*/
AutoLogin::AutoLogin(Windower::AutoLoginSettings &Settings_in)
	: m_hParentWnd(NULL), m_hIEServer(NULL), m_Settings(Settings_in),
	  m_pPasswordInput(NULL), m_pFormIterator(NULL), m_pHTMLDoc(NULL),
	  m_LoginComplete(false), m_PasswordSet(false)
{
	m_hParentWnd = Settings_in.GetParentWnd();
	// initialize COM
	::CoInitialize(NULL);
}

//! \brief AutoLogin destructor
AutoLogin::~AutoLogin()
{
	// shutdown COM
	::CoUninitialize();
}

//! \brief Monitors the forms during the login process and automatically fills the password field
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
						m_PasswordSet = false;
						// invalidate the password
						if (m_pPasswordInput != NULL)
						{
							m_pPasswordInput->Release();
							m_pPasswordInput = NULL;
						}
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

/*! \brief Automatically fills the password field
	\return true if the password field was filled; false otherwise
*/
bool AutoLogin::AutoCompleteForm()
{
	IHTMLElement *pCurrentForm = NULL;
	IHTMLElement *pElement = NULL;

	if (m_pFormIterator == NULL)
		m_pFormIterator = new HTMLFormIterator(*m_pHTMLDoc);

	while (m_PasswordSet == false && m_pFormIterator->End() == false)
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
					if (SUCCEEDED(pElement->QueryInterface(IID_IHTMLInputElement, (void**)&m_pPasswordInput)) && m_pPasswordInput != NULL)
					{
						string_t Key;
						long KeyHash;

						// retrieve the key used to encrypt the password
						CryptUtils::GenerateMachineID(Key);
						KeyHash = CryptUtils::Hash(Key);

						// check if the key hashes match
						if (m_Settings.GetKeyHash() == KeyHash)
						{
							string_t CryptedPassword;

							// retrieve the password from the settings
							CryptUtils::HexToString(m_Settings.GetPassword(), CryptedPassword);

							if (CryptedPassword.empty() == false)
							{
								string_t Password;

								CryptUtils::Crypt(Key, CryptedPassword, Password);
								m_PasswordSet = SetPasswordInput(Password.c_str());
							}
						}

						if (m_PasswordSet == false)
						{
							// we've made it so far but failed, give up
							m_PasswordSet = true;
							// reset the config
							m_Settings.SetKeyHash(0);
							m_Settings.SetPassword(_T(""));

							m_Settings.Save();
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

/*! \brief Retrieves and HTML element given its ID
	\param[in] pParent_in : the parent of the element
	\param[in] pID_in : the ID of the element
	\return a pointer to the element if found; NULL otherwise
*/
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

/*! \brief Fills the password field with the specified password
	\param[in] pPassword_in : the password
	\return true if the field was filled successfully; false otherwise
*/
bool AutoLogin::SetPasswordInput(const TCHAR *pPassword_in)
{
	bool Result = false;

	if (pPassword_in != NULL && m_pPasswordInput != NULL)
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

/*! \brief Retrieves the top window of the IE server
	\param[in] Timeout_in : a timeout value
	\return a handle on the window if found; NULL otherwise
*/
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

/*! \brief Waits until the document is complete
	\param[in] Timeout_in : a timeout value
	\return true if the document is complete; false otherwise
*/
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

/*! \brief Retrieves the function pointer on ObjectFromLresult
	\return the function pointer on ObjectFromLresult
*/
LPFNOBJECTFROMLRESULT AutoLogin::GetObjectFromLParamAddr()
{
	HINSTANCE hInst = ::LoadLibrary(_T("OleAcc.dll"));
	LPFNOBJECTFROMLRESULT pResult = NULL;

	if (hInst != NULL)
	{
		pResult = (LPFNOBJECTFROMLRESULT)::GetProcAddress(hInst, "ObjectFromLresult");

		::FreeLibrary(hInst);
	}

	return pResult;
}

/*! \brief Retrieves the HTML document
	\param[in] Timeout_in : a timeout value
	\return true if the document was found; false otherwise
*/
bool AutoLogin::GetHTMLDocument(long Timeout_in)
{
	bool Result = false;

	if (m_pHTMLDoc == NULL && m_hIEServer != NULL)
	{
		LPFNOBJECTFROMLRESULT fnObjectFromLRESULT = GetObjectFromLParamAddr();

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

/*! \brief Checks if the specified status matches the status of the document
	\param[in] pStatus_in : the status to check against the status of the document
	\return true if the statuses match
*/
bool AutoLogin::IsStatus(const TCHAR *pStatus_in)
{
	return (pStatus_in != NULL && UpdateDocumentState() 
		 && m_DocumentState.compare(pStatus_in) == 0);
}

/*! \brief Retrieves the status of the document
	\return true if the status was retrieved successfully; false otherwise
*/
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