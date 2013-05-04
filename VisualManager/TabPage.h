/**************************************************************************
	created		:	2011-03-21
	filename	: 	TabPage.h
	author		:	Xebeth`
	copyright	:	North Edge (2012)
	purpose		:	Tab page for tabbed dialogs
**************************************************************************/
#ifndef _VISUAL_MANAGER_SDK_INCLUDE_
	#error Please include the global header file 'VisualManager.h'
#endif

#ifndef __TAB_PAGE_H__
#define __TAB_PAGE_H__

class CSetDPI;

//! a map of static for which a mandatory mark (*) is set
typedef std::map<UINT, CStatic*> MandatoryMarks;
//! mandatory mark color
#define MANDATORY_FIELD_COLOR RGB(127, 0, 0)

/*! \brief Tab page for tabbed dialogs */
class APPVIMGR_API TabPage : public CDialogBar
{
	// MFC runtime class macro
	DECLARE_DYNAMIC(TabPage)
public:
	TabPage(UINT Width_in, UINT Height_in, UINT TabIndex_in);
	virtual ~TabPage();

	bool Create(CWnd *pParentWnd_in, const CRect &ContainerRect_in,
				UINT HeaderHeight_in, UINT nIDTemplate_in);
	void RecalcActiveTabBorder(void);

	/*! \brief Sets the height of the associated header 
		\param[in] HeaderHeight_in : the new height
	*/
	void SetHeaderHeight(UINT HeaderHeight_in) { m_HeaderHeight = HeaderHeight_in; }

	void SetFixedDPI(UINT DialogTplID_in, int DPI_in);

	virtual void SetMandatory(UINT FieldLabelID_in, bool Mandatory_in = true);
	//! \brief Callback called when the page is created
	virtual void OnCreate(void) { InitControls(); }
	//! \brief Callback called when the page is activated
	virtual void OnActivate(void) {}
	//! \brief Callback called when the page is deactivated
	virtual void OnDeactivate(void) {}
	//! \brief Initializes the controls contained in the page
	virtual void InitControls(void) {}

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC_in_out);
	afx_msg HBRUSH OnCtlColor(CDC* pDC_in_out, CWnd* pWnd_in_out, UINT nControlID_in);

	//! Transparent brush used to draw transparent items
	HBRUSH m_hTransparentBrush;
	//! Background color brush
	HBRUSH m_hBkgColor;
	//! Background color of the tab
	COLORREF m_Background;
	//! Border color of the tab
	COLORREF m_Border;
	//! Area representing the border of the tab
	CRect m_ActiveTabBorder;
	//! Area representing the content of the tab
	CRect m_ContainerRect;
	//! The index of the tab page
	UINT m_TabIndex;
	//! Height of the associated header
	UINT m_HeaderHeight;
	//! marks for mandatory fields
	MandatoryMarks m_Marks;
	//! mandatory mark resource ID
	UINT m_MandatoryID;

private:
	//! forces the DPI resolution of the dialog bar
	CSetDPI *m_pDPI;
};

#endif//__TAB_PAGE_H__