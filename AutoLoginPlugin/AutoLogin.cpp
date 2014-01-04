/**************************************************************************
	created		:	2011-07-03
	filename	: 	AutoLogin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Monitors the forms during the login process
					and automatically fills the password field
**************************************************************************/
#include "stdafx.h"
#include <MsHTML.h>
#include <ExDisp.h>
#include <afxdlgs.h>

#include <PluginPropertyPage.h>

#include "CryptUtils.h"
#include "AutoLogin.h"

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
		ThreadData *pThreadData = reinterpret_cast<ThreadData*>(pUserData_in);

		if (pThreadData->m_pSettings != NULL && ::IsWindow(pThreadData->m_hParentWnd))
		{
			AutoLogin Login(*pThreadData->m_pSettings, pThreadData->m_hParentWnd);

			Login.MonitorForms();

			return 0L;
		}
	}

	return -1;
}

/*! \brief AutoLogin constructor
	\param[in] Settings_in : the settings of the AutoLogin plugin
*/
AutoLogin::AutoLogin(Windower::WindowerProfile &Settings_in, HWND hParentWnd_in)
	: m_hParentWnd(hParentWnd_in), m_hIEServer(NULL), m_Settings(Settings_in), m_bLoop(true),
	  m_PasswordSet(false), m_AutoSubmitted(false),  m_UserSet(false),
	  m_pFormIterator(NULL), m_pIFrameDoc(NULL), m_pPageDoc(NULL)
{
	// initialize COM
	::CoInitialize(NULL);
}

//! \brief AutoLogin destructor
AutoLogin::~AutoLogin()
{
	ResetForms();
	// shutdown COM
	::CoUninitialize();
}

//! \brief Monitors the forms during the login process and automatically fills the password field
void AutoLogin::MonitorForms()
{
	m_hIEServer = GetIEServerWindow(5000);

	if (m_hIEServer != NULL)
	{
		while (m_bLoop || IsStatus(m_pIFrameDoc, _T("complete")) == false)
		{
			if (GetHTMLDocument(5000) && IsStatus(m_pIFrameDoc, _T("complete")))
			{
				// try to auto-complete the login form
				if (m_UserSet == false && m_PasswordSet == false)
					AutoCompleteForm();
				// go easy on the CPU
				Sleep(500);
			}
		}

		// cleanup
		if (m_pFormIterator != NULL)
			m_pFormIterator->Release();
		if (m_pIFrameDoc != NULL)
			m_pIFrameDoc->Release();
		if (m_pPageDoc != NULL)
			m_pPageDoc->Release();

		ResetForms();
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
		m_pFormIterator = new HTMLFormIterator(*m_pIFrameDoc);

	while (m_pFormIterator->End() == false)
	{
		pCurrentForm = m_pFormIterator->Next();

		if (pCurrentForm != NULL)
		{
			string_t Username = m_Settings.GetUsername();
			IHTMLInputElement *pInputElement = NULL;

			// the user input hasn't been found yet
			if (Username.empty() == false && m_UserSet == false)
			{
				// look for it in the current form
				pElement = FindChildById(pCurrentForm, _T("sqexid"));

				if (pElement != NULL)
				{
					if (SUCCEEDED(pElement->QueryInterface(IID_IHTMLInputElement, (LPVOID*)&pInputElement)) && pInputElement != NULL)
					{
						m_UserSet = SetInputValue(pInputElement, Username.c_str());

						pInputElement->Release();
						pInputElement = NULL;
					}
					else
					{
						m_bLoop = m_AutoSubmitted = false;
						m_pIFrameDoc->Release();
						m_pIFrameDoc = NULL;
					}

					pElement->Release();
					pElement = NULL;
				}
			}

			// the password input hasn't been found yet
			if (m_UserSet && m_PasswordSet == false)
			{
				// look for it in the current form
				pElement = FindChildById(pCurrentForm, _T("password"));

				if (pElement != NULL)
				{
					if (SUCCEEDED(pElement->QueryInterface(IID_IHTMLInputElement, (LPVOID*)&pInputElement)) && pInputElement != NULL)
					{
						string_t Key;				
						long KeyHash;

						// retrieve the key used to encrypt the password
						CryptUtils::GenerateMachineID(Key, _T("C:\\"));
						KeyHash = CryptUtils::Hash(Key);

						// check if the key hashes match
						if (m_Settings.GetKeyHash() == KeyHash)
						{
							string_t CryptedPassword;

							// retrieve the password from the settings
							CryptUtils::HexToString(m_Settings.GetCryptedPassword(), CryptedPassword);

							if (CryptedPassword.empty() == false)
							{
								string_t Password;

								CryptUtils::Crypt(Key, CryptedPassword, Password);
								m_PasswordSet = SetInputValue(pInputElement, Password.c_str());
							}
						}

 						pInputElement->Release();
						pInputElement = NULL;
					}
					else
					{
						m_bLoop = m_AutoSubmitted = false;
						m_pIFrameDoc->Release();
						m_pIFrameDoc = NULL;
					}

					pElement->Release();
					pElement = NULL;
				}

				if (m_AutoSubmitted == false && m_UserSet && m_PasswordSet)
				{
					if (m_Settings.IsAutoSubmitted())
					{
						// auto-submit the form
						pElement = FindChildById(pCurrentForm, _T("btLogin"));

						if (pElement != NULL)
						{
							pElement->click();
							pElement->Release();
							m_AutoSubmitted = true;
						}
						else
						{
							m_AutoSubmitted = false;
							m_pIFrameDoc->Release();
							m_pIFrameDoc = NULL;
						}
					}
					else
					{
						// look for the one-time password field in the current form
						pElement = FindChildById(pCurrentForm, _T("otppw"));

						if (pElement != NULL)
						{
							IHTMLElement2 *pFocusInput = NULL;

							if (SUCCEEDED(pElement->QueryInterface(IID_IHTMLElement2, (LPVOID*)&pFocusInput)) && pFocusInput != NULL)
							{
								pFocusInput->focus();
								pFocusInput->Release();
								m_AutoSubmitted = true;
							}
							else
							{
								m_AutoSubmitted = false;
								m_pIFrameDoc->Release();
								m_pIFrameDoc = NULL;
							}

							pElement->Release();
						}
					}
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

/*! \brief 
*/
IHTMLDocument2* AutoLogin::GetIFrameDocument(long Timeout_in)
{
	IHTMLFramesCollection2 *pIFrames = NULL;
	IHTMLDocument2 *pDoc = NULL;

	if (SUCCEEDED(m_pPageDoc->get_frames(&pIFrames)) && pIFrames != NULL)
	{
		VARIANT Index, FrameItem;
		long Count = 0L;

		Index.vt = VT_I4;
		Index.lVal = 0;

		FrameItem.vt = VT_DISPATCH;
		FrameItem.pdispVal = NULL;

		pIFrames->get_length(&Count);

		if (Count > 0L && SUCCEEDED(pIFrames->item(&Index, &FrameItem)) && FrameItem.pdispVal != NULL)
		{
			IHTMLWindow2 *pFrame = NULL;

			if (SUCCEEDED(FrameItem.pdispVal->QueryInterface(IID_IHTMLWindow2, (LPVOID*)&pFrame)) && pFrame != NULL)
			{
				HRESULT hRes = pFrame->get_document(&pDoc);

				// if the access to the iframe document is denied, we'll have to work around cross-frame restrictions
				if (hRes == E_ACCESSDENIED)
				{
					IServiceProvider *pProvider = NULL;

					// retrieve the service provider interface
					if (SUCCEEDED(pFrame->QueryInterface(IID_IServiceProvider, (LPVOID*)&pProvider)) && pProvider != NULL)
					{
						IWebBrowser2 *pBrowser = NULL;

						// retrieve the web browser service
						if (SUCCEEDED(pProvider->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (LPVOID*)&pBrowser)) && pBrowser != NULL)
						{
							IDispatch *pDispatch = NULL;

							// retrieve the document
							if (SUCCEEDED(pBrowser->get_Document(&pDispatch)) && pDispatch != NULL)
							{
								pDispatch->QueryInterface(IID_IHTMLDocument2, (LPVOID*)&pDoc);
								pDispatch->Release();
							}

							pBrowser->Release();
						}

						pProvider->Release();
					}
				}
				// cleanup
				pFrame->Release();
			}
			// cleanup
			FrameItem.pdispVal->Release();
		}
		// cleanup
		pIFrames->Release();
	}

	return pDoc;
}

/*! \brief Retrieves the HTML document
	\param[in] Timeout_in : a timeout value
	\return true if the document was found; false otherwise
*/
bool AutoLogin::GetHTMLDocument(long Timeout_in)
{
	bool Result = false;

	if (m_hIEServer != NULL)
	{
		LPFNOBJECTFROMLRESULT fnObjectFromLRESULT = GetObjectFromLParamAddr();

		if (fnObjectFromLRESULT != NULL)
		{
			UINT nMsg = ::RegisterWindowMessage(_T("WM_HTML_GETOBJECT"));
			LRESULT lRes = 0;

			while (Timeout_in >= 0 && Result == false)
			{
				::SendMessageTimeout(m_hIEServer, nMsg, 0L, 0L, SMTO_ABORTIFHUNG, 200, (DWORD*)&lRes);
				Result = (SUCCEEDED((*fnObjectFromLRESULT)(lRes, IID_IHTMLDocument2, 0, (LPVOID*)&m_pPageDoc)) && m_pPageDoc != NULL);

				if (Result == false)
				{
					Timeout_in -= 250;
					Sleep(250);
				}
			}
		}

		if (Result)
		{
			// wait for the document to load
			WaitUntilDocumentComplete(m_pPageDoc, Timeout_in);

#if defined _DEBUG && defined _DUMP_HTML
			IPersistFile* pFile = NULL;

			if(SUCCEEDED(m_pPageDoc->QueryInterface(IID_IPersistFile, (void**)&pFile)))
			{
				LPCOLESTR file = L"c:\\ffxivlogin.htm";
				pFile->Save(file, TRUE);
			}
#endif // _DEBUG

			// try to change the language combo => seems to have no effect (event not triggered?)
			IHTMLElement *pBody = NULL, *pElement = NULL;

			if (m_pIFrameDoc == NULL && SUCCEEDED(m_pPageDoc->get_body(&pBody)) && pBody != NULL)
			{
				pElement = FindChildById(pBody, _T("langSelect"));

				if (pElement != NULL)
				{
					IHTMLSelectElement *pLngCombo = NULL;

					if (SUCCEEDED(pElement->QueryInterface(IID_IHTMLSelectElement, (LPVOID*)&pLngCombo)) && pLngCombo != NULL)
					{
						pLngCombo->put_selectedIndex(m_Settings.GetLanguage());
						pLngCombo->Release();
					}

					pElement->Release();
				}

				// cleanup
				pBody->Release();
			}

			// find the login iframe
			IHTMLDocument2 *pDoc = GetIFrameDocument(Timeout_in);

			// update the document of the iframe
			if (pDoc != NULL)
			{
				if (pDoc != m_pIFrameDoc)
				{
					if (m_pIFrameDoc != NULL)
						m_pIFrameDoc->Release();
					// assign the document
					m_pIFrameDoc = pDoc;
					ResetForms();
					// wait for the iframe to load
					WaitUntilDocumentComplete(m_pIFrameDoc, Timeout_in);

#if defined _DEBUG && defined _DUMP_HTML
					IPersistFile* pFile = NULL;

					if(SUCCEEDED(m_pIFrameDoc->QueryInterface(IID_IPersistFile, (void**)&pFile)))
					{
						LPCOLESTR file = L"c:\\ffxivlogin-frame.htm";
						pFile->Save(file, TRUE);
					}
#endif // _DEBUG
				}
				else
					pDoc->Release();
			}
		}
	}

	return (m_pIFrameDoc != NULL);
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

void AutoLogin::ResetForms()
{
	// reset the form
	m_UserSet = m_PasswordSet = false;
	delete m_pFormIterator;
	m_pFormIterator = NULL;
	m_bLoop = true;
}