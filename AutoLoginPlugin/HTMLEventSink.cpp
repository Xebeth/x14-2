/**************************************************************************
	created		:	2011-07-10
	filename	: 	HTMLEventSink.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <mshtmdid.h>
#include <MsHtml.h>

#include "HTMLEventSink.h"
#include "CryptUtils.h"
#include "AutoLogin.h"

STDMETHODIMP HTMLEventSink::GetTypeInfoCount(UINT * pctinfo)
{
	if (pctinfo == NULL)
		return E_INVALIDARG;
	else
		*pctinfo = 0;

	return S_OK;
}

STDMETHODIMP HTMLEventSink::GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo ** pptinfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP HTMLEventSink::GetIDsOfNames(REFIID riid, OLECHAR ** rgszNames, UINT cNames,
										  LCID lcid, DISPID * rgdispid)
{
	return E_NOTIMPL;
}

STDMETHODIMP HTMLEventSink::Invoke(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
								   DISPPARAMS * pdispparams, VARIANT * pvarResult,
								   EXCEPINFO * pexcepinfo, UINT * puArgErr)
{ 
	// catch the onsubmit event of the form
	if ((dispidMember == DISPID_IHTMLFORMELEMENT_SUBMIT || dispidMember == DISPID_IHTMLFORMELEMENT_ONSUBMIT) && m_pAutoLogin != NULL)
		m_pAutoLogin->OnSubmit();

	return S_OK;
}

STDMETHODIMP HTMLEventSink::QueryInterface(REFIID riid, void ** ppvObj)
{
	if (riid == IID_IUnknown)
		*ppvObj = static_cast<IUnknown*>(this);
	else if (riid == IID_IDispatch || riid == __uuidof(HTMLElementEvents2))
		*ppvObj = static_cast<IDispatch*>(this);
	else
		*ppvObj = NULL;

	if (*ppvObj != NULL)
	{
		static_cast<IUnknown*>(*ppvObj)->AddRef();

		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) HTMLEventSink::AddRef()
{
	return ++m_refCount;
}

STDMETHODIMP_(ULONG) HTMLEventSink::Release()
{

	if (--m_refCount == 0)
	{
		delete this;
		return 0;
	}

	return m_refCount;
}