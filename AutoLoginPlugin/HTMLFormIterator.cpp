/**************************************************************************
	created		:	2011-07-05
	filename	: 	HTMLFormIterator.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <MsHTML.h>

#include "HTMLFormIterator.h"

IHTMLElement* HTMLFormIterator::Next()
{
	if (m_pCurrentForm != NULL)
	{
		m_pCurrentForm->Release();
		m_pCurrentForm = NULL;
	}

	if (End() == false)
	{
		IDispatch* pElemDispatch = NULL;

		if (SUCCEEDED(m_pForms->item(m_CurrentIndex, m_CurrentIndex, &pElemDispatch)) && pElemDispatch != NULL)
		{
			pElemDispatch->QueryInterface(IID_IHTMLElement, (void**)&m_pCurrentForm);
			pElemDispatch->Release();

			m_CurrentIndex.lVal++;
		}		
	}

	return m_pCurrentForm;
}

long HTMLFormIterator::InitForms()
{
	m_CurrentIndex.vt = VT_I4;
	m_CurrentIndex.lVal = 0;

	if (SUCCEEDED(m_Document.get_forms(&m_pForms)) && m_pForms != NULL)
		m_pForms->get_length(&m_FormCount);

	return m_FormCount;
}

void HTMLFormIterator::Reset()
{
	m_CurrentIndex.vt = VT_I4;
	m_CurrentIndex.lVal = 0;
	
	if (m_pCurrentForm != NULL)
	{
		m_pCurrentForm->Release();
		InitForms();
	}
	
	m_pCurrentForm = NULL;
}

void HTMLFormIterator::Release()
{
	if (m_pCurrentForm != NULL)
		m_pCurrentForm->Release();

	if (m_pForms != NULL)
		m_pForms->Release();
}