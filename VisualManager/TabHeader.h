/**************************************************************************
	created		:	2011-03-17
	filename	: 	TabHeader.h
	author		:	Xebeth`
	copyright	:	North Edge (2012)
	purpose		:	Tab header for tabbed dialogs
**************************************************************************/
#ifndef _VISUAL_MANAGER_SDK_INCLUDE_
	#error Please include the global header file 'VisualManager.h'
#endif

#ifndef __TAB_HEADER_H__
#define __TAB_HEADER_H__

/*! \brief Tab header for tabbed dialogs */
class APPVIMGR_API TabHeader : public CMFCButton
{
	// MFC runtime class macro
	DECLARE_DYNAMIC(TabHeader)
public:
	TabHeader();

	void InitFont(CFont* pFont_in);

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct_in);

	//! Background color of the button in its inactive state
	COLORREF m_InactiveBackground;
	//! Border color of the button in its inactive state
	COLORREF m_InactiveBorder;
	//! Border color of the button in its active state
	COLORREF m_ActiveBorder;
	//! Background color of the button in its active state
	COLORREF m_ActiveBackground;
	//! Background color of the button in its pushed state
	COLORREF m_PushedBackground;
	//! Border color of the button in its pushed state
	COLORREF m_PushedBorder;
	//! Font for the selected state (bold)
	CFont m_ActiveFont;
	//! Font for the normal state
	CFont m_InactiveFont;
	//! Color of the text
	COLORREF m_Arrow;
};

#endif//__TAB_HEADER_H__