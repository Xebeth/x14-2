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

// #define _DUMP_HTML

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
	: m_hParentWnd(NULL), m_hIEServer(NULL), m_Settings(Settings_in), m_UserSet(false),
	  m_pPasswordInput(NULL), m_pFormIterator(NULL), m_pHTMLDoc(NULL),
	  m_LoginComplete(false), m_PasswordSet(false), m_pUserInput(NULL),
	  m_pTokenInput(NULL)
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
			if (GetHTMLDocument(5000) && m_pHTMLDoc != NULL)
			{
				// check if the document is ready
				if (IsStatus(m_pHTMLDoc, _T("complete")))
				{
					// try to auto-complete the login form
					if (m_PasswordSet == false && AutoCompleteForm())
						SetFocusOnTokenInput();

					Sleep(100);
				}
				else if (WaitUntilDocumentComplete(m_pHTMLDoc, 10000))
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
			string_t Username = m_Settings.GetUsername();

			// the one time password input hasn't been found yet
			if (m_pTokenInput == NULL)
			{
				// look for it in the current form
				m_pTokenInput = FindChildById(pCurrentForm, _T("inputOTP"));
			}

			// the user input hasn't been found yet
			if (Username.empty() == false && m_pUserInput == NULL)
			{
				// look for it in the current form
				pElement = FindChildById(pCurrentForm, _T("sqexid"));

				if (pElement != NULL && SUCCEEDED(pElement->QueryInterface(IID_IHTMLInputElement, (LPVOID*)&m_pUserInput)) && m_pUserInput != NULL)
					m_UserSet = SetInputValue(m_pUserInput, Username.c_str());
			}

			// the password input hasn't been found yet
			if (m_pPasswordInput == NULL)
			{
				// look for it in the current form
				pElement = FindChildById(pCurrentForm, _T("passwd"));

				if (pElement != NULL)
				{
					if (SUCCEEDED(pElement->QueryInterface(IID_IHTMLInputElement, (LPVOID*)&m_pPasswordInput)) && m_pPasswordInput != NULL)
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
								m_LoginComplete = m_PasswordSet = SetInputValue(m_pPasswordInput, Password.c_str());
							}
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
			HRESULT hr = pElemDispatch->QueryInterface(IID_IHTMLElementCollection, (LPVOID*)&pElements);

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
						if (SUCCEEDED(pInputDispatch->QueryInterface(IID_IHTMLElement, (LPVOID*)&pElement)) && pElement != NULL)
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

/*! \brief Fills a field with the specified value
	\param[in,out] pInput_in_out : the input field to fill
	\param[in] pPassword_in : the value of the field
	\return true if the field was filled successfully; false otherwise
*/
bool AutoLogin::SetInputValue(IHTMLInputElement *pInput_in_out, const TCHAR *pValue_in)
{
	bool Result = false;

	if (pValue_in != NULL && pInput_in_out != NULL)
	{
		BSTR Value = SysAllocString(pValue_in);

		if ((Result = SUCCEEDED(pInput_in_out->put_value(Value))) == false)
		{
			pInput_in_out->Release();
			pInput_in_out = NULL;
		}

		SysFreeString(Value);
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
bool AutoLogin::WaitUntilDocumentComplete(IHTMLDocument2 *pDoc_in, long Timeout_in)
{
	bool Result = false;

	if (pDoc_in != NULL)
	{
		while (Timeout_in >= 0 && Result == false)
		{
			Result = IsStatus(pDoc_in, _T("complete"));

			if (Result == false)
			{
				Timeout_in -= 250;
				Sleep(250);
			}
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
				Result = (SUCCEEDED((*fnObjectFromLRESULT)(lRes, IID_IHTMLDocument2, 0, (LPVOID*)&m_pHTMLDoc)) && m_pHTMLDoc != NULL);

				if (Result == false)
				{
					Timeout_in -= 250;
					Sleep(250);
				}
			}
		}

		if (Result)
		{
			// find the login iframe	
			IHTMLFramesCollection2 *pIFrames = NULL;
			// wait for the document to load
			WaitUntilDocumentComplete(m_pHTMLDoc, Timeout_in);

			if (SUCCEEDED(m_pHTMLDoc->get_frames(&pIFrames)) && pIFrames != NULL)
			{
				VARIANT Index, FrameItem;

				Index.vt = VT_I4;
				Index.lVal = 0;

				FrameItem.vt = VT_DISPATCH;
				FrameItem.pdispVal = NULL;

				if (SUCCEEDED(pIFrames->item(&Index, &FrameItem)) && FrameItem.pdispVal != NULL)
				{
					IHTMLWindow2 *pFrame = NULL;
					IHTMLDocument2 *pDoc = NULL;

					if (SUCCEEDED(FrameItem.pdispVal->QueryInterface(IID_IHTMLWindow2, (LPVOID*)&pFrame)) && pFrame != NULL
					 && SUCCEEDED(pFrame->get_document(&pDoc)) && pDoc != NULL)
					{
						m_pHTMLDoc->Release();
						m_pHTMLDoc = pDoc;

						WaitUntilDocumentComplete(m_pHTMLDoc, Timeout_in);

#if defined _DEBUG && defined _DUMP_HTML
						IPersistFile* pFile = NULL;

						if(SUCCEEDED(m_pHTMLDoc->QueryInterface(IID_IPersistFile, (void**)&pFile)))
						{
							LPCOLESTR file = L"ffxivloginframe.htm";
							pFile->Save(file, TRUE);
						}
#endif // _DEBUG
						// cleanup
						pFrame->Release();
					}
					// cleanup
					FrameItem.pdispVal->Release();
				}
				// cleanup
				pIFrames->Release();
			}
		}
	}

	return Result;
}

/*! \brief Checks if the specified status matches the status of the document
	\param[in] pStatus_in : the status to check against the status of the document
	\return true if the statuses match
*/
bool AutoLogin::IsStatus(IHTMLDocument2 *pDoc_in, const TCHAR *pStatus_in)
{
	return (pStatus_in != NULL && UpdateDocumentState(pDoc_in) 
		 && m_DocumentState.compare(pStatus_in) == 0);
}

/*! \brief Retrieves the status of the document
	\return true if the status was retrieved successfully; false otherwise
*/
bool AutoLogin::UpdateDocumentState(IHTMLDocument2 *pDoc_in)
{
	bool Result = false;

	if (pDoc_in != NULL)
	{
		BSTR DocState = SysAllocString(_T(""));

		if (SUCCEEDED(pDoc_in->get_readyState(&DocState)))
		{
			m_DocumentState = DocState;
			Result = true;
		}

		SysFreeString(DocState);
	}

	return Result;
}

/*! \brief Sets the focus on the one time password input
	\return true if the focus was set; false otherwise
*/
bool AutoLogin::SetFocusOnTokenInput()
{
	IHTMLElement2 *pTokenInput = NULL;
	bool Result = false;

	if (m_pTokenInput != NULL)
	{
		if (SUCCEEDED(m_pTokenInput->QueryInterface(IID_IHTMLElement2, (LPVOID*)&pTokenInput)) && pTokenInput != NULL)
		{
			Result = (pTokenInput->focus() == S_OK);

			pTokenInput->Release();
			pTokenInput = NULL;
		}
		// we're done here
		if (Result)
		{
			m_pTokenInput->Release();
			m_pTokenInput = NULL;
		}
	}

	return Result;
}