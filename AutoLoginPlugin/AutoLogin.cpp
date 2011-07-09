/**************************************************************************
	created		:	2011-07-03
	filename	: 	AutoLogin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <MsHTML.h>

#include "HTMLFormIterator.h"
#include "AutoLogin.h"

typedef HRESULT (STDAPICALLTYPE *LPFNOBJECTFROMLRESULT)(LRESULT lResult, REFIID riid, WPARAM wParam, void** ppvObject);

DWORD WINAPI AutoLoginThread(LPVOID pParam)
{
	AutoLogin Login((HWND)pParam);

	CoInitialize(NULL);
	Login.MonitorForms();
	CoUninitialize();

	return 0L;
}

void AutoLogin::MonitorForms()
{
	IHTMLElement *pCurrentForm = NULL;
	IHTMLElement *pElement = NULL;
	BSTR CurrentID = _T("");

	m_hIEServer = GetIEServerWindow(5000);

	while (m_LoginComplete == false)
	{
		if (m_pHTMLDoc != NULL)
		{
			// check if the document is loading
			if (IsStatus(_T("complete")))
			{
				if (m_pFormIterator == NULL)
				{
					m_pFormIterator = new HTMLFormIterator(*m_pHTMLDoc);
#ifdef _DEBUG
					IPersistFile *pFile = NULL;

					if(SUCCEEDED(m_pHTMLDoc->QueryInterface(IID_IPersistFile, (void**)&pFile)))
					{
						LPCOLESTR file = L"f:\\ffxivlogin.html";
						pFile->Save(file, TRUE);
						pFile->Release();
					}
#endif // _DEBUG
				}

				if (m_pFormIterator->End() == false)
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
								SetPasswordInput(pElement, _T(""));
								m_LoginComplete = true;
							}
						}
					}
				}
				// monitor the forms every 500 ms
				Sleep(500);
			}
			else if (WaitUntilDocumentComplete(10000))
				if (m_pFormIterator != NULL)
					m_pFormIterator->Reset();
		}
		else 
			GetHTMLDocument(5000);
	}
	// cleanup
	m_pPasswordInput->Release();
	m_pFormIterator->Release();
	m_pHTMLDoc->Release();

	delete m_pFormIterator;
	m_pFormIterator = NULL;
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
				BSTR EltID = _T("");
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
						}

						pInputDispatch->Release();
					}
				}

				pElements->Release();
			}

			pElemDispatch->Release();
		}
	}

	return pElement;
}



bool AutoLogin::SetPasswordInput(IHTMLElement *pElement_in, const TCHAR *pPassword_in)
{
	bool Result = false;

	if (pPassword_in != NULL)
	{
		if (pElement_in != NULL)
		{
			if (pPassword_in != NULL && SUCCEEDED(pElement_in->QueryInterface(IID_IHTMLInputElement, (void**)&m_pPasswordInput)))
			{
				BSTR Passwd = SysAllocString(pPassword_in);

				if ((Result = SUCCEEDED(m_pPasswordInput->put_value(Passwd))) == false)
				{
					m_pPasswordInput->Release();
					m_pPasswordInput = NULL;
				}

				SysFreeString(Passwd);
			}

			pElement_in->Release();
		}
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
		 && _tcscmp(m_DocumentState, pStatus_in) == 0);
}

bool AutoLogin::UpdateDocumentState()
{
	m_DocumentState = _T("");

	return SUCCEEDED(m_pHTMLDoc->get_readyState(&m_DocumentState));
}