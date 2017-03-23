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
	if (pUserData_in != nullptr)
	{
		ThreadData *pThreadData = reinterpret_cast<ThreadData*>(pUserData_in);

		if (pThreadData->m_pSettings != nullptr && ::IsWindow(pThreadData->m_hParentWnd))
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
	: m_pIFrameDoc(nullptr), m_pPageDoc(nullptr), m_Settings(Settings_in), m_pFormIterator(nullptr),
	  m_PasswordSet(false), m_UserSet(false),  m_AutoSubmitted(false),
	  m_bLoop(true), m_hParentWnd(hParentWnd_in), m_hIEServer(nullptr)
{
	// initialize COM
	SUCCEEDED(::CoInitialize(nullptr));
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

	if (m_hIEServer != nullptr)
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
		if (m_pFormIterator != nullptr)
			m_pFormIterator->Release();
		if (m_pIFrameDoc != nullptr)
			m_pIFrameDoc->Release();
		if (m_pPageDoc != nullptr)
			m_pPageDoc->Release();

		ResetForms();
	}
}

/*! \brief Automatically fills the password field
	\return true if the password field was filled; false otherwise
*/
bool AutoLogin::AutoCompleteForm()
{
	IHTMLElement *pCurrentForm, *pElement;

	if (m_pFormIterator == nullptr)
		m_pFormIterator = new HTMLFormIterator(*m_pIFrameDoc);

	while (m_pFormIterator->End() == false)
	{
		pCurrentForm = m_pFormIterator->Next();

		if (pCurrentForm != nullptr)
		{
			string_t Username = m_Settings.GetUsername();
			IHTMLInputElement *pInputElement = nullptr;

			// the user input hasn't been found yet
			if (Username.empty() == false && m_UserSet == false)
			{
				// look for it in the current form
				pElement = FindChildById(pCurrentForm, _T("sqexid"));

				if (pElement != nullptr)
				{
					if (SUCCEEDED(pElement->QueryInterface(IID_IHTMLInputElement, reinterpret_cast<LPVOID*>(&pInputElement))) && pInputElement != nullptr)
					{
						m_UserSet = SetInputValue(pInputElement, Username.c_str());

						pInputElement->Release();
						pInputElement = nullptr;
					}
					else
					{
						m_bLoop = m_AutoSubmitted = false;
						m_pIFrameDoc->Release();
						m_pIFrameDoc = nullptr;
					}

					pElement->Release();
				}
			}

			// the password input hasn't been found yet
			if (m_UserSet && m_PasswordSet == false)
			{
				// look for it in the current form
				pElement = FindChildById(pCurrentForm, _T("password"));

				if (pElement != nullptr)
				{
					if (SUCCEEDED(pElement->QueryInterface(IID_IHTMLInputElement, reinterpret_cast<LPVOID*>(&pInputElement))) && pInputElement != nullptr)
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
						pInputElement = nullptr;
					}
					else
					{
						m_bLoop = m_AutoSubmitted = false;
						m_pIFrameDoc->Release();
						m_pIFrameDoc = nullptr;
					}

					pElement->Release();
				}

				if (m_AutoSubmitted == false && m_UserSet && m_PasswordSet)
				{
					if (m_Settings.IsAutoSubmitted())
					{
						// auto-submit the form
						pElement = FindChildById(pCurrentForm, _T("btLogin"));

						if (pElement != nullptr)
						{
							pElement->click();
							pElement->Release();
							m_AutoSubmitted = true;
						}
						else
						{
							m_AutoSubmitted = false;
							m_pIFrameDoc->Release();
							m_pIFrameDoc = nullptr;
						}
					}
					else
					{
						// look for the one-time password field in the current form
						pElement = FindChildById(pCurrentForm, _T("otppw"));

						if (pElement != nullptr)
						{
							IHTMLElement2 *pFocusInput = nullptr;

							if (SUCCEEDED(pElement->QueryInterface(IID_IHTMLElement2, reinterpret_cast<LPVOID*>(&pFocusInput))) && pFocusInput != nullptr)
							{
								pFocusInput->focus();
								pFocusInput->Release();
								m_AutoSubmitted = true;
							}
							else
							{
								m_AutoSubmitted = false;
								m_pIFrameDoc->Release();
								m_pIFrameDoc = nullptr;
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
	\return a pointer to the element if found; nullptr otherwise
*/
IHTMLElement* AutoLogin::FindChildById(IHTMLElement* pParent_in, const TCHAR *pID_in)
{
	IHTMLElement *pElement = nullptr;

	if (pParent_in != nullptr && pID_in != nullptr)
	{
		IDispatch* pElemDispatch = nullptr;

		if (SUCCEEDED(pParent_in->get_all(&pElemDispatch)) && pElemDispatch != nullptr)
		{
			IHTMLElementCollection *pElements = nullptr;
			HRESULT hr = pElemDispatch->QueryInterface(IID_IHTMLElementCollection, reinterpret_cast<LPVOID*>(&pElements));

			if (SUCCEEDED(hr) && pElements != nullptr)
			{
				IDispatch* pInputDispatch = nullptr;
				BSTR EltID = SysAllocString(_T(""));
				long nLength = 0;

				if (SUCCEEDED(pElements->get_length(&nLength)))
				{
					VARIANT Index;

					Index.vt = VT_I4;

					for (long i = 0; i < nLength; ++i)
					{
						pElement = nullptr;
						Index.lVal = i;

						if (SUCCEEDED(pElements->item(Index, Index, &pInputDispatch)) && pInputDispatch != nullptr)
						{
							if (SUCCEEDED(pInputDispatch->QueryInterface(IID_IHTMLElement, reinterpret_cast<LPVOID*>(&pElement))) && pElement != nullptr)
							{
								if (SUCCEEDED(pElement->get_id(&EltID)))
								{

									if (EltID != nullptr && _tcscmp(EltID, pID_in) == 0)
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
							}

							pInputDispatch->Release();
						}
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

	if (pValue_in != nullptr && pInput_in_out != nullptr)
	{
		BSTR Value = SysAllocString(pValue_in);

		if ((Result = SUCCEEDED(pInput_in_out->put_value(Value))) == false)
		{
			pInput_in_out->Release();
		}

		SysFreeString(Value);
	}

	return Result;
}

/*! \brief Retrieves the top window of the IE server
	\param[in] Timeout_in : a timeout value
	\return a handle on the window if found; nullptr otherwise
*/
HWND AutoLogin::GetIEServerWindow(long Timeout_in) const
{
	HWND Result = nullptr;

	while (Timeout_in >= 0 && Result == nullptr)
	{
		Result = FindWindowEx(m_hParentWnd, nullptr, IE_SERVER_CLASSNAME, nullptr);

		if (Result == nullptr)
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

	if (pDoc_in != nullptr)
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
	LPFNOBJECTFROMLRESULT pResult = nullptr;

	if (hInst != nullptr)
	{
		pResult = reinterpret_cast<LPFNOBJECTFROMLRESULT>(::GetProcAddress(hInst, "ObjectFromLresult"));

		::FreeLibrary(hInst);
	}

	return pResult;
}

/*! \brief 
*/
IHTMLDocument2* AutoLogin::GetIFrameDocument() const
{
	IHTMLFramesCollection2 *pIFrames = nullptr;
	IHTMLDocument2 *pDoc = nullptr;

	if (SUCCEEDED(m_pPageDoc->get_frames(&pIFrames)) && pIFrames != nullptr)
	{
		VARIANT Index, FrameItem;
		long Count = 0L;

		Index.vt = VT_I4;
		Index.lVal = 0;

		FrameItem.vt = VT_DISPATCH;
		FrameItem.pdispVal = nullptr;

		if (SUCCEEDED(pIFrames->get_length(&Count)) && Count > 0L
		 && SUCCEEDED(pIFrames->item(&Index, &FrameItem)) 
		 && FrameItem.pdispVal != nullptr)
		{
			IHTMLWindow2 *pFrame = nullptr;

			if (SUCCEEDED(FrameItem.pdispVal->QueryInterface(IID_IHTMLWindow2, reinterpret_cast<LPVOID*>(&pFrame))) && pFrame != nullptr)
			{
				HRESULT hRes = pFrame->get_document(&pDoc);

				// if the access to the iframe document is denied, we'll have to work around cross-frame restrictions
				if (hRes == E_ACCESSDENIED)
				{
					IServiceProvider *pProvider = nullptr;

					// retrieve the service provider interface
					if (SUCCEEDED(pFrame->QueryInterface(IID_IServiceProvider, reinterpret_cast<LPVOID*>(&pProvider))) && pProvider != nullptr)
					{
						IWebBrowser2 *pBrowser = nullptr;

						// retrieve the web browser service
						if (SUCCEEDED(pProvider->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, reinterpret_cast<LPVOID*>(&pBrowser))) && pBrowser != nullptr)
						{
							IDispatch *pDispatch = nullptr;

							// retrieve the document
							if (SUCCEEDED(pBrowser->get_Document(&pDispatch)) && pDispatch != nullptr)
							{
								pDispatch->QueryInterface(IID_IHTMLDocument2, reinterpret_cast<LPVOID*>(&pDoc));
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

	if (m_hIEServer != nullptr)
	{
		LPFNOBJECTFROMLRESULT fnObjectFromLRESULT = GetObjectFromLParamAddr();

		if (fnObjectFromLRESULT != nullptr)
		{
			UINT nMsg = ::RegisterWindowMessage(_T("WM_HTML_GETOBJECT"));
			LRESULT lRes = 0;

			while (Timeout_in >= 0 && Result == false)
			{
				::SendMessageTimeout(m_hIEServer, nMsg, 0L, 0L, SMTO_ABORTIFHUNG, 200, reinterpret_cast<DWORD_PTR*>(&lRes));
				Result = (SUCCEEDED((*fnObjectFromLRESULT)(lRes, IID_IHTMLDocument2, 0, reinterpret_cast<LPVOID*>(&m_pPageDoc))) && m_pPageDoc != nullptr);

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
			IPersistFile* pFile = nullptr;

			if(SUCCEEDED(m_pPageDoc->QueryInterface(IID_IPersistFile, (void**)&pFile)))
			{
				LPCOLESTR file = L"c:\\ffxivlogin.htm";
				pFile->Save(file, TRUE);
			}
#endif // _DEBUG

			// try to change the language combo => seems to have no effect (event not triggered?)
			IHTMLElement *pBody = nullptr, *pElement;

			if (m_pIFrameDoc == nullptr && SUCCEEDED(m_pPageDoc->get_body(&pBody)) && pBody != nullptr)
			{
				pElement = FindChildById(pBody, _T("langSelect"));

				if (pElement != nullptr)
				{
					IHTMLSelectElement *pLngCombo = nullptr;

					if (SUCCEEDED(pElement->QueryInterface(IID_IHTMLSelectElement, reinterpret_cast<LPVOID*>(&pLngCombo))) && pLngCombo != nullptr)
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
			IHTMLDocument2 *pDoc = GetIFrameDocument();

			// update the document of the iframe
			if (pDoc != nullptr)
			{
				if (pDoc != m_pIFrameDoc)
				{
					if (m_pIFrameDoc != nullptr)
						m_pIFrameDoc->Release();
					// assign the document
					m_pIFrameDoc = pDoc;
					ResetForms();
					// wait for the iframe to load
					WaitUntilDocumentComplete(m_pIFrameDoc, Timeout_in);

#if defined _DEBUG && defined _DUMP_HTML
					IPersistFile* pFile = nullptr;

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

	return (m_pIFrameDoc != nullptr);
}

/*! \brief Checks if the specified status matches the status of the document
	\param[in] pStatus_in : the status to check against the status of the document
	\return true if the statuses match
*/
bool AutoLogin::IsStatus(IHTMLDocument2 *pDoc_in, const TCHAR *pStatus_in)
{
	return (pStatus_in != nullptr && UpdateDocumentState(pDoc_in) 
		 && m_DocumentState.compare(pStatus_in) == 0);
}

/*! \brief Retrieves the status of the document
	\return true if the status was retrieved successfully; false otherwise
*/
bool AutoLogin::UpdateDocumentState(IHTMLDocument2 *pDoc_in)
{
	bool Result = false;

	if (pDoc_in != nullptr)
	{
		BSTR DocState = SysAllocString(_T(""));

		if (SUCCEEDED(pDoc_in->get_readyState(&DocState)))
		{
			m_DocumentState.assign(DocState);
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
	m_pFormIterator = nullptr;
	m_bLoop = true;
}