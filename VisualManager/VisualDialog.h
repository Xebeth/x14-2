/**************************************************************************
	created		:	2011-03-22
	filename	: 	VisualDialog.h
	author		:	Xebeth`
	copyright	:	North Edge (2012)
	purpose		:	CDialogEx derived class used as a base class for all the dialogs in the application
					It applies the main window theme to dialogs using the visual manager
**************************************************************************/
#ifndef _VISUAL_MANAGER_SDK_INCLUDE_
	#error Please include the global header file 'VisualManager.h'
#endif

#ifndef __VISUAL_DIALOG_H__
#define __VISUAL_DIALOG_H__

class VisualDialogImpl;
class CSetDPI;

/*! \brief CDialogEx derived class used to apply the main window theme to dialogs */
class APPVIMGR_API VisualDialog : public CDialogEx
{
	// VisualDialog is tightly coupled with its implementation
	friend class VisualDialogImpl;
	// MFC runtime class macro
	DECLARE_DYNAMIC(VisualDialog)
public:
	explicit VisualDialog(UINT TemplateID_in, CWnd *pParentWnd_in = NULL);
	virtual ~VisualDialog();

	afx_msg void OnSize(UINT nSizeType_in, int Width_in, int Height_in);
	virtual BOOL OnInitDialog(void);

	/*! \brief Checks if the dialog is active -- updated in OnNcActivate(...)
		\return TRUE if the dialog is active; FALSE otherwise
		\sa VisualDialog::OnNcActivate
	*/
	BOOL IsActive() { return m_bIsActive; }
	/*! \brief Checks if the control with the specified resource ID is the default button
		\param[in] ResID_in : a control resource ID
		\return return true if the control is the default button; false otherwise
	*/
	bool IsDefaultButton(UINT ResID_in) { return (m_DefaultButton == ResID_in); }

	virtual void UpdateControlText(UINT ControlID_in, const CString &Text_in,
								   bool Invalidate_in = true, bool ForceUpdate_in = false);
	virtual bool IsMFCButton(const CButton *pButton_in);
	virtual void SetDefaultButton(UINT ButtonResID_in);

	static void SetFixedDPI(HWND hWnd_in, UINT DialogTplID_in, int DPI_in, bool ForceAttach_in = false);
	static bool CreateImageList(CImageList &ImageList_in_out, UINT ResID_in, UINT IconCount_in,
								CSize IconSize_in, UINT ColorRes_in = ILC_COLOR32);

protected:
	/*! \brief Allows a derived class of VisualDialog to define the color of its static controls
			   This function is called in OnCtlColor(...)
		\param[in] pDC_in_out : contains a pointer to the display context for the child window (may be temporary)
		\param[in] pWnd_in_out : contains a pointer to the control asking for the color (may be temporary)
		\param[in] nControlID_in : the type of the control
		\return a handle to the background brush used to draw the static control
		\sa VisualDialog::OnCtlColor
	*/
	virtual HBRUSH OnStaticColor(CDC* pDC_in_out, CWnd* pWnd_in_out, UINT nControlID_in) const { return m_hTransparentBrush; }
	virtual void UpdateWindowRect(void);

	bool SetDefaultButton(CButton *pButton_in, bool Default_in = true);
	bool CreateImageList(UINT ResID_in, UINT IconCount_in, CSize IconSize_in, UINT ColorRes_in = ILC_COLOR32);

	afx_msg void OnNcCalcSize(BOOL bCalcValidRects_in, NCCALCSIZE_PARAMS FAR* pCalcSizeParams_in);
	afx_msg HBRUSH OnCtlColor(CDC* pDC_in_out, CWnd* pWnd_in_out, UINT nControlID_in);
	afx_msg void OnActivate(UINT nState_in, CWnd* pWndOther_in, BOOL bMinimized_in);
	afx_msg void OnNcLButtonDown(UINT nHitTest_in, CPoint MousePos_in);
	afx_msg void OnNcMouseMove(UINT nHitTest_in, CPoint MousePos_in);
	afx_msg void OnNcLButtonUp(UINT nHitTest_in, CPoint MousePos_in);
	afx_msg void OnMouseMove(UINT nFlags_in, CPoint MousePos_in);
	afx_msg LRESULT OnSetText (WPARAM wParam, LPARAM lParam); 
	afx_msg LRESULT OnNcHitTest(CPoint MousePos_in);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC_in_out);
	afx_msg BOOL OnNcActivate(BOOL bActive_in);
	afx_msg void OnNcPaint(void);

	DECLARE_MESSAGE_MAP()

	//! the width of the dialog frame
	int m_FrameWidth;
	//! the height of the dialog frame
	int m_FrameHeight;
	//! Transparent brush used to draw transparent items
	HBRUSH m_hTransparentBrush;
	//! Background color brush
	HBRUSH m_hBkgColor;
	//! String containing the title of the dialog
	CString m_DialogTitle;
	//! Rectangle representing the area of the caption bar
	CRect m_CaptionRect;
	//! Rectangle representing the client area
	CRect m_ClientRect;
	//! Rectangle representing the area of the frame
	CRect m_FrameRect;
	//! Rectangle representing the area of the dialog window
	CRect m_WindowRect;
	//! Font used to draw the title of the dialog in the caption bar
	CFont m_TitleFont;
	//! Flag specifying if the dialog is active or not
	BOOL m_bIsActive;
	//! a handle to the icon of the dialog
	HICON m_hIcon;
	//! The resource ID of the dialog icon
	UINT m_IconResID;
	//! background color of the dialog icon
	COLORREF m_IconBackground;
	//! icon border color
	COLORREF m_IconBorder;
	//! the resource ID of the default button
	UINT m_DefaultButton;
	//! image list for icons
	CImageList *m_pImageList;
	//! flag specifying if the window is a tool window
	bool m_bIsToolWindow;

private:
	//! the implementation of the dialog
	VisualDialogImpl *m_pImpl;
	//! forces the DPI resolution of the dialog
	static CSetDPI *m_pDPI;
};

#endif//__VISUAL_DIALOG_H__