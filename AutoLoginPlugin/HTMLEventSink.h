/**************************************************************************
	created		:	2011-07-10
	filename	: 	HTMLEventSink.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	HTML event sink to handle form submission
**************************************************************************/
#ifndef __HTML_EVENT_SINK_H__
#define __HTML_EVENT_SINK_H__

class AutoLogin;

class HTMLEventSink : public IDispatch
{
public:
	HTMLEventSink() : m_refCount(0), m_pAutoLogin(NULL) {}

	STDMETHODIMP GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo ** pptinfo);
	STDMETHODIMP GetTypeInfoCount(UINT * pctinfo);
	STDMETHODIMP GetIDsOfNames( REFIID riid, OLECHAR ** rgszNames, UINT cNames, LCID lcid, DISPID * rgdispid);
	STDMETHODIMP QueryInterface(REFIID riid, void ** ppvObj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP Invoke(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
						DISPPARAMS * pdispparams, VARIANT * pvarResult,
						EXCEPINFO * pexcepinfo, UINT * puArgErr);

	void SetAutoLogin(AutoLogin *pAutoLogin_in) { m_pAutoLogin = pAutoLogin_in; }

protected:

	AutoLogin *m_pAutoLogin;
	int m_refCount;
};

#endif//__HTML_EVENT_SINK_H__