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
	explicit HTMLFormIterator(IHTMLDocument2 &Document_in)
		: m_pForms(nullptr), m_pCurrentForm(nullptr),
		  m_Document(Document_in), m_FormCount(0)
	{
		InitForms();
	}

	IHTMLElement* Next();
	bool End() const { return (m_pForms != nullptr && m_CurrentIndex.lVal >= m_FormCount); }
	void Release() const;
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