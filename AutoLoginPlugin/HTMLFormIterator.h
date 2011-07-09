/**************************************************************************
	created		:	2011-07-05
	filename	: 	HTMLFormIterator.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#ifndef __HTML_FORM_ITERATOR_H__
#define __HTML_FORM_ITERATOR_H__

class HTMLFormIterator
{
public:
	HTMLFormIterator(IHTMLDocument2 &Document_in)
		: m_Document(Document_in), m_pForms(NULL),
		  m_pCurrentForm(NULL), m_FormCount(0)
			{ InitForms(); }

	IHTMLElement* Next();
	bool End() { return (m_pForms != NULL && m_CurrentIndex.lVal >= m_FormCount); }
	void Release();
	void Reset();

private:
	long InitForms();

	IHTMLElementCollection *m_pForms;
	IHTMLElement *m_pCurrentForm;
	IHTMLDocument2 &m_Document;
	VARIANT m_CurrentIndex;
	long m_FormCount;
};

#endif//__HTML_FORM_ITERATOR_H__