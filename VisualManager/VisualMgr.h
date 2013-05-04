/**************************************************************************
	created		:	2013-05-03
	filename	: 	VisualMgr.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef _VISUAL_MANAGER_SDK_INCLUDE_
	#error Please include the global header file 'VisualManager.h'
#endif

#ifndef __VISUAL_MGR_H__
#define __VISUAL_MGR_H__

#ifdef _AFX_PACKING
	#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
	#pragma component(minrebuild, off)
#endif

// fixed DPI value
#define DLG_FIXED_DPI 96

//!	\brief default visual manager
class APPVIMGR_API VisualManager : public CMFCVisualManagerOffice2003
{
	DECLARE_DYNCREATE(VisualManager)

public:
	/*! \brief List of application style
	 *	\remarks There is only one style for the moment
	 */
	enum eStyle
	{
		STYLE_DEFAULT, //!< Default style
	};

	virtual void OnUpdateSystemColors();

	static bool DisableDwmNcRendering(HWND hWnd_in);
	virtual BOOL IsOwnerDrawCaption() { return CanDrawImage(); }

	virtual BOOL OnNcActivate(CWnd* pWnd, BOOL bActive);
	virtual BOOL OnNcPaint(CWnd* pWnd, const CObList& lstSysButtons, CRect rectRedraw);
	virtual BOOL OnSetWindowRegion(CWnd* pWnd, CSize sizeWindow);

	virtual CSize GetNcBtnSize(BOOL bSmall) const;

	CMFCToolBarImages* GetCloseButtonImages()		{ return m_SysBtnClose; }
	CMFCToolBarImages* GetRestoreButtonImages()		{ return m_SysBtnRestore; }
	CMFCToolBarImages* GetMaximizeButtonImages()	{ return m_SysBtnMaximize; }
	CMFCToolBarImages* GetMinimizeButtonImages()	{ return m_SysBtnMinimize; }
	// Tab dialog header
	COLORREF GetTabDialogHeaderInactiveBackgroundColor(void) { return m_clrTabDialogHeaderInactiveBackground; }
	COLORREF GetTabDialogHeaderActiveBackgroundColor(void) { return m_clrTabDialogHeaderActiveBackground; }
	COLORREF GetTabDialogHeaderPushedBackgroundColor(void) { return m_clrTabDialogHeaderPushedBackground; }
	COLORREF GetTabDialogHeaderInactiveBorderColor(void) { return m_clrTabDialogHeaderInactiveBorder; }
	COLORREF GetTabDialogHeaderActiveBorderColor(void) { return m_clrTabDialogHeaderActiveBorder; }
	COLORREF GetTabDialogHeaderPushedBorderColor(void) { return m_clrTabDialogHeaderPushedBorder; }
	COLORREF GetTabDialogHeaderArrowColor(void) { return m_clrTabDialogHeaderArrow; }
	// Tab dialog page
	COLORREF GetTabDialogPageBackgroundColor(void) { return m_clrTabDialogPageBackground; }
	COLORREF GetTabDialogPageBorderColor(void) { return m_clrTabDialogPageBorder; }

	CMFCControlRenderer* GetSysBtnBack() { return m_SysBtnBack; }

	static VisualManager* __stdcall GetInstance() \
	{ return DYNAMIC_DOWNCAST(VisualManager, CMFCVisualManager::GetInstance()); }

	BOOL DrawPushButton(CDC* pDC, CRect rect, CMFCButton* pButton, UINT uiState);
	BOOL DrawParentBackground(CWnd* pWnd, CDC* pDC, LPRECT rectClip);

	void DrawButtonFrame(CDC* pDC_in, const CRect &Rect_in, bool IsDefault_in = false,
						 bool IsPressed_in = false, bool IsHighlighted_in = false,
						 bool IsDisabled_in = false, bool IsChecked_in = false,
						 bool IsFlat_in = false, bool NoBorder_in = false) const;

	void DrawButtonText(CDC* pDC_in, const CString &Text_in, COLORREF Color_in,
						const CRect &Rect_in, int AlignStyle_in, bool IsDisabled_in);

	static COLORREF ColorAverage(COLORREF Color1_in, COLORREF Color2_in);
	static COLORREF GrayAverage(COLORREF Color_in);
	static BOOL MapToGray(CDC *pDC, HBITMAP& hBmp);

	BOOL IsOwnerDrawMenuCheck();
	BOOL IsHighlightWholeMenuItem();

	virtual COLORREF GetStatusBarPaneTextColor(CMFCStatusBar* pStatusBar, CMFCStatusBarPaneInfo* pPane);
	virtual COLORREF GetToolbarButtonTextColor(CMFCToolBarButton* pButton, CMFCVisualManager::AFX_BUTTON_STATE state);
	virtual COLORREF GetMenuItemTextColor(CMFCToolBarMenuButton* pButton, BOOL bHighlighted, BOOL bDisabled);
	virtual COLORREF GetHighlightedMenuItemTextColor(CMFCToolBarMenuButton* pButton);

	virtual void OnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip, BOOL bNCArea = FALSE);
	virtual void OnFillHighlightedArea(CDC* pDC, CRect rect, CBrush* pBrush, CMFCToolBarButton* pButton);
	virtual void OnDrawBarGripper(CDC* pDC, CRect rectGripper, BOOL bHorz, CBasePane* pBar);
	virtual void OnDrawSeparator(CDC* pDC, CBasePane* pBar, CRect rect, BOOL bIsHoriz);
	virtual COLORREF OnDrawMenuLabel(CDC* pDC, CRect rect);
	virtual COLORREF OnDrawPaneCaption(CDC* pDC, CDockablePane* pBar, BOOL bActive, CRect rectCaption, CRect rectButtons);
	virtual COLORREF OnFillCaptionBarButton(CDC* pDC, CMFCCaptionBar* pBar, CRect rect,
		BOOL bIsPressed, BOOL bIsHighlighted, BOOL bIsDisabled, BOOL bHasDropDownArrow, BOOL bIsSysButton);
	virtual void OnDrawTabContent(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd, COLORREF clrText);

	virtual int GetShowAllMenuItemsHeight(CDC* pDC, const CSize& sizeDefault);
	virtual void OnDrawShowAllMenuItems(CDC* pDC, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);

	virtual void OnDrawStatusBarPaneBorder(CDC* pDC, CMFCStatusBar* pBar, CRect rectPane, UINT uiID, UINT nStyle);
	virtual void OnDrawStatusBarSizeBox(CDC* pDC, CMFCStatusBar* pStatBar, CRect rectSizeBox);

	virtual void OnDrawComboDropButton(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted, CMFCToolBarComboBoxButton* pButton);
	virtual void OnDrawComboBorder(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted, CMFCToolBarComboBoxButton* pButton);
	virtual void OnDrawEditBorder(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsHighlighted, CMFCToolBarEditBoxButton* pButton);
	virtual void OnDrawTearOffCaption(CDC* pDC, CRect rect, BOOL bIsActive);
	virtual void OnDrawMenuResizeBar(CDC* pDC, CRect rect, int nResizeFlags);
	virtual void OnDrawMenuScrollButton(CDC* pDC, CRect rect, BOOL bIsScrollDown, BOOL bIsHighlited, BOOL bIsPressed, BOOL bIsDisabled);
	virtual void OnDrawMenuSystemButton(CDC* pDC, CRect rect, UINT uiSystemCommand, UINT nStyle, BOOL bHighlight);	

	virtual void OnFillButtonInterior(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);
	virtual void OnDrawButtonBorder(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);
	virtual void OnDrawButtonSeparator(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state, BOOL bHorz);

	virtual void OnHighlightMenuItem(CDC *pDC, CMFCToolBarMenuButton* pButton, CRect rect, COLORREF& clrText);
	virtual void OnHighlightRarelyUsedMenuItems(CDC* pDC, CRect rectRarelyUsed);
	virtual void OnDrawMenuCheck(CDC* pDC, CMFCToolBarMenuButton* pButton, CRect rect, BOOL bHighlight, BOOL bIsRadio);

	virtual void OnDrawMenuItemButton(CDC* pDC, CMFCToolBarMenuButton* pButton, CRect rectButton, BOOL bHighlight, BOOL bDisabled);

	// MiniFrame:
	virtual COLORREF OnFillMiniFrameCaption(CDC* pDC, CRect rectCaption, CPaneFrameWnd* pFrameWnd, BOOL bActive);
	virtual void OnDrawMiniFrameBorder(CDC* pDC, CPaneFrameWnd* pFrameWnd, CRect rectBorder, CRect rectBorderSize);
	virtual void OnDrawFloatingToolbarBorder(	CDC* pDC, CMFCBaseToolBar* pToolBar, CRect rectBorder, CRect rectBorderSize);

	// tabs
	virtual void GetTabFrameColors(const CMFCBaseTabCtrl* pTabWnd, COLORREF& clrDark, COLORREF& clrBlack,
		COLORREF& clrHighlight, COLORREF& clrFace, COLORREF& clrDarkShadow, COLORREF& clrLight, CBrush*& pbrFace, CBrush*& pbrBlack);
	virtual void OnEraseTabsArea(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd);
	virtual BOOL OnEraseTabsFrame(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd);
	virtual void OnEraseTabsButton(CDC* pDC, CRect rect, CMFCButton* pButton, CMFCBaseTabCtrl* pWndTab);
	virtual void OnDrawTabsButtonBorder(CDC* pDC, CRect& rect, CMFCButton* pButton, UINT uiState, CMFCBaseTabCtrl* pWndTab);
	virtual void OnDrawTab(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd);
	virtual void OnFillTab(CDC* pDC, CRect rectFill, CBrush* pbrFill, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd);
	virtual BOOL AlwaysHighlight3DTabs() const { return CanDrawImage() ? TRUE : CMFCVisualManagerOffice2003::AlwaysHighlight3DTabs(); }
	virtual COLORREF GetTabTextColor(const CMFCBaseTabCtrl* pTabWnd, int iTab, BOOL bIsActive);
	virtual int GetTabHorzMargin(const CMFCBaseTabCtrl* pTabWnd);

	virtual void OnDrawCaptionBarInfoArea(CDC* pDC, CMFCCaptionBar* pBar, CRect rect);
	virtual COLORREF GetCaptionBarTextColor(CMFCCaptionBar* pBar);

	virtual COLORREF GetToolbarDisabledTextColor()
	{
		return CanDrawImage() ? m_clrToolBarBtnTextDisabled : CMFCVisualManagerOffice2003::GetToolbarDisabledTextColor();
	}

	virtual void OnDrawTasksGroupCaption(CDC* pDC, CMFCTasksPaneTaskGroup* pGroup, BOOL bIsHighlighted = FALSE, BOOL bIsSelected = FALSE, BOOL bCanCollapse = FALSE);
	virtual void OnDrawTask(CDC* pDC, CMFCTasksPaneTask* pTask, CImageList* pIcons, BOOL bIsHighlighted = FALSE, BOOL bIsSelected = FALSE);
	virtual void OnDrawScrollButtons(CDC* pDC, const CRect& rect, const int nBorderSize,int iImage, BOOL bHilited);
	virtual void OnDrawHeaderCtrlBorder(CMFCHeaderCtrl* pCtrl, CDC* pDC, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted);
	virtual void OnDrawCheckBoxEx(CDC *pDC, CRect rect, int nState, BOOL bHighlighted, BOOL bPressed,BOOL bEnabled);

	virtual void OnFillPopupWindowBackground(CDC* pDC, CRect rect);
	virtual COLORREF OnDrawPopupWindowCaption(CDC* pDC, CRect rectCaption, CMFCDesktopAlertWnd* pPopupWnd);
	virtual BOOL OnEraseMDIClientArea(CDC* pDC, CRect rectClient);

	// ToolTip
	virtual BOOL GetToolTipInfo(CMFCToolTipInfo& params, UINT nType = (UINT)(-1));

	virtual AFX_SMARTDOCK_THEME GetSmartDockingTheme();

	// Dialog
	const COLORREF& GetDialogInactiveCaptionLightColor(void) const { return m_clrDialogInactiveCaptionLight; }	// m_clrDialogInactiveCaptionLight
	const COLORREF& GetDialogInactiveCaptionDarkColor(void) const { return m_clrDialogInactiveCaptionDark; }	// m_clrDialogInactiveCaptionDark
	const COLORREF& GetDialogActiveCaptionLightColor(void) const { return m_clrDialogActiveCaptionLight; }		// m_clrDialogActiveCaptionLight
	const COLORREF& GetDialogActiveCaptionDarkColor(void) const { return m_clrDialogActiveCaptionDark; }		// m_clrDialogActiveCaptionDark
	const COLORREF& GetDialogInactiveCaptionTextColor(void) const { return m_clrDialogInactiveCaptionText; }	// m_clrDialogInactiveCaptionText
	const COLORREF& GetDialogActiveCaptionTextColor(void) const { return m_clrDialogActiveCaptionText; }		// m_clrDialogActiveCaptionText
	const COLORREF& GetDialogCaptionBorderColor(void) const { return m_clrDialogCaptionBorder; }				// m_clrDialogCaptionBorder
	const COLORREF& GetDialogBackgroundColor(void) const { return m_clrDialogBackground; }						// m_clrDialogBackground
	const COLORREF& GetDialogFrameColor(void) const { return m_clrDialogFrame; }								// m_clrDialogFrame

	// Toolbars
	const COLORREF& GetToolBarGradientLightColor(void) const { return m_clrToolBarGradientLight; }				// m_clrToolBarGradientLight
	const COLORREF& GetToolBarTitleTextColor(void) const { return m_clrToolBarTitleTextColor; }					// m_clrToolBarTitleTextColor
	const COLORREF& GetToolBarTitleLineColor(void) const { return m_clrToolBarTitleLineColor; }					// m_clrToolBarTitleLineColor

	// Menu
	const COLORREF& GetMenuBarButtonTextColor(void) const { return m_clrMenuBarBtnText; }						// m_clrMenuBarBtnText

	// Button
	const COLORREF& GetDisabledButtonColor(void) { return m_clrDisabledButton; }								// m_clrDisabledButton

	/************************************************************************/
	/*                        Static functions                              */
	/************************************************************************/
	static BOOL __stdcall SetStyle(eStyle Style_in, LPCTSTR lpszPath = NULL);
	static eStyle __stdcall GetStyle();
	static void __stdcall SetResourceHandle(HINSTANCE hinstRes);
	static void __stdcall CleanStyle();

	static bool __stdcall IsDPI(int DPI_in);
	static bool __stdcall GetDPI(int &DPIX_out, int &DPIY_out);

	/*************************/
	/*    PROTECTED BLOCK    */
	/*************************/
protected:
	VisualManager(void);
	virtual ~VisualManager(void);

	BOOL CanDrawImage() const
	{
		return afxGlobalData.m_nBitsPerPixel > 8 && !afxGlobalData.IsHighContrastMode() && m_bLoaded;
	}

	static CString __stdcall GetStyleResourceID(eStyle Style_in, const TCHAR *pszResID_in_out);

	virtual void CleanUp();

	void DrawSeparator(CDC* pDC, const CRect& rect, BOOL bHorz);
	void DrawSeparator(CDC* pDC, const CRect& rect, CPen& pen1, CPen& pen2, BOOL bHorz);
	void DrawNcBtn(CDC* pDC, const CRect& rect, UINT nButton, AFX_BUTTON_STATE state, BOOL bSmall, BOOL bActive, BOOL bMDI = FALSE);
	void DrawNcText(CDC* pDC, CRect& rect, const CString& strTitle, const CString& strDocument, BOOL bPrefix,
					BOOL bActive, BOOL bIsRTL, BOOL bTextCenter, BOOL bGlass = FALSE, int nGlassGlowSize = 0, COLORREF clrGlassText = (COLORREF)-1);
	void DrawNcCaption(CDC* pDC, CRect rectCaption, DWORD dwStyle, DWORD dwStyleEx, const CString& strTitle,
					const CString& strDocument, HICON hIcon, BOOL bPrefix, BOOL bActive, BOOL bTextCenter, const CObList& lstSysButtons);

	bool LoadXML(LPCTSTR lpszResID, CTagManager & TagManager_out, CString & StylePrefix_out) const;
	bool LoadGlobalsStyleRes(LPCTSTR lpszResID_in);
	bool LoadMainWndStyleRes(LPCTSTR lpszResID_in);
	bool LoadBarsStyleRes(LPCTSTR lpszResID_in);
	bool LoadPopupStyleRes(LPCTSTR lpszResID_in);
	bool LoadDialogStyleRes(LPCTSTR lpszResID_in);
	bool LoadMenuStyleRes(LPCTSTR lpszResID_in);
	bool LoadTabsStyleRes(LPCTSTR lpszResID_in);
	bool LoadEditStyleRes(LPCTSTR lpszResID_in);
	bool LoadComboStyleRes(LPCTSTR lpszResID_in);
	bool LoadHeaderStyleRes(LPCTSTR lpszResID_in);
	bool LoadTabDialogStyleRes(LPCTSTR lpszResID_in);

	bool IsFillBarBackgroundIsHandled(const CBasePane *pBar_in);
	bool IsDrawComboDropButtonIsHandled(const CMFCToolBarComboBoxButton* pButton_in);

	bool DrawMenuSystemButton(CDC* pDC, CRect rect, UINT uiSystemCommand, UINT nStyle, BOOL bHighlight);
	void DrawCheckBoxEx(CDC *pDC, CRect rect, int nState, BOOL bHighlighted, BOOL bPressed, BOOL bEnabled);
	BOOL NcPaint(CWnd* pWnd, const CObList& lstSysButtons, CRect rectRedraw);


	bool LoadResources(void);

	BOOL m_bLoaded;
	BOOL m_bNcTextCenter;
	BOOL m_bToolTipParams;

	CSize m_szNcBtnSize[2];

	CMFCToolBarImages m_ToolBarGripper;
	CMFCToolBarImages m_ToolBarTear;
	CMFCToolBarImages m_StatusBarPaneBorder;
	CMFCToolBarImages m_StatusBarSizeBox;
	CMFCToolBarImages m_SysBtnClose[2];
	CMFCToolBarImages m_SysBtnRestore[2];
	CMFCToolBarImages m_SysBtnMaximize[2];
	CMFCToolBarImages m_SysBtnMinimize[2];
	CMFCToolBarImages m_PopupResizeBar_HV;
	CMFCToolBarImages m_PopupResizeBar_HVT;
	CMFCToolBarImages m_PopupResizeBar_V;
	CMFCToolBarImages m_MenuItemMarkerC;
	CMFCToolBarImages m_MenuItemMarkerR;
	CMFCToolBarImages m_RibbonPanelSeparator;

	CMFCControlRenderer m_ctrlToolBarBorder;
	CMFCControlRenderer m_ctrlStatusBarBack;
	CMFCControlRenderer m_ctrlStatusBarBack_Ext;
	CMFCControlRenderer m_SysBtnBack[2];
	CMFCControlRenderer m_ctrlComboBoxBtn;
	CMFCControlRenderer m_ctrlMainBorder;
	CMFCControlRenderer m_ctrlMDIChildBorder;
	CMFCControlRenderer m_ctrlMainBorderCaption;
	CMFCControlRenderer m_ctrlPopupBorder;
	CMFCControlRenderer m_ctrlPopupResizeBar;
	CMFCControlRenderer m_ctrlMenuBarBtn;
	CMFCControlRenderer m_ctrlMenuItemBack;
	CMFCControlRenderer m_ctrlMenuItemShowAll;
	CMFCControlRenderer m_ctrlMenuHighlighted[2];
	CMFCControlRenderer m_ctrlMenuButtonBorder;
	CMFCControlRenderer m_ctrlMenuScrollBtn[2];
	CMFCControlRenderer m_ctrlToolBarBtn;
	CMFCControlRenderer m_ctrlTaskScrollBtn;
	CMFCControlRenderer m_ctrlTab3D[2];
	CMFCControlRenderer m_ctrlTabFlat[2];

	CPen   m_penTabFlatInner[2];
	CPen   m_penTabFlatOuter[2];
	CPen   m_penSeparator2;
	CPen   m_penSeparatorDark;
	CFont  m_AppCaptionFont;
	CBrush m_brMainClientArea;
	CBrush m_brGroupBackground;

	COLORREF m_clrAppCaptionActiveStart;
	COLORREF m_clrAppCaptionActiveFinish;
	COLORREF m_clrAppCaptionInactiveStart;
	COLORREF m_clrAppCaptionInactiveFinish;
	COLORREF m_clrAppCaptionActiveText;
	COLORREF m_clrAppCaptionInactiveText;
	COLORREF m_clrAppCaptionActiveTitleText;
	COLORREF m_clrAppCaptionInactiveTitleText;
	COLORREF m_clrMainClientArea;
	COLORREF m_clrMenuBarGradientLight;
	COLORREF m_clrMenuBarGradientLight2;
	COLORREF m_clrMenuBarGradientDark;
	COLORREF m_clrMenuBarGradientDark2;
	COLORREF m_clrMenuBarGradientVertLight;
	COLORREF m_clrMenuBarGradientVertDark;
	COLORREF m_clrMenuBarBtnText;
	COLORREF m_clrMenuBarBtnTextHighlighted;
	COLORREF m_clrMenuBarBtnTextDisabled;
	COLORREF m_clrToolBarBtnText;
	COLORREF m_clrToolBarBtnTextHighlighted;
	COLORREF m_clrToolBarBtnTextDisabled;
	COLORREF m_clrToolBarTitleTextColor;
	COLORREF m_clrToolBarTitleLineColor;
	COLORREF m_clrMenuText;
	COLORREF m_clrMenuTextHighlighted;
	COLORREF m_clrMenuTextDisabled;
	COLORREF m_clrStatusBarText;
	COLORREF m_clrStatusBarTextDisabled;
	COLORREF m_clrExtenedStatusBarTextDisabled;
	COLORREF m_clrEditBorder;
	COLORREF m_clrEditBorderDisabled;
	COLORREF m_clrEditBorderHighlighted;
	COLORREF m_clrEditSelection;
	COLORREF m_clrComboBorder;
	COLORREF m_clrComboBorderDisabled;
	COLORREF m_clrComboBorderPressed;
	COLORREF m_clrComboBorderHighlighted;
	COLORREF m_clrComboBtnStart;
	COLORREF m_clrComboBtnFinish;
	COLORREF m_clrComboBtnBorder;
	COLORREF m_clrComboBtnDisabledStart;
	COLORREF m_clrComboBtnDisabledFinish;
	COLORREF m_clrComboBtnBorderDisabled;
	COLORREF m_clrComboBtnPressedStart;
	COLORREF m_clrComboBtnPressedFinish;
	COLORREF m_clrComboBtnBorderPressed;
	COLORREF m_clrComboBtnHighlightedStart;
	COLORREF m_clrComboBtnHighlightedFinish;
	COLORREF m_clrComboBtnBorderHighlighted;
	COLORREF m_clrComboSelection;
	COLORREF m_clrHeaderNormalStart;
	COLORREF m_clrHeaderNormalFinish;
	COLORREF m_clrHeaderNormalBorder;
	COLORREF m_clrHeaderHighlightedStart;
	COLORREF m_clrHeaderHighlightedFinish;
	COLORREF m_clrHeaderHighlightedBorder;
	COLORREF m_clrHeaderPressedStart;
	COLORREF m_clrHeaderPressedFinish;
	COLORREF m_clrHeaderPressedBorder;
	COLORREF m_clrSeparator1;
	COLORREF m_clrSeparator2;
	COLORREF m_clrGroupText;
	COLORREF m_clrCaptionBarText;
	COLORREF m_clrPaneInactiveCaptionBorder;
	COLORREF m_clrPaneActiveCaptionBorder;
	COLORREF m_clrTaskPaneGroupCaptionHighDark;
	COLORREF m_clrTaskPaneGroupCaptionHighLight;
	COLORREF m_clrTaskPaneGroupCaptionHighSpecDark;
	COLORREF m_clrTaskPaneGroupCaptionHighSpecLight;
	COLORREF m_clrTaskPaneGroupCaptionTextSpec;
	COLORREF m_clrTaskPaneGroupCaptionTextHighSpec;
	COLORREF m_clrTaskPaneGroupCaptionText;
	COLORREF m_clrTaskPaneGroupCaptionTextHigh;
	COLORREF m_clrTabFlatBlack;
	COLORREF m_clrTabFlatHighlight;
	COLORREF m_clrTabTextActive;
	COLORREF m_clrTabTextInactive;
	COLORREF m_clrTabTextHighlight;
	COLORREF m_clrPopupGradientLight;
	COLORREF m_clrPopupGradientDark;
	// Dialog
	COLORREF m_clrDialogActiveCaptionText;
	COLORREF m_clrDialogInactiveCaptionText;
	COLORREF m_clrDialogActiveCaptionDark;
	COLORREF m_clrDialogActiveCaptionLight;
	COLORREF m_clrDialogInactiveCaptionDark;
	COLORREF m_clrDialogInactiveCaptionLight;
	COLORREF m_clrDialogCaptionBorder;
	COLORREF m_clrDialogBackground;
	COLORREF m_clrDialogFrame;
	// Buttons
	COLORREF m_clrButtonGradientLight;
	COLORREF m_clrButtonGradientDark;
	COLORREF m_clrDefaultButtonBorder;
	COLORREF m_clrDisabledButton;
	COLORREF m_clrPressedButton;
	COLORREF m_clrButtonBorder;
	// Tab dialog header
	COLORREF m_clrTabDialogHeaderInactiveBackground;
	COLORREF m_clrTabDialogHeaderActiveBackground;
	COLORREF m_clrTabDialogHeaderPushedBackground;
	COLORREF m_clrTabDialogHeaderInactiveBorder;
	COLORREF m_clrTabDialogHeaderActiveBorder;
	COLORREF m_clrTabDialogHeaderPushedBorder;
	COLORREF m_clrTabDialogHeaderArrow;
	// Tab dialog page
	COLORREF m_clrTabDialogPageBackground;
	COLORREF m_clrTabDialogPageBorder;
	
	CMFCToolTipInfo m_ToolTipParams;

	CMap<HWND, HWND, BOOL, BOOL> m_ActivateFlag;

	static eStyle    m_s_Style;
	static CString   m_s_strStylePrefix;
	static HINSTANCE m_s_hinstRes;
	static BOOL      m_s_bAutoFreeRes;

	BOOL IsWindowActive(CWnd* pWnd) const;
	CSize GetSystemBorders(void) const;
};

#ifdef _AFX_MINREBUILD
	#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
	#pragma pack(pop)
#endif

#endif//__VISUAL_MGR_H__