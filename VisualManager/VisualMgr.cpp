/**************************************************************************
	created		:	2013-05-03
	filename	: 	VisualMgr.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#pragma region Includes

#include <afxcontrolbarimpl.h>
#include <afxcontrolbarutil.h>
#include <afxglobalutils.h>
#include <afxtoolbar.h>
#include <afxdrawmanager.h>
#include <afxpopupmenubar.h>
#include <afxmenubar.h>
#include <afxglobals.h>
#include <afxtoolbarmenubutton.h>
#include <afxcustomizebutton.h>
#include <afxmenuimages.h>
#include <afxcaptionbar.h>
#include <afxbasetabctrl.h>
#include <afxcolorbar.h>
#include <afxtabctrl.h>
#include <afxtaskspane.h>
#include <afxstatusbar.h>
#include <afxautohidebutton.h>
#include <afxheaderctrl.h>
#include <afxrebar.h>
#include <afxdesktopalertwnd.h>
#include <afxdropdowntoolbar.h>
#include <afxtagmanager.h>
#include <afxframewndex.h>
#include <afxmdiframewndex.h>
#include <afxdockablepane.h>
#include <afxoutlookbartabctrl.h>
#include <afxtoolbarcomboboxbutton.h>

#include <afxribbonbar.h>
#include <afxribbonpanel.h>
#include <afxribboncategory.h>
#include <afxribbonbutton.h>
#include <afxribbonquickaccesstoolbar.h>
#include <afxribboncombobox.h>
#include <afxribbonmainpanel.h>
#include <afxribbonpanelmenu.h>
#include <afxribbonlabel.h>
#include <afxribbonpalettegallery.h>
#include <afxribbonstatusbar.h>
#include <afxribbonstatusbarpane.h>
#include <afxribbonprogressbar.h>
#include <afxribbonlinkctrl.h>

#include <dwmapi.h>

#include "VisualMgr.h"
#include "VisualDialog.h"

#include "VisualDialog.h"
#include "OverlayTabCtrl.h"

extern CFrameWnd* g_pTopLevelFrame;
CGlobalUtils afxGlobalUtils;

#pragma endregion Includes

//! resource type for the XML files used in theme support
#define AFX_RT_STYLE_XML		_T("STYLE_XML")
//! main resource ID
#define AFX_STYLE_GLOBALS_ID	_T("IDX_GLOBALS")

const VisualManager::eStyle c_StyleDefault = VisualManager::STYLE_DEFAULT;
VisualManager::eStyle VisualManager::m_s_Style = c_StyleDefault;
CString VisualManager::m_s_strStylePrefix;
HINSTANCE VisualManager::m_s_hinstRes = NULL;
BOOL VisualManager::m_s_bAutoFreeRes = FALSE;

IMPLEMENT_DYNCREATE(VisualManager, CMFCVisualManagerOffice2003)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VisualManager::VisualManager() : m_bNcTextCenter(FALSE), m_bLoaded(FALSE)
{
#ifdef _DEBUG
	CleanUp();
#endif // _DEBUG

	m_szNcBtnSize[0] = CSize(0, 0);
	m_szNcBtnSize[1] = CSize(0, 0);
}

VisualManager::~VisualManager()
{
	CMenuImages::SetColor(CMenuImages::ImageBlack, (COLORREF)-1);

	CleanUp();
}

CString __stdcall VisualManager::GetStyleResourceID(eStyle Style_in, const TCHAR *pszResID_in_out)
{
	CString strResID(pszResID_in_out);
	CString strStylePrefix;

	switch(Style_in)
	{
	case STYLE_DEFAULT:
		strStylePrefix = _T("DEFAULT_");
		break;

	default:
		ASSERT(FALSE);
	}

	strResID = strStylePrefix + strResID;

	return strResID;
};

BOOL __stdcall VisualManager::SetStyle(eStyle Style_in, LPCTSTR lpszPath)
{
	if (m_s_Style == Style_in && m_s_hinstRes >(HINSTANCE) 32)
	{
		return TRUE;
	}

	UNREFERENCED_PARAMETER(lpszPath);
	CString strStyle(GetStyleResourceID(Style_in, AFX_STYLE_GLOBALS_ID));
	HINSTANCE hinstRes = AfxFindResourceHandle(strStyle, AFX_RT_STYLE_XML);

	if (::FindResource(hinstRes, strStyle, AFX_RT_STYLE_XML) == NULL)
	{
#if !defined _AFXDLL
		TRACE(_T("\r\nImportant: to enable the visual look in static link,\r\n"));
		TRACE(_T("include 'VisualManager.rc' from the RC file in your project.\r\n\r\n"));
#else
		TRACE(_T("Cannot load Style: %s\r\n"), strStyle);
#endif
		ASSERT(FALSE);

		return FALSE;
	}

	CleanStyle();
	m_s_Style = Style_in;
	SetResourceHandle(hinstRes);

	return TRUE;
}

VisualManager::eStyle __stdcall VisualManager::GetStyle()
{
	return m_s_Style;
}

void __stdcall VisualManager::SetResourceHandle(HINSTANCE hinstRes)
{
	m_s_bAutoFreeRes = FALSE;

	if (m_s_hinstRes != hinstRes)
	{
		m_s_hinstRes = hinstRes;

		if (CMFCVisualManager::GetInstance()->IsKindOf(RUNTIME_CLASS(VisualManager)))
		{
			CMFCVisualManager::GetInstance()->OnUpdateSystemColors();
		}
	}
}

void __stdcall VisualManager::CleanStyle()
{
	if (m_s_bAutoFreeRes && m_s_hinstRes >(HINSTANCE) 32)
	{
		::FreeLibrary(m_s_hinstRes);
	}

	m_s_hinstRes = NULL;
	m_s_Style = c_StyleDefault;
	m_s_strStylePrefix.Empty();
}

void VisualManager::CleanUp()
{
	m_clrEditBorder                = (COLORREF)(-1);
	m_clrEditBorderDisabled        = (COLORREF)(-1);
	m_clrEditBorderHighlighted     = (COLORREF)(-1);
	m_clrEditSelection             = (COLORREF)(-1);
	m_clrComboBorder               = (COLORREF)(-1);
	m_clrComboBorderDisabled       = (COLORREF)(-1);
	m_clrComboBorderPressed        = (COLORREF)(-1);
	m_clrComboBorderHighlighted    = (COLORREF)(-1);
	m_clrComboBtnStart             = (COLORREF)(-1);
	m_clrComboBtnFinish            = (COLORREF)(-1);
	m_clrComboBtnBorder            = (COLORREF)(-1);
	m_clrComboBtnDisabledStart     = (COLORREF)(-1);
	m_clrComboBtnDisabledFinish    = (COLORREF)(-1);
	m_clrComboBtnBorderDisabled    = (COLORREF)(-1);
	m_clrComboBtnPressedStart      = (COLORREF)(-1);
	m_clrComboBtnPressedFinish     = (COLORREF)(-1);
	m_clrComboBtnBorderPressed     = (COLORREF)(-1);
	m_clrComboBtnHighlightedStart  = (COLORREF)(-1);
	m_clrComboBtnHighlightedFinish = (COLORREF)(-1);
	m_clrComboBtnBorderHighlighted = (COLORREF)(-1);
	m_clrComboSelection            = (COLORREF)(-1);
	m_clrToolBarTitleTextColor	   = (COLORREF)(-1);
	m_clrToolBarTitleLineColor	   = (COLORREF)(-1);
	m_ctrlComboBoxBtn.CleanUp();

	m_ToolBarGripper.Clear();
	m_ToolBarTear.Clear();
	m_ctrlToolBarBorder.CleanUp();

	m_ctrlStatusBarBack.CleanUp();
	m_ctrlStatusBarBack_Ext.CleanUp();
	m_StatusBarPaneBorder.Clear();
	m_StatusBarSizeBox.Clear();

	m_SysBtnBack[0].CleanUp();
	m_SysBtnBack[1].CleanUp();
	m_SysBtnClose[0].Clear();
	m_SysBtnClose[1].Clear();
	m_SysBtnRestore[0].Clear();
	m_SysBtnRestore[1].Clear();
	m_SysBtnMaximize[0].Clear();
	m_SysBtnMaximize[1].Clear();
	m_SysBtnMinimize[0].Clear();
	m_SysBtnMinimize[1].Clear();

	m_brMainClientArea.DeleteObject();

	m_AppCaptionFont.DeleteObject();
	m_penSeparator2.DeleteObject();

	m_brGroupBackground.DeleteObject();
	m_clrGroupText = (COLORREF)-1;

	m_penSeparatorDark.DeleteObject();

	m_ctrlMainBorder.CleanUp();
	m_ctrlMDIChildBorder.CleanUp();
	m_ctrlMainBorderCaption.CleanUp();
	m_ctrlPopupBorder.CleanUp();
	m_ctrlPopupResizeBar.CleanUp();
	m_PopupResizeBar_HV.Clear();
	m_PopupResizeBar_HVT.Clear();
	m_PopupResizeBar_V.Clear();

	m_ctrlMenuBarBtn.CleanUp();

	m_ctrlMenuItemBack.CleanUp();
	m_MenuItemMarkerC.Clear();
	m_MenuItemMarkerR.Clear();
	m_ctrlMenuItemShowAll.CleanUp();
	m_ctrlMenuHighlighted[0].CleanUp();
	m_ctrlMenuHighlighted[1].CleanUp();
	m_ctrlMenuButtonBorder.CleanUp();
	m_ctrlMenuScrollBtn[0].CleanUp();
	m_ctrlMenuScrollBtn[1].CleanUp();

	m_ctrlToolBarBtn.CleanUp();

	m_ctrlTaskScrollBtn.CleanUp();

	m_ctrlTab3D[0].CleanUp();
	m_ctrlTab3D[1].CleanUp();
	m_ctrlTabFlat[0].CleanUp();
	m_ctrlTabFlat[1].CleanUp();
	m_clrTabFlatBlack = CLR_DEFAULT;
	m_clrTabTextActive = CLR_DEFAULT;
	m_clrTabTextHighlight = CLR_DEFAULT;
	m_clrTabTextInactive = CLR_DEFAULT;
	m_clrTabFlatHighlight = CLR_DEFAULT;
	m_penTabFlatInner[0].DeleteObject();
	m_penTabFlatInner[1].DeleteObject();
	m_penTabFlatOuter[0].DeleteObject();
	m_penTabFlatOuter[1].DeleteObject();

	m_clrCaptionBarText = afxGlobalData.clrWindow;

	m_bToolTipParams = FALSE;
	CMFCToolTipInfo dummy;
	m_ToolTipParams = dummy;

	m_ActivateFlag.RemoveAll();

	m_bLoaded = FALSE;
}

/*!	\brief Loads the specified resource as XML and make the version check
	\param[in] lpszResID_in String passed to CTagManager::LoadFromResource()
	\param[out] TagManager_out Tag manager with the <STYLE> and <VERSION> block parsed
	\param[out] StylePrefix_out Style prefix retrieved from the XML file
	\returns true if success (file version match too) else false
 */
bool VisualManager::LoadXML(LPCTSTR lpszResID_in, CTagManager & TagManager_out, CString & StylePrefix_out) const
{
	if (!TagManager_out.LoadFromResource(lpszResID_in, AFX_RT_STYLE_XML))
	{
#if !defined _AFXDLL
		TRACE(_T("\r\nImportant: to enable the Maquet look in static link,\r\n"));
		TRACE(_T("include 'VisualManager.rc' from the RC file in your project.\r\n\r\n"));
		ASSERT(FALSE);
#endif
		return false;
	}

	{
		CString strStyle;
		TagManager_out.ExcludeTag(_T("STYLE"), strStyle);
		TagManager_out.SetBuffer(strStyle);
	}

	// Check the format version: 1
	CString strItem;

	if (!TagManager_out.IsEmpty())
	{
		int nVersion = 0;

		if (TagManager_out.ExcludeTag(_T("VERSION"), strItem))
		{
			CTagManager tmItem(strItem);

			tmItem.ReadInt(_T("NUMBER"), nVersion);

			if (nVersion == 1)
			{
				if (tmItem.ExcludeTag(_T("ID_PREFIX"), strItem))
				{
					strItem.Trim();
					StylePrefix_out = strItem;
				}

				return true;
			}
		}
	}
	return false;
}

/*!	\brief Loads the "Globals" section of style
	\param[in] lpszResID_in String passed to CTagManager::LoadFromResource() e.g. <style prefix>IDX_GLOBALS
	\returns true if the resources were successfuly loaded; false otherwise
 */
bool VisualManager::LoadGlobalsStyleRes(LPCTSTR lpszResID_in)
{
	CTagManager TagManager;
	CString StylePrefix, StrItem; // style prefix defined in the XML

	if(LoadXML(lpszResID_in, TagManager, StylePrefix) == false)
		return false;

	if (TagManager.ExcludeTag(_T("GLOBALS"), StrItem) == false)
		return false;

	CTagManager tmItem(StrItem);

	// buttons
	tmItem.ReadColor(_T("ButtonGradientLight"), m_clrButtonGradientLight);
	tmItem.ReadColor(_T("ButtonGradientDark"), m_clrButtonGradientDark);
	tmItem.ReadColor(_T("DefaultButtonBorder"), m_clrDefaultButtonBorder);
	tmItem.ReadColor(_T("ButtonBorder"), m_clrButtonBorder);

	m_clrDisabledButton = ColorAverage(GrayAverage(m_clrButtonBorder),
									   GrayAverage(m_clrButtonGradientDark));
	m_clrPressedButton = ColorAverage(GrayAverage(m_clrButtonGradientLight),
									  GrayAverage(m_clrButtonGradientDark));

	tmItem.ReadColor(_T("PaneInactiveCaptionBorder"), m_clrPaneInactiveCaptionBorder);
	tmItem.ReadColor(_T("PaneActiveCaptionBorder"), m_clrPaneActiveCaptionBorder);

	tmItem.ReadColor(_T("BarText"), afxGlobalData.clrBarText);

	if (tmItem.ReadColor(_T("BarFace"), afxGlobalData.clrBarFace))
	{
		afxGlobalData.brBarFace.DeleteObject();
		afxGlobalData.brBarFace.CreateSolidBrush(afxGlobalData.clrBarFace);
		m_clrMenuShadowBase = afxGlobalData.clrBarFace;
	}
	if (tmItem.ReadColor(_T("ActiveCaption"), afxGlobalData.clrActiveCaption))
	{
		afxGlobalData.clrInactiveCaption     = afxGlobalData.clrActiveCaption;
		afxGlobalData.brActiveCaption.DeleteObject();
		afxGlobalData.brActiveCaption.CreateSolidBrush(afxGlobalData.clrActiveCaption);
	}
	if (tmItem.ReadColor(_T("CaptionText"), afxGlobalData.clrCaptionText))
	{
		afxGlobalData.clrInactiveCaptionText = afxGlobalData.clrCaptionText;
	}

	tmItem.ReadColor(_T("InactiveCaption"), afxGlobalData.clrInactiveCaption);
	afxGlobalData.brInactiveCaption.DeleteObject();
	afxGlobalData.brInactiveCaption.CreateSolidBrush(afxGlobalData.clrInactiveCaption);
	tmItem.ReadColor(_T("InactiveCaptionText"), afxGlobalData.clrInactiveCaptionText);

	tmItem.ReadColor(_T("BarShadow"), afxGlobalData.clrBarShadow);
	tmItem.ReadColor(_T("BarDkShadow"), afxGlobalData.clrBarDkShadow);
	tmItem.ReadColor(_T("BarLight"), afxGlobalData.clrBarLight);

	COLORREF clrFloatToolBarBorder;
	tmItem.ReadColor(_T("FloatToolBarBorder"), clrFloatToolBarBorder);
	m_brFloatToolBarBorder.DeleteObject();
	m_brFloatToolBarBorder.CreateSolidBrush(clrFloatToolBarBorder);

	tmItem.ReadColor(_T("HighlightGradientDark"), m_clrHighlightGradientDark);
	tmItem.ReadColor(_T("HighlightGradientLight"), m_clrHighlightGradientLight);

	m_clrHighlightDnGradientDark = m_clrHighlightGradientLight;
	m_clrHighlightDnGradientLight = m_clrHighlightGradientDark;
	tmItem.ReadColor(_T("HighlightDnGradientDark"), m_clrHighlightDnGradientDark);
	tmItem.ReadColor(_T("HighlightDnGradientLight"), m_clrHighlightDnGradientLight);

	m_clrHighlightCheckedGradientDark = m_clrHighlightDnGradientLight;
	m_clrHighlightCheckedGradientLight = m_clrHighlightDnGradientDark;
	tmItem.ReadColor(_T("HighlightCheckedGradientDark"), m_clrHighlightCheckedGradientDark);
	tmItem.ReadColor(_T("HighlightCheckedGradientLight"), m_clrHighlightCheckedGradientLight);

	tmItem.ReadColor(_T("PressedButtonBorder"), m_clrPressedButtonBorder);

	COLORREF clrHB = afxGlobalData.clrHilite;
	COLORREF clrHT = afxGlobalData.clrTextHilite;

	if (tmItem.ReadColor(_T("Highlight"), clrHB) && tmItem.ReadColor(_T("HighlightText"), clrHT))
	{
		afxGlobalData.clrHilite = clrHB;

		afxGlobalData.brHilite.DeleteObject();
		afxGlobalData.brHilite.CreateSolidBrush(clrHB);

		afxGlobalData.clrTextHilite = clrHT;
	}

	tmItem.ReadColor(_T("MenuShadowColor"), m_clrMenuShadowBase);

	// ToolTipParams
	m_bToolTipParams = tmItem.ReadToolTipInfo(_T("TOOLTIP"), m_ToolTipParams);

	return true;
}

/*!	\brief Loads the "MainWnd" section of style
	\param[in] lpszResID_in String passed to CTagManager::LoadFromResource() e.g. <style prefix>IDX_MAINWND
	\returns true if the resources were successfuly loaded; false otherwise
 */
bool VisualManager::LoadMainWndStyleRes(LPCTSTR lpszResID_in)
{
	CTagManager TagManager;
	CString StylePrefix, StrItem; // style prefix defined in the XML

	if(LoadXML(lpszResID_in, TagManager, StylePrefix) == false)
		return false;

	if (TagManager.ExcludeTag(_T("MAINWND"), StrItem) == false)
		return false;

	CTagManager TmItem(StrItem);
	CString StrCaption;

	// caption
	if (TmItem.ExcludeTag(_T("CAPTION"), StrCaption))
	{
		CTagManager TmCaption(StrCaption);

		NONCLIENTMETRICS ncm;
		if (afxGlobalData.GetNonClientMetrics (ncm))
		{
			TmCaption.ReadFont(_T("FONT"), ncm.lfCaptionFont);
			m_AppCaptionFont.DeleteObject();
			m_AppCaptionFont.CreateFontIndirect(&ncm.lfCaptionFont);
		}

		TmCaption.ReadColor(_T("ActiveStart"), m_clrAppCaptionActiveStart);
		TmCaption.ReadColor(_T("ActiveFinish"), m_clrAppCaptionActiveFinish);
		TmCaption.ReadColor(_T("InactiveStart"), m_clrAppCaptionInactiveStart);
		TmCaption.ReadColor(_T("InactiveFinish"), m_clrAppCaptionInactiveFinish);
		TmCaption.ReadColor(_T("ActiveText"), m_clrAppCaptionActiveText);
		TmCaption.ReadColor(_T("InactiveText"), m_clrAppCaptionInactiveText);
		TmCaption.ReadColor(_T("ActiveTitleText"), m_clrAppCaptionActiveTitleText);
		TmCaption.ReadColor(_T("InactiveTitleText"), m_clrAppCaptionInactiveTitleText);

		TmCaption.ReadBool(_T("TextCenter"), m_bNcTextCenter);

		TmCaption.ReadControlRenderer(_T("BORDER"), m_ctrlMainBorderCaption, StylePrefix + _T("IDB_MAINBORDER_CAPTION"));

		m_szNcBtnSize[0] = CSize(::GetSystemMetrics(SM_CXSIZE), ::GetSystemMetrics(SM_CYSIZE));
		m_szNcBtnSize[1] = CSize(::GetSystemMetrics(SM_CXSMSIZE), ::GetSystemMetrics(SM_CYSMSIZE));

		// buttons
		CString strButtons;

		if (TmCaption.ExcludeTag(_T("BUTTONS"), strButtons))
		{
			CTagManager TmButtons(strButtons);

			for (int i = 0; i < 2; i++)
			{
				CString str;
				CString suffix;
				if (i == 1)
				{
					suffix = _T("_S");
				}
				if (TmButtons.ExcludeTag(i == 0 ? _T("NORMAL") : _T("SMALL"), str))
				{
					CTagManager tmBtn(str);

					tmBtn.ReadSize(_T("ConstSize"), m_szNcBtnSize[i]);

					CSize SizeIcon(0, 0);
					if (tmBtn.ReadSize(_T("IconSize"), SizeIcon))
					{
						m_SysBtnClose[i].Clear();
						m_SysBtnClose[i].SetPreMultiplyAutoCheck(TRUE);
						m_SysBtnClose[i].SetImageSize(SizeIcon);
						m_SysBtnClose[i].LoadStr(StylePrefix + _T("IDB_SYS_BTN_CLOSE") + suffix);

						m_SysBtnRestore[i].Clear();
						m_SysBtnRestore[i].SetPreMultiplyAutoCheck(TRUE);
						m_SysBtnRestore[i].SetImageSize(SizeIcon);
						m_SysBtnRestore[i].LoadStr(StylePrefix + _T("IDB_SYS_BTN_RESTORE") + suffix);

						m_SysBtnMaximize[i].Clear();
						m_SysBtnMaximize[i].SetPreMultiplyAutoCheck(TRUE);
						m_SysBtnMaximize[i].SetImageSize(SizeIcon);
						m_SysBtnMaximize[i].LoadStr(StylePrefix + _T("IDB_SYS_BTN_MAXIMIZE") + suffix);

						m_SysBtnMinimize[i].Clear();
						m_SysBtnMinimize[i].SetPreMultiplyAutoCheck(TRUE);
						m_SysBtnMinimize[i].SetImageSize(SizeIcon);
						m_SysBtnMinimize[i].LoadStr(StylePrefix + _T("IDB_SYS_BTN_MINIMIZE") + suffix);
					}

					CTagManager::ParseControlRenderer(tmBtn.GetBuffer(), m_SysBtnBack[i], StylePrefix + _T("IDB_SYS_BTN_BACK"));
				}
			}
		}
	}

	// border
	TmItem.ReadControlRenderer(_T("BORDER"), m_ctrlMainBorder, StylePrefix + _T("IDB_MAINBORDER"));
	TmItem.ReadControlRenderer(_T("BORDER_MDICHILD"), m_ctrlMDIChildBorder, StylePrefix + _T("IDB_MDICHILDBORDER"));

	if (TmItem.ReadColor(_T("MainClientArea"), m_clrMainClientArea))
	{
		m_brMainClientArea.DeleteObject();
		m_brMainClientArea.CreateSolidBrush(m_clrMainClientArea);
	}

	return true;
}


/*!	\brief Loads the "Menu" section of the style
	\param[in] lpszResID_in String passed to CTagManager::LoadFromResource() e.g. <style prefix>IDX_MENU
	\returns true if the resources were successfuly loaded; false otherwise
 */
bool VisualManager::LoadMenuStyleRes(LPCTSTR lpszResID_in)
{
	CTagManager TagManager;
	CString StylePrefix, StrItem; // style prefix defined in the XML

	if(LoadXML(lpszResID_in, TagManager, StylePrefix) == false)
		return false;

	if (TagManager.ExcludeTag(_T("MENU"), StrItem) == false)
		return false;

	// menu
	CTagManager TmItem(StrItem);

	if (TmItem.ReadColor(_T("Light"), m_clrMenuLight))
	{
		m_brMenuLight.DeleteObject();
		m_brMenuLight.CreateSolidBrush(m_clrMenuLight);
	}

	m_clrMenuRarelyUsed = CLR_DEFAULT;
	TmItem.ReadColor(_T("Rarely"), m_clrMenuRarelyUsed);

	TmItem.ReadColor(_T("Border"), m_clrMenuBorder);

	if (TmItem.ReadColor(_T("Separator1"), m_clrSeparator1))
	{
		m_penSeparator.DeleteObject();
		m_penSeparator.CreatePen(PS_SOLID, 1, m_clrSeparator1);
	}

	if (TmItem.ReadColor(_T("Separator2"), m_clrSeparator2))
	{
		m_penSeparator2.DeleteObject();
		m_penSeparator2.CreatePen(PS_SOLID, 1, m_clrSeparator2);
	}

	COLORREF clrGroupBack = (COLORREF)-1;
	if (TmItem.ReadColor(_T("GroupBackground"), clrGroupBack))
	{
		m_brGroupBackground.DeleteObject();
		m_brGroupBackground.CreateSolidBrush(clrGroupBack);
	}

	TmItem.ReadColor(_T("GroupText"), m_clrGroupText);

	if (TmItem.ReadColor(_T("ItemBorder"), m_clrMenuItemBorder))
	{
		m_penMenuItemBorder.DeleteObject();
		m_penMenuItemBorder.CreatePen(PS_SOLID, 1, m_clrMenuItemBorder);
	}

	TmItem.ReadInt(_T("BorderSize"), m_nMenuBorderSize);

	TmItem.ReadControlRenderer(_T("ItemBack"), m_ctrlMenuItemBack, StylePrefix + _T("IDB_MENU_ITEM_BACK"));
	TmItem.ReadToolBarImages(_T("ItemCheck"), m_MenuItemMarkerC, StylePrefix + _T("IDB_MENU_ITEM_MARKER_C"));
	TmItem.ReadToolBarImages(_T("ItemRadio"), m_MenuItemMarkerR, StylePrefix + _T("IDB_MENU_ITEM_MARKER_R"));
	TmItem.ReadControlRenderer(_T("ItemShowAll"), m_ctrlMenuItemShowAll, StylePrefix + _T("IDB_MENU_ITEM_SHOWALL"));
	TmItem.ReadControlRenderer(_T("Highlighted"), m_ctrlMenuHighlighted[0], StylePrefix + _T("IDB_MENU_BTN"));
	TmItem.ReadControlRenderer(_T("HighlightedDisabled"), m_ctrlMenuHighlighted[1], StylePrefix + _T("IDB_MENU_BTN_DISABLED"));
	TmItem.ReadControlRenderer(_T("ButtonBorder"), m_ctrlMenuButtonBorder, StylePrefix + _T("IDB_MENU_BTN_VERT_SEPARATOR"));
	TmItem.ReadControlRenderer(_T("ScrollBtn_T"), m_ctrlMenuScrollBtn[0], StylePrefix + _T("IDB_MENU_BTN_SCROLL_T"));
	TmItem.ReadControlRenderer(_T("ScrollBtn_B"), m_ctrlMenuScrollBtn[1], StylePrefix + _T("IDB_MENU_BTN_SCROLL_B"));

	TmItem.ReadColor(_T("TextNormal"), m_clrMenuText);
	TmItem.ReadColor(_T("TextHighlighted"), m_clrMenuTextHighlighted);
	TmItem.ReadColor(_T("TextDisabled"), m_clrMenuTextDisabled);

	COLORREF clrImages = m_clrMenuText;
	CString strColors;

	if (TmItem.ExcludeTag(_T("COLORS"), strColors))
	{
		CTagManager TmColors(strColors);

		TmColors.ReadColor(_T("Black"), clrImages);
		CMenuImages::SetColor(CMenuImages::ImageBlack, clrImages);

		TmColors.ReadColor(_T("Black2"), clrImages);
		CMenuImages::SetColor(CMenuImages::ImageBlack2, clrImages);

		struct XColors
		{
			CMenuImages::IMAGE_STATE state;
			LPCTSTR name;
		};

		XColors colors[4] =
		{
			{CMenuImages::ImageGray, _T("Gray")},
			{CMenuImages::ImageLtGray, _T("LtGray")},
			{CMenuImages::ImageWhite, _T("White")},
			{CMenuImages::ImageDkGray, _T("DkGray")}
		};

		for (int ic = 0; ic < 4; ic++)
		{
			if (TmColors.ReadColor(colors[ic].name, clrImages))
			{
				CMenuImages::SetColor(colors[ic].state, clrImages);
			}
		}
	}
	else
	{
		TmItem.ReadColor(_T("ImagesColor"), clrImages);
		CMenuImages::SetColor(CMenuImages::ImageBlack, clrImages);
		CMenuImages::SetColor(CMenuImages::ImageBlack2, clrImages);
	}

	return true;
}

/*!	\brief Loads the "Tabs" section of the style
	\param[in] lpszResID_in The resource string ID passed to CTagManager::LoadFromResource() e.g. <style prefix>IDX_TABS
	\returns true if the resources were successfuly loaded; false otherwise
 */
bool VisualManager::LoadTabsStyleRes(LPCTSTR lpszResID_in)
{
	CTagManager TagManager;
	CString StylePrefix, StrItem; // style prefix defined in the XML

	if(LoadXML(lpszResID_in, TagManager, StylePrefix) == false)
		return false;

	if (TagManager.ExcludeTag(_T("TABS"), StrItem) == false)
		return false;

	CTagManager TmItem(StrItem);
	CString StrTab;

	TmItem.ReadColor(_T("TextColorActive"), m_clrTabTextActive);
	TmItem.ReadColor(_T("TextColorInactive"), m_clrTabTextInactive);
	TmItem.ReadColor(_T("TextColorHighlight"), m_clrTabTextHighlight);

	if (TmItem.ExcludeTag(_T("3D"), StrTab))
	{
		CTagManager TmTab(StrTab);
		CString StrBtn;

		if (TmTab.ExcludeTag(_T("BUTTON"), StrBtn))
		{
			CMFCControlRendererInfo params(StylePrefix + _T("IDB_TAB_3D"), CRect(0, 0, 0, 0), CRect(0, 0, 0, 0));
			if (CTagManager::ParseControlRendererInfo(StrBtn, params))
			{
				m_ctrlTab3D[0].Create(params);
				m_ctrlTab3D[1].Create(params, TRUE);
			}
		}
	}

	if (TmItem.ExcludeTag(_T("FLAT"), StrTab))
	{
		CTagManager TmTab(StrTab);
		CString StrBtn;

		if (TmTab.ExcludeTag(_T("BUTTON"), StrBtn))
		{
			CMFCControlRendererInfo params(StylePrefix + _T("IDB_TAB_FLAT"), CRect(0, 0, 0, 0), CRect(0, 0, 0, 0));
			if (CTagManager::ParseControlRendererInfo(StrBtn, params))
			{
				m_ctrlTabFlat[0].Create(params);
				m_ctrlTabFlat[1].Create(params, TRUE);
			}
		}

		TmTab.ReadColor(_T("Black"), m_clrTabFlatBlack);
		TmTab.ReadColor(_T("Highlight"), m_clrTabFlatHighlight);

		COLORREF clr;

		if (TmTab.ReadColor(_T("BorderInnerNormal"), clr))
		{
			m_penTabFlatInner[0].DeleteObject();
			m_penTabFlatInner[0].CreatePen(PS_SOLID, 1, clr);
		}
		if (TmTab.ReadColor(_T("BorderInnerActive"), clr))
		{
			m_penTabFlatInner[1].DeleteObject();
			m_penTabFlatInner[1].CreatePen(PS_SOLID, 1, clr);
		}
		if (TmTab.ReadColor(_T("BorderOuterNormal"), clr))
		{
			m_penTabFlatOuter[0].DeleteObject();
			m_penTabFlatOuter[0].CreatePen(PS_SOLID, 1, clr);
		}
		if (TmTab.ReadColor(_T("BorderOuterActive"), clr))
		{
			m_penTabFlatOuter[1].DeleteObject();
			m_penTabFlatOuter[1].CreatePen(PS_SOLID, 1, clr);
		}
	}

	return true;
}

/*!	\brief Loads the "Edit" section of the style
	\param[in] lpszResID_in The resource string ID passed to CTagManager::LoadFromResource() e.g. <style prefix>IDX_EDIT
	\returns true if the resources were successfuly loaded; false otherwise
 */
bool VisualManager::LoadEditStyleRes(LPCTSTR lpszResID_in)
{
	CTagManager TagManager;
	CString StylePrefix, StrItem; // style prefix defined in the XML

	if(LoadXML(lpszResID_in, TagManager, StylePrefix) == false)
		return false;

	if (TagManager.ExcludeTag(_T("EDIT"), StrItem) == false)
		return false;

	CTagManager TmItem(StrItem);

	TmItem.ReadColor(_T("BorderNormal"), m_clrEditBorder);
	TmItem.ReadColor(_T("BorderHighlighted"), m_clrEditBorderHighlighted);
	TmItem.ReadColor(_T("BorderDisabled"), m_clrEditBorderDisabled);
	TmItem.ReadColor(_T("Selection"), m_clrEditSelection);

	return true;
}

/*!	\brief Loads the "Combo" section of the style
	\param[in] lpszResID_in The resource string ID passed to CTagManager::LoadFromResource() e.g. <style prefix>IDX_COMBO
	\returns true if the resources were successfuly loaded; false otherwise
 */
bool VisualManager::LoadComboStyleRes(LPCTSTR lpszResID_in)
{
	CTagManager TagManager;
	CString StylePrefix, StrItem; // style prefix defined in the XML

	if(LoadXML(lpszResID_in, TagManager, StylePrefix) == false)
		return false;

	m_clrComboBorder               = afxGlobalData.clrWindow;
	m_clrComboBorderDisabled       = afxGlobalData.clrBtnShadow;
	m_clrComboBorderHighlighted    = m_clrMenuItemBorder;
	m_clrComboBorderPressed        = m_clrComboBorderHighlighted;
	m_clrComboBtnBorder            = m_clrComboBorder;
	m_clrComboBtnBorderHighlighted = m_clrComboBorderHighlighted;
	m_clrComboBtnBorderPressed     = m_clrComboBorderHighlighted;
	m_clrComboSelection            = afxGlobalData.clrHilite;
	m_clrComboBtnStart             = m_clrToolBarGradientDark;
	m_clrComboBtnFinish            = m_clrToolBarGradientLight;
	m_clrComboBtnDisabledStart     = afxGlobalData.clrBtnFace;
	m_clrComboBtnDisabledFinish    = m_clrComboBtnDisabledStart;
	m_clrComboBtnHighlightedStart  = m_clrHighlightGradientDark;
	m_clrComboBtnHighlightedFinish = m_clrHighlightGradientLight;
	m_clrComboBtnPressedStart      = m_clrHighlightDnGradientDark;
	m_clrComboBtnPressedFinish     = m_clrHighlightDnGradientLight;

	if (TagManager.ExcludeTag(_T("COMBO"), StrItem) == false)
		return false;


	// combobox
	CTagManager TmItem(StrItem);
	CString StrButton;

	TmItem.ReadColor(_T("BorderNormal"), m_clrComboBorder);
	TmItem.ReadColor(_T("BorderHighlighted"), m_clrComboBorderHighlighted);
	TmItem.ReadColor(_T("BorderDisabled"), m_clrComboBorderDisabled);

	m_clrComboBorderPressed = m_clrComboBorderHighlighted;
	TmItem.ReadColor(_T("BorderPressed"), m_clrComboBorderPressed);

	TmItem.ReadColor(_T("Selection"), m_clrComboSelection);

	if (TmItem.ExcludeTag(_T("BUTTON"), StrButton))
	{
		CTagManager TmButton(StrButton);

		TmButton.ReadColor(_T("GradientStartNormal"), m_clrComboBtnStart);
		TmButton.ReadColor(_T("GradientFinishNormal"), m_clrComboBtnFinish);
		TmButton.ReadColor(_T("BtnBorderNormal"), m_clrComboBtnBorder);

		if (!TmButton.ReadControlRenderer(_T("IMAGE"), m_ctrlComboBoxBtn, StylePrefix + _T("IDB_COMBOBOX_BTN")))
		{
			TmButton.ReadColor(_T("GradientStartHighlighted"), m_clrComboBtnHighlightedStart);
			TmButton.ReadColor(_T("GradientFinishHighlighted"), m_clrComboBtnHighlightedFinish);
			TmButton.ReadColor(_T("GradientStartDisabled"), m_clrComboBtnDisabledStart);
			TmButton.ReadColor(_T("GradientFinishDisabled"), m_clrComboBtnDisabledFinish);
			TmButton.ReadColor(_T("GradientStartPressed"), m_clrComboBtnPressedStart);
			TmButton.ReadColor(_T("GradientFinishPressed"), m_clrComboBtnPressedFinish);

			TmButton.ReadColor(_T("BtnBorderHighlighted"), m_clrComboBtnBorderHighlighted);
			TmButton.ReadColor(_T("BtnBorderDisabled"), m_clrComboBtnBorderDisabled);

			m_clrComboBtnBorderPressed = m_clrComboBtnBorderHighlighted;
			TmButton.ReadColor(_T("BtnBorderPressed"), m_clrComboBtnBorderPressed);
		}
	}

	return true;
}

/*!	\brief Loads the "Header" section of the style
	\param[in] lpszResID_in The resource string ID passed to CTagManager::LoadFromResource() e.g. <style prefix>IDX_HEADER
	\returns true if the resources were successfuly loaded; false otherwise
 */
bool VisualManager::LoadHeaderStyleRes(LPCTSTR lpszResID_in)
{
	CTagManager TagManager;
	CString StylePrefix, StrItem; // style prefix defined in the XML

	if(LoadXML(lpszResID_in, TagManager, StylePrefix) == false)
		return false;

	if (TagManager.ExcludeTag(_T("HEADER"), StrItem) == false)
		return false;

	CTagManager TmItem(StrItem);

	TmItem.ReadColor(_T("NormalStart"), m_clrHeaderNormalStart);
	TmItem.ReadColor(_T("NormalFinish"), m_clrHeaderNormalFinish);
	TmItem.ReadColor(_T("NormalBorder"), m_clrHeaderNormalBorder);
	TmItem.ReadColor(_T("HighlightedStart"), m_clrHeaderHighlightedStart);
	TmItem.ReadColor(_T("HighlightedFinish"), m_clrHeaderHighlightedFinish);
	TmItem.ReadColor(_T("HighlightedBorder"), m_clrHeaderHighlightedBorder);
	TmItem.ReadColor(_T("PressedStart"), m_clrHeaderPressedStart);
	TmItem.ReadColor(_T("PressedFinish"), m_clrHeaderPressedFinish);
	TmItem.ReadColor(_T("PressedBorder"), m_clrHeaderPressedBorder);

	return true;
}

/*!	\brief Loads the "TabDialog" section of the style
	\param[in] lpszResID_in The resource string ID passed to CTagManager::LoadFromResource() e.g. <style prefix>IDX_TABDIALOG
	\returns true if the resources were successfuly loaded; false otherwise
 */
bool VisualManager::LoadTabDialogStyleRes(LPCTSTR lpszResID_in)
{
	CTagManager TagManager;
	CString StylePrefix, StrItem; // style prefix defined in the XML

	if(LoadXML(lpszResID_in, TagManager, StylePrefix) == false)
		return false;

	if (TagManager.ExcludeTag(_T("TAB_DIALOG"), StrItem) == false)
		return false;

	// header
	CTagManager TmItem(StrItem);

	if (TmItem.ExcludeTag(_T("HEADER"), StrItem))
	{
		CTagManager TmHeader(StrItem);

		TmHeader.ReadColor(_T("InactiveBackground"), m_clrTabDialogHeaderInactiveBackground);
		TmHeader.ReadColor(_T("ActiveBackground"), m_clrTabDialogHeaderActiveBackground);
		TmHeader.ReadColor(_T("PushedBackground"), m_clrTabDialogHeaderPushedBackground);
		TmHeader.ReadColor(_T("InactiveBorder"), m_clrTabDialogHeaderInactiveBorder);
		TmHeader.ReadColor(_T("ActiveBorder"), m_clrTabDialogHeaderActiveBorder);
		TmHeader.ReadColor(_T("PushedBorder"), m_clrTabDialogHeaderPushedBorder);
		TmHeader.ReadColor(_T("Arrow"), m_clrTabDialogHeaderArrow);
	}

	if (TmItem.ExcludeTag(_T("PAGE"), StrItem))
	{
		CTagManager TmPage(StrItem);

		TmPage.ReadColor(_T("Background"), m_clrTabDialogPageBackground);
		TmPage.ReadColor(_T("Border"), m_clrTabDialogPageBorder);
	}

	return true;
}

/*!	\brief Loads the "Bars" section of style
	\param[in] lpszResID_in String passed to CTagManager::LoadFromResource() e.g. <style prefix>IDX_BARS
	\returns true if the resources were successfuly loaded; false otherwise
 */
bool VisualManager::LoadBarsStyleRes(LPCTSTR lpszResID_in)
{
	CTagManager TagManager;
	CString StylePrefix, StrItem; // style prefix defined in the XML

	if(LoadXML(lpszResID_in, TagManager, StylePrefix) == false)
		return false;

	if (TagManager.ExcludeTag(_T("BARS"), StrItem) == false)
		return false;

	CTagManager TmItem(StrItem);
	CString StrBar;

	if (TmItem.ExcludeTag(_T("DEFAULT"), StrBar))
	{
		CTagManager TmBar(StrBar);

		if (TmBar.ReadColor(_T("Bkgnd"), m_clrBarBkgnd))
		{
			m_brBarBkgnd.DeleteObject();
			m_brBarBkgnd.CreateSolidBrush(m_clrBarBkgnd);
		}

		TmBar.ReadColor(_T("GradientLight"), m_clrBarGradientLight);
		m_clrBarGradientDark = m_clrBarGradientLight;
		TmBar.ReadColor(_T("GradientDark"), m_clrBarGradientDark);
	}

	if (TmItem.ExcludeTag(_T("TOOLBAR"), StrBar))
	{
		CTagManager TmBar(StrBar);

		m_clrToolBarGradientLight = m_clrBarGradientLight;
		m_clrToolBarGradientDark  = m_clrBarGradientDark;

		m_clrToolbarDisabled = CDrawingManager::SmartMixColors(m_clrToolBarGradientDark, m_clrToolBarGradientLight);

		TmBar.ReadColor(_T("GradientLight"), m_clrToolBarGradientLight);
		TmBar.ReadColor(_T("GradientDark"), m_clrToolBarGradientDark);

		m_clrToolBarGradientVertLight = m_clrToolBarGradientLight;
		m_clrToolBarGradientVertDark  = m_clrToolBarGradientDark;

		TmBar.ReadColor(_T("TitleTextColor"), m_clrToolBarTitleTextColor);
		TmBar.ReadColor(_T("TitleLineColor"), m_clrToolBarTitleLineColor);

		TmBar.ReadColor(_T("GradientVertLight"), m_clrToolBarGradientVertLight);
		TmBar.ReadColor(_T("GradientVertDark"), m_clrToolBarGradientVertDark);

		TmBar.ReadColor(_T("CustomizeButtonGradientLight"), m_clrCustomizeButtonGradientLight);
		TmBar.ReadColor(_T("CustomizeButtonGradientDark"), m_clrCustomizeButtonGradientDark);

		TmBar.ReadToolBarImages(_T("GRIPPER"), m_ToolBarGripper, StylePrefix + _T("IDB_GRIPPER"));
		TmBar.ReadToolBarImages(_T("TEAR"), m_ToolBarTear, StylePrefix + _T("IDB_TEAR"));

		TmBar.ReadControlRenderer(_T("BUTTON"), m_ctrlToolBarBtn, StylePrefix + _T("IDB_TOOLBAR_BTN"));
		TmBar.ReadControlRenderer(_T("BORDER"), m_ctrlToolBarBorder, StylePrefix + _T("IDB_TOOLBAR_BORDER"));

		m_clrToolBarBtnText = afxGlobalData.clrBarText;
		m_clrToolBarBtnTextHighlighted = m_clrToolBarBtnText;
		TmBar.ReadColor(_T("TextNormal"), m_clrToolBarBtnText);
		TmBar.ReadColor(_T("TextHighlighted"), m_clrToolBarBtnTextHighlighted);
		TmBar.ReadColor(_T("TextDisabled"), m_clrToolBarBtnTextDisabled);

		if (TmBar.ReadColor(_T("BottomLineColor"), m_clrToolBarBottomLine))
		{
			m_penBottomLine.DeleteObject();
			m_penBottomLine.CreatePen(PS_SOLID, 1, m_clrToolBarBottomLine);
		}

		m_penSeparatorDark.DeleteObject();
		m_penSeparatorDark.CreatePen(PS_SOLID, 1, CDrawingManager::PixelAlpha(m_clrToolBarBottomLine, RGB(255, 255, 255), 95));

		m_penSeparatorLight.DeleteObject();
		m_penSeparatorLight.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	}

	if (TmItem.ExcludeTag(_T("MENUBAR"), StrBar))
	{
		CTagManager TmBar(StrBar);

		m_clrMenuBarGradientLight = m_clrToolBarGradientLight;
		m_clrMenuBarGradientDark  = m_clrToolBarGradientDark;

		// 2-way gradient
		TmBar.ReadColor(_T("GradientLight"), m_clrMenuBarGradientLight);
		TmBar.ReadColor(_T("GradientLight2"), m_clrMenuBarGradientLight2);
		TmBar.ReadColor(_T("GradientDark"), m_clrMenuBarGradientDark);
		TmBar.ReadColor(_T("GradientDark2"), m_clrMenuBarGradientDark2);			

		m_clrMenuBarGradientVertLight = m_clrMenuBarGradientLight;
		m_clrMenuBarGradientVertDark  = m_clrMenuBarGradientDark;

		TmBar.ReadColor(_T("GradientVertLight"), m_clrMenuBarGradientVertLight);
		TmBar.ReadColor(_T("GradientVertDark"), m_clrMenuBarGradientVertDark);

		m_clrMenuBarBtnText            = m_clrToolBarBtnText;
		m_clrMenuBarBtnTextHighlighted = m_clrToolBarBtnTextHighlighted;
		m_clrMenuBarBtnTextDisabled    = m_clrToolBarBtnTextDisabled;
		TmBar.ReadColor(_T("TextNormal"), m_clrMenuBarBtnText);
		TmBar.ReadColor(_T("TextHighlighted"), m_clrMenuBarBtnTextHighlighted);
		TmBar.ReadColor(_T("TextDisabled"), m_clrMenuBarBtnTextDisabled);

		TmBar.ReadControlRenderer(_T("BUTTON"), m_ctrlMenuBarBtn, StylePrefix + _T("IDB_MENUBAR_BTN"));
	}

	if (TmItem.ExcludeTag(_T("POPUPBAR"), StrBar))
	{
		CTagManager TmBar(StrBar);
		TmBar.ReadControlRenderer(_T("BORDER"), m_ctrlPopupBorder, StylePrefix + _T("IDB_POPUPMENU_BORDER"));

		CString strResize;
		if (TmBar.ExcludeTag(_T("RESIZEBAR"), strResize))
		{
			CTagManager tmResize(strResize);
			tmResize.ReadControlRenderer(_T("BACK"), m_ctrlPopupResizeBar, StylePrefix + _T("IDB_POPUPMENU_RESIZEBAR"));
			tmResize.ReadToolBarImages(_T("ICON_HV"), m_PopupResizeBar_HV, StylePrefix + _T("IDB_POPUPMENU_RESIZEBAR_ICON_HV"));
			tmResize.ReadToolBarImages(_T("ICON_HVT"), m_PopupResizeBar_HVT, StylePrefix + _T("IDB_POPUPMENU_RESIZEBAR_ICON_HVT"));
			tmResize.ReadToolBarImages(_T("ICON_V"), m_PopupResizeBar_V, StylePrefix + _T("IDB_POPUPMENU_RESIZEBAR_ICON_V"));
		}
	}

	if (TmItem.ExcludeTag(_T("STATUSBAR"), StrBar))
	{
		CTagManager TmBar(StrBar);

		TmBar.ReadControlRenderer(_T("BACK"), m_ctrlStatusBarBack, StylePrefix + _T("IDB_STATUSBAR_BACK"));
		TmBar.ReadControlRenderer(_T("BACK_EXT"), m_ctrlStatusBarBack_Ext, StylePrefix + _T("IDB_STATUSBAR_BACK_EXT"));

		TmBar.ReadToolBarImages(_T("PANEBORDER"), m_StatusBarPaneBorder, StylePrefix + _T("IDB_STATUSBAR_PANEBORDER"));
		TmBar.ReadToolBarImages(_T("SIZEBOX"), m_StatusBarSizeBox, StylePrefix + _T("IDB_STATUSBAR_SIZEBOX"));

		m_clrStatusBarText = m_clrMenuBarBtnText;
		m_clrStatusBarTextDisabled = m_clrMenuBarBtnTextDisabled;
		m_clrExtenedStatusBarTextDisabled = m_clrMenuBarBtnTextDisabled;

		TmBar.ReadColor(_T("TextNormal"), m_clrStatusBarText);
		TmBar.ReadColor(_T("TextDisabled"), m_clrStatusBarTextDisabled);
		TmBar.ReadColor(_T("TextExtendedDisabled"), m_clrExtenedStatusBarTextDisabled);
	}

	if (TmItem.ExcludeTag(_T("CAPTIONBAR"), StrBar))
	{
		CTagManager TmBar(StrBar);

		TmBar.ReadColor(_T("GradientLight"), m_clrCaptionBarGradientLight);
		TmBar.ReadColor(_T("GradientDark"), m_clrCaptionBarGradientDark);
		TmBar.ReadColor(_T("TextNormal"), m_clrCaptionBarText);
	}

	return true;
}

/*!	\brief Loads the "Popup" section of style
	\param[in] lpszResID_in String passed to CTagManager::LoadFromResource() e.g. <style prefix>IDX_POPUP
	\returns true if the resources were successfuly loaded; false otherwise
 */
bool VisualManager::LoadPopupStyleRes(LPCTSTR lpszResID_in)
{
	CTagManager TagManager;
	CString StylePrefix, StrItem; // style prefix defined in the XML

	if(LoadXML(lpszResID_in, TagManager, StylePrefix) == false)
		return false;

	m_clrPopupGradientLight = m_clrBarGradientLight;
	m_clrPopupGradientDark = m_clrBarGradientDark;

	if (TagManager.ExcludeTag(_T("POPUP"), StrItem) == false)
		return false;

	CTagManager TmItem(StrItem);

	TmItem.ReadColor(_T("GradientFillLight"), m_clrPopupGradientLight);
	TmItem.ReadColor(_T("GradientFillDark"), m_clrPopupGradientDark);

	return true;
}

/*!	\brief Loads the "Dialog" section of style
	\param[in] lpszResID_in String passed to CTagManager::LoadFromResource() e.g. <style prefix>IDX_DIALOG
	\returns true if the resources were successfuly loaded; false otherwise
 */
bool VisualManager::LoadDialogStyleRes(LPCTSTR lpszResID_in)
{
	CTagManager TagManager;
	CString StylePrefix, StrItem; // style prefix defined in the XML

	if(LoadXML(lpszResID_in, TagManager, StylePrefix) == false)
		return false;

	if (TagManager.ExcludeTag(_T("DIALOG"), StrItem) == false)
		return false;

	CTagManager TmItem(StrItem);

	TmItem.ReadColor(_T("InactiveCaptionLight"), m_clrDialogInactiveCaptionLight);
	TmItem.ReadColor(_T("InactiveCaptionDark"), m_clrDialogInactiveCaptionDark);
	TmItem.ReadColor(_T("ActiveCaptionLight"), m_clrDialogActiveCaptionLight);
	TmItem.ReadColor(_T("ActiveCaptionDark"), m_clrDialogActiveCaptionDark);
	TmItem.ReadColor(_T("InactiveCaptionText"), m_clrDialogInactiveCaptionText);
	TmItem.ReadColor(_T("ActiveCaptionText"), m_clrDialogActiveCaptionText);
	TmItem.ReadColor(_T("CaptionBorder"), m_clrDialogCaptionBorder);
	TmItem.ReadColor(_T("Background"), m_clrDialogBackground);
	TmItem.ReadColor(_T("Frame"), m_clrDialogFrame);

	return true;
}

/*!	\brief Virtual function that should load every resources
	\sa LoadGlobalsStyleRes(), LoadMainWndStyleRes(), LoadBarsStyleRes(), LoadDialogStyleRest()

	In the VisualManager the function calls:
	LoadGlobalsStyleRes(), LoadMainWndStyleRes(), LoadBarsStyleRes(), LoadPopupStyleRes(), LoadDialogStyleRest()
 */
bool VisualManager::LoadResources(void)
{
	eStyle CurrentStyle = GetStyle();

	return (LoadGlobalsStyleRes  (GetStyleResourceID(CurrentStyle, _T("IDX_GLOBALS")))
		 && LoadMainWndStyleRes  (GetStyleResourceID(CurrentStyle, _T("IDX_MAINWND")))
		 && LoadPopupStyleRes    (GetStyleResourceID(CurrentStyle, _T("IDX_POPUP")))
		 && LoadBarsStyleRes     (GetStyleResourceID(CurrentStyle, _T("IDX_BARS")))
		 && LoadDialogStyleRes   (GetStyleResourceID(CurrentStyle, _T("IDX_DIALOG")))
		 && LoadTabDialogStyleRes(GetStyleResourceID(CurrentStyle, _T("IDX_TABDIALOG")))
		 && LoadHeaderStyleRes	 (GetStyleResourceID(CurrentStyle, _T("IDX_HEADER")))
		 && LoadComboStyleRes	 (GetStyleResourceID(CurrentStyle, _T("IDX_COMBO")))
		 && LoadEditStyleRes	 (GetStyleResourceID(CurrentStyle, _T("IDX_EDIT")))
		 && LoadMenuStyleRes	 (GetStyleResourceID(CurrentStyle, _T("IDX_MENU")))
		 && LoadTabsStyleRes	 (GetStyleResourceID(CurrentStyle, _T("IDX_TABS"))));
}

void VisualManager::OnUpdateSystemColors()
{
	CleanUp();

	CMFCVisualManagerOffice2003::OnUpdateSystemColors();

	if (afxGlobalData.IsHighContrastMode() || afxGlobalData.m_nBitsPerPixel <= 8)
	{
		return;
	}

	m_nMenuBorderSize = 1;

	HINSTANCE hinstResOld = NULL;

	if (m_s_hinstRes == NULL)
	{
		SetStyle(c_StyleDefault);
	}

	// Save old style. In our case there is none (there is only one style)
	if (m_s_hinstRes != NULL)
	{
		hinstResOld = AfxGetResourceHandle();
		AfxSetResourceHandle(m_s_hinstRes);
	}

	if((m_bLoaded = LoadResources()) == false)
	{
		// Load failed => try to restore the old style
		if (hinstResOld != NULL)
		{
			AfxSetResourceHandle(hinstResOld);
		}
	}

	if (hinstResOld != NULL)
	{
		AfxSetResourceHandle(hinstResOld);
	}
}

BOOL VisualManager::IsWindowActive(CWnd* pWnd) const
{
	BOOL bActive = FALSE;

	HWND hWnd = pWnd->GetSafeHwnd();

	if (hWnd != NULL)
	{
		if (!m_ActivateFlag.Lookup(pWnd->GetSafeHwnd(), bActive))
		{
			//ASSERT(FALSE);
			bActive = TRUE;
		}
	}

	return bActive;
}

BOOL VisualManager::OnNcActivate(CWnd* pWnd, BOOL bActive)
{
	ASSERT_VALID(pWnd);

	if (pWnd->GetSafeHwnd() == NULL)
	{
		return FALSE;
	}

	if (afxGlobalData.DwmIsCompositionEnabled())
	{
		return FALSE;
	}

	// stay active if WF_STAYACTIVE bit is on
	if (pWnd->m_nFlags & WF_STAYACTIVE)
	{
		bActive = TRUE;
	}

	// but do not stay active if the window is disabled
	if (!pWnd->IsWindowEnabled())
	{
		bActive = FALSE;
	}

	BOOL bIsMDIFrame = FALSE;
	BOOL bWasActive = FALSE;

	// If the active state of an owner-draw MDI frame window changes, we need to
	// invalidate the MDI client area so the MDI child window captions are redrawn.
	if (IsOwnerDrawCaption())
	{
		bIsMDIFrame = pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd));
		bWasActive = IsWindowActive(pWnd);
	}

	m_ActivateFlag[pWnd->GetSafeHwnd()] = bActive;
	pWnd->SendMessage(WM_NCPAINT, 0, 0);

	if (IsOwnerDrawCaption())
	{
		if (bIsMDIFrame && (bWasActive != bActive))
		{
			::RedrawWindow(((CMDIFrameWnd *)pWnd)->m_hWndMDIClient, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
		}
	}

	return TRUE;
}

void VisualManager::DrawNcBtn(CDC* pDC, const CRect& rect, UINT nButton, AFX_BUTTON_STATE state, BOOL bSmall, BOOL bActive, BOOL bMDI/* = FALSE*/)
{
	ASSERT_VALID(pDC);

	CMFCToolBarImages* pImage = NULL;

	int nIndex = bSmall ? 1 : 0;

	if (nButton == SC_CLOSE)
	{
		pImage = &m_SysBtnClose[nIndex];
	}
	else if (nButton == SC_MINIMIZE)
	{
		pImage = &m_SysBtnMinimize[nIndex];
	}
	else if (nButton == SC_MAXIMIZE)
	{
		pImage = &m_SysBtnMaximize[nIndex];
	}
	else if (nButton == SC_RESTORE)
	{
		pImage = &m_SysBtnRestore[nIndex];
	}
	else
	{
		return;
	}

	CMFCToolBarImages::ImageAlignHorz horz = afxGlobalData.GetRibbonImageScale() != 1. ? CMFCToolBarImages::ImageAlignHorzStretch : CMFCToolBarImages::ImageAlignHorzCenter;
	CMFCToolBarImages::ImageAlignVert vert = afxGlobalData.GetRibbonImageScale() != 1. ? CMFCToolBarImages::ImageAlignVertStretch : CMFCToolBarImages::ImageAlignVertCenter;

	CRect rtBtnImage(CPoint(0, 0), pImage->GetImageSize());

	if (!bActive)
	{
		rtBtnImage.OffsetRect(0, pImage->GetImageSize().cy * 3);
	}
	else
	{
		if (state != ButtonsIsRegular)
		{
			m_SysBtnBack[nIndex].Draw(pDC, rect, state == ButtonsIsHighlighted ? 0 : 1);
			rtBtnImage.OffsetRect(0, pImage->GetImageSize().cy * (state == ButtonsIsHighlighted ? 1 : 2));
		}
	}

	pImage->DrawEx(pDC, rect, 0, horz, vert, rtBtnImage);
}

void VisualManager::DrawNcText(CDC* pDC, CRect& rect, const CString& strTitle, const CString& strDocument, BOOL bPrefix, BOOL bActive,
	BOOL bIsRTL, BOOL bTextCenter, BOOL bGlass/* = FALSE*/, int nGlassGlowSize/* = 0*/, COLORREF clrGlassText/* = (COLORREF)-1*/)
{
	if ((strTitle.IsEmpty() && strDocument.IsEmpty()) || rect.right <= rect.left)
	{
		return;
	}

	ASSERT_VALID(pDC);

	int nOldMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF clrOldText = pDC->GetTextColor();

	DWORD dwTextStyle = DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER | (bIsRTL ? DT_RTLREADING : 0);

	if (strDocument.IsEmpty())
	{
		COLORREF clrText = bActive ? m_clrAppCaptionActiveTitleText : m_clrAppCaptionInactiveTitleText;

		int widthFull = rect.Width();
		int width = pDC->GetTextExtent(strTitle).cx;

		if (bTextCenter && width < widthFull)
		{
			rect.left += (widthFull - width) / 2;
		}

		rect.right = min(rect.left + width, rect.right);

		if (rect.right > rect.left)
		{
			if (bGlass)
			{
				DrawTextOnGlass(pDC, strTitle, rect, dwTextStyle, nGlassGlowSize, clrGlassText);
			}
			else
			{
				pDC->SetTextColor(clrText);
				pDC->DrawText(strTitle, rect, dwTextStyle);
			}
		}
	}
	else
	{
		const CString& str1 = bPrefix ? strDocument : strTitle;
		const CString& str2 = bPrefix ? strTitle : strDocument;

		COLORREF clrText1 = bActive ? m_clrAppCaptionActiveText : m_clrAppCaptionInactiveText;
		COLORREF clrText2 = bActive ? m_clrAppCaptionActiveTitleText : m_clrAppCaptionInactiveTitleText;

		if (!bPrefix)
		{
			COLORREF clr = clrText1;
			clrText1 = clrText2;
			clrText2 = clr;
		}

		int widthFull = rect.Width();
		CSize sz1 = pDC->GetTextExtent(str1);
		CSize sz2 = pDC->GetTextExtent(str2);
		int width = sz1.cx + sz2.cx;
		int left = rect.left;

		if (bTextCenter && width < widthFull)
		{
			rect.left += (widthFull - width) / 2;
		}

		rect.right = min(rect.left + width, rect.right);

		if (bIsRTL)
		{
			if (width <= rect.Width())
			{
				rect.left += sz2.cx;
			}
			else
			{
				if (sz1.cx < rect.Width())
				{
					rect.left += max(0, sz2.cx +(rect.Width() - width));
				}
			}
		}

		if (bGlass)
		{
			DrawTextOnGlass(pDC, str1, rect, dwTextStyle, nGlassGlowSize, clrGlassText);
		}
		else
		{
			pDC->SetTextColor(clrText1);
			pDC->DrawText(str1, rect, dwTextStyle);
		}

		if (bIsRTL)
		{
			if (width <= (rect.right - left))
			{
				rect.right = rect.left;
				rect.left  = rect.right - sz2.cx;
			}
			else
			{
				rect.left = left;
				rect.right -= sz1.cx;
			}
		}
		else
		{
			rect.left += sz1.cx;
		}

		if (rect.right > rect.left)
		{
			if (bGlass)
			{
				DrawTextOnGlass(pDC, str2, rect, dwTextStyle, nGlassGlowSize, clrGlassText);
			}
			else
			{
				pDC->SetTextColor(clrText2);
				pDC->DrawText(str2, rect, dwTextStyle);
			}
		}
	}

	pDC->SetBkMode(nOldMode);
	pDC->SetTextColor(clrOldText);
}

void VisualManager::DrawNcCaption(CDC* pDC, CRect rectCaption, DWORD dwStyle, DWORD dwStyleEx, const CString& strTitle,
	const CString& strDocument, HICON hIcon, BOOL bPrefix, BOOL bActive, BOOL bTextCenter, const CObList& lstSysButtons)
{
	const BOOL bIsRTL = (dwStyleEx & WS_EX_LAYOUTRTL) == WS_EX_LAYOUTRTL;
	const int nSysCaptionHeight = ::GetSystemMetrics(SM_CYCAPTION);
	CSize szSysBorder(GetSystemBorders());

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(pDC, rectCaption.Width(), rectCaption.Height());
	CBitmap* pBmpOld = memDC.SelectObject(&memBmp);
	memDC.BitBlt(0, 0, rectCaption.Width(), rectCaption.Height(), pDC, 0, 0, SRCCOPY);

	BOOL bMaximized = (dwStyle & WS_MAXIMIZE) == WS_MAXIMIZE;

	{
		COLORREF clr1  = bActive ? m_clrAppCaptionActiveStart : m_clrAppCaptionInactiveStart;
		COLORREF clr2  = bActive ? m_clrAppCaptionActiveFinish : m_clrAppCaptionInactiveFinish;

		CRect rectCaption1(rectCaption);
		CRect rectBorder(m_ctrlMainBorderCaption.GetParams().m_rectSides);

		rectCaption1.DeflateRect(rectBorder.left, rectBorder.top, rectBorder.right, rectBorder.bottom);

		{
			CDrawingManager dm(memDC);
			dm.FillGradient2(rectCaption1, clr1, clr2, 90);
		}

		m_ctrlMainBorderCaption.DrawFrame(&memDC, rectCaption, bActive ? 0 : 1);
	}

	CRect rect(rectCaption);
	rect.DeflateRect(szSysBorder.cx, bMaximized ? szSysBorder.cy : 0, szSysBorder.cx, 0);

	// Draw icon:
	if (hIcon != NULL)
	{
		CSize szIcon(::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		CRect rectIcon(rect.TopLeft(), CSize(nSysCaptionHeight, rect.Height()));

		long x = rect.left + max(0, (rectIcon.Width()  - szIcon.cx) / 2);
		long y = rect.top  + max(0, (rectIcon.Height() - szIcon.cy) / 2);

		::DrawIconEx(memDC.GetSafeHdc(), x, y, hIcon, szIcon.cx, szIcon.cy, 0, NULL, DI_NORMAL);

		rect.left += rectIcon.Width();
	}

	// Draw system buttons:
	int xButtonsRight = rect.right;

	for (POSITION pos = lstSysButtons.GetHeadPosition(); pos != NULL;)
	{
		CMFCCaptionButtonEx* pButton = (CMFCCaptionButtonEx*)lstSysButtons.GetNext(pos);
		ASSERT_VALID(pButton);

		AFX_BUTTON_STATE state = ButtonsIsRegular;

		if (pButton->m_bPushed && pButton->m_bFocused)
		{
			state = ButtonsIsPressed;
		}
		else if (pButton->m_bFocused)
		{
			state = ButtonsIsHighlighted;
		}

		UINT uiHit = pButton->GetHit();
		UINT nButton = 0;

		switch(uiHit)
		{
		case AFX_HTCLOSE:
			nButton = SC_CLOSE;
			break;

		case AFX_HTMAXBUTTON:
			nButton = (dwStyle & WS_MAXIMIZE) == WS_MAXIMIZE ? SC_RESTORE : SC_MAXIMIZE;
			break;

		case AFX_HTMINBUTTON:
			nButton = (dwStyle & WS_MINIMIZE) == WS_MINIMIZE ? SC_RESTORE : SC_MINIMIZE;
			break;
		}

		CRect rectBtn(pButton->GetRect());

		if (bMaximized)
		{
			rectBtn.OffsetRect(szSysBorder.cx, szSysBorder.cy);
		}
		else
		{
			// recompute the position of the button
			// to avoid glitches with windows 7
			int cyBtn = rectBtn.Height();

			rectBtn.top = rectCaption.top + cyBtn / 4;
			rectBtn.bottom = rectBtn.top + cyBtn;

			pButton->SetRect(rectBtn);
		}

		DrawNcBtn(&memDC, rectBtn, nButton, state, FALSE, bActive, FALSE);

		xButtonsRight = min(xButtonsRight, pButton->GetRect().left);
	}

	// Draw text:
	if ((!strTitle.IsEmpty() || !strDocument.IsEmpty()) && rect.left < rect.right)
	{
		CFont* pOldFont = (CFont*)memDC.SelectObject(&m_AppCaptionFont);

		CRect rectText = rect;
		rectText.right = xButtonsRight - 1;

		DrawNcText(&memDC, rectText, strTitle, strDocument, bPrefix, bActive, bIsRTL, bTextCenter);

		memDC.SelectObject(pOldFont);
	}

	pDC->BitBlt(rectCaption.left, rectCaption.top, rectCaption.Width(), rectCaption.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pBmpOld);
}

/*	\brief Called by VisualManager::OnNcPaint() 

	Do exactly what CMFCVisualManagerOffice2003::OnNcPaint() except that it does not handled Ribbon
 */
BOOL VisualManager::NcPaint(CWnd* pWnd, const CObList& lstSysButtons, CRect rectRedraw)
{
	if (!CanDrawImage())
	{
		return CMFCVisualManagerOffice2003::OnNcPaint(pWnd, lstSysButtons, rectRedraw);
	}

	ASSERT_VALID(pWnd);

	if (pWnd->GetSafeHwnd() == NULL)
		return FALSE;

	CWindowDC dc(pWnd);

	if (dc.GetSafeHdc() != NULL)
	{
		CRgn rgn;
		if (!rectRedraw.IsRectEmpty())
		{
			rgn.CreateRectRgnIndirect(rectRedraw);
			dc.SelectClipRgn(&rgn);
		}

		CRect rtWindow;
		pWnd->GetWindowRect(rtWindow);
		pWnd->ScreenToClient(rtWindow);

		CRect rtClient;
		pWnd->GetClientRect(rtClient);

		rtClient.OffsetRect(-rtWindow.TopLeft());
		dc.ExcludeClipRect(rtClient);

		rtWindow.OffsetRect(-rtWindow.TopLeft());

		BOOL bActive = IsWindowActive(pWnd);

		// Modify bActive (if currently TRUE) for owner-drawn MDI child windows: draw child
		// frame active only if window is active MDI child and the MDI frame window is active.
		if (bActive && IsOwnerDrawCaption() && pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
		{
			CMDIFrameWnd *pParent = ((CMDIChildWnd *)pWnd)->GetMDIFrame();
			if (pParent)
			{
				CMDIChildWnd *pActiveChild = pParent->MDIGetActive(NULL);
				if (pActiveChild)
				{
					bActive = ((pActiveChild->GetSafeHwnd() == pWnd->GetSafeHwnd()) && IsWindowActive(pParent));
				}
			}
		}

		CRect rectCaption(rtWindow);
		CSize szSysBorder(GetSystemBorders());

		rectCaption.bottom = rectCaption.top + szSysBorder.cy;

		const DWORD dwStyle = pWnd->GetStyle();
		BOOL bMaximized = (dwStyle & WS_MAXIMIZE) == WS_MAXIMIZE;

		const int nSysCaptionHeight = ::GetSystemMetrics(SM_CYCAPTION);
		rectCaption.bottom += nSysCaptionHeight + 2;

		const DWORD dwStyleEx = pWnd->GetExStyle();

		HICON hIcon = afxGlobalUtils.GetWndIcon(pWnd);

		CString strText;
		pWnd->GetWindowText(strText);

		CString strTitle(strText);
		CString strDocument;

		BOOL bPrefix = FALSE;
		if ((dwStyle & FWS_ADDTOTITLE) == FWS_ADDTOTITLE)
		{
			bPrefix = (dwStyle & FWS_PREFIXTITLE) == FWS_PREFIXTITLE;
			CFrameWnd* pFrameWnd = DYNAMIC_DOWNCAST(CFrameWnd, pWnd);

			if (pFrameWnd != NULL)
			{
				strTitle = pFrameWnd->GetTitle();

				if (!strTitle.IsEmpty())
				{
					int pos = strText.Find(strTitle);

					if (pos != -1)
					{
						if (strText.GetLength() > strTitle.GetLength())
						{
							if (pos == 0)
							{
								bPrefix = FALSE; // avoid exception
								strTitle = strText.Left(strTitle.GetLength() + 3);
								strDocument = strText.Right(strText.GetLength() - strTitle.GetLength());
							}
							else
							{
								strTitle = strText.Right(strTitle.GetLength() + 3);
								strDocument = strText.Left(strText.GetLength() - strTitle.GetLength());
							}
						}
					}
				}
				else
				{
					strDocument = strText;
				}
			}
		}

		if (bMaximized)
		{
			rectCaption.InflateRect(szSysBorder.cx, szSysBorder.cy, szSysBorder.cx, 0);
		}

		DrawNcCaption(&dc, rectCaption, dwStyle, dwStyleEx, strTitle, strDocument, hIcon, bPrefix, bActive, m_bNcTextCenter, lstSysButtons);

		if (bMaximized)
		{
			return TRUE;
		}

		rtWindow.top = rectCaption.bottom;
		dc.ExcludeClipRect(rectCaption);

		if (pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
		{
			m_ctrlMDIChildBorder.DrawFrame(&dc, rtWindow, bActive ? 0 : 1);
		}
		else
		{
			m_ctrlMainBorder.DrawFrame(&dc, rtWindow, bActive ? 0 : 1);
		}

		//-------------------------------
		// Find status bar extended area:
		//-------------------------------
		CRect rectExt(0, 0, 0, 0);
		BOOL bExtended    = FALSE;
		BOOL bBottomFrame = FALSE;
		BOOL bIsStatusBar = FALSE;

		CWnd* pStatusBar = pWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR, TRUE);

		if (pStatusBar->GetSafeHwnd() != NULL && pStatusBar->IsWindowVisible())
		{
			CMFCStatusBar* pClassicStatusBar = DYNAMIC_DOWNCAST(CMFCStatusBar, pStatusBar);
			if (pClassicStatusBar != NULL)
			{
				bExtended = pClassicStatusBar->GetExtendedArea(rectExt);
				bIsStatusBar = TRUE;
			}

			else
			{
				CMFCRibbonStatusBar* pRibbonStatusBar = DYNAMIC_DOWNCAST(CMFCRibbonStatusBar, pStatusBar);
				if (pRibbonStatusBar != NULL)
				{
					bExtended    = pRibbonStatusBar->GetExtendedArea(rectExt);
					bBottomFrame = pRibbonStatusBar->IsBottomFrame();
					bIsStatusBar = TRUE;
				}
			}

		}

		if (bIsStatusBar)
		{
			CRect rectStatus;
			pStatusBar->GetClientRect(rectStatus);

			int nHeight = rectStatus.Height();
			rectStatus.bottom = rtWindow.bottom;
			rectStatus.top    = rectStatus.bottom - nHeight -(bBottomFrame ? -1 : szSysBorder.cy);
			rectStatus.left   = rtWindow.left;
			rectStatus.right  = rtWindow.right;

			if (bExtended)
			{
				rectExt.left   = rectStatus.right - rectExt.Width() - szSysBorder.cx;
				rectExt.top    = rectStatus.top;
				rectExt.bottom = rectStatus.bottom;
				rectExt.right  = rtWindow.right;
			}

			m_ctrlStatusBarBack.Draw(&dc, rectStatus, bActive ? 0 : 1);

			if (bExtended)
			{
				rectExt.left -= m_ctrlStatusBarBack_Ext.GetParams().m_rectCorners.left;
				m_ctrlStatusBarBack_Ext.Draw(&dc, rectExt, bActive ? 0 : 1);
			}
		}

		dc.SelectClipRgn(NULL);

		return TRUE;
	}

	return CMFCVisualManagerOffice2003::OnNcPaint(pWnd, lstSysButtons, rectRedraw);
}

BOOL VisualManager::OnNcPaint(CWnd* pWnd, const CObList& lstSysButtons, CRect rectRedraw)
{
	return NcPaint(pWnd, lstSysButtons, rectRedraw);
}

BOOL VisualManager::OnSetWindowRegion(CWnd* pWnd, CSize sizeWindow)
{
	ASSERT_VALID(pWnd);

	if (pWnd->GetSafeHwnd() == NULL)
	{
		return FALSE;
	}

	if (!CanDrawImage())
	{
		return FALSE;
	}

	if (afxGlobalData.DwmIsCompositionEnabled())
	{
		return FALSE;
	}

	CSize sz(0, 0);

	BOOL bMainWnd = FALSE;

	if (DYNAMIC_DOWNCAST(CMFCPopupMenu, pWnd) != NULL)
	{
		sz  = CSize(3, 3);
	}
	else if (DYNAMIC_DOWNCAST(CMFCRibbonBar, pWnd) != NULL)
	{
		return FALSE;
	}
	else
	{
		if ((pWnd->GetStyle() & WS_MAXIMIZE) == WS_MAXIMIZE)
		{
			pWnd->SetWindowRgn(NULL, TRUE);
			return TRUE;
		}

		sz  = CSize(9, 9);
		bMainWnd = TRUE;
	}

	if (sz != CSize(0, 0))
	{
		CRgn rgn;
		BOOL bCreated = FALSE;

		bCreated = rgn.CreateRoundRectRgn(0, 0, sizeWindow.cx + 1, sizeWindow.cy + 1, sz.cx, sz.cy);

		if (bCreated)
		{
			if (pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
			{
				CRgn rgnWinodw;
				rgnWinodw.CreateRectRgn(0, sz.cy, sizeWindow.cx, sizeWindow.cy);

				rgn.CombineRgn(&rgn, &rgnWinodw, RGN_OR);
			}

			pWnd->SetWindowRgn((HRGN)rgn.Detach(), TRUE);
			return TRUE;
		}
	}

	return FALSE;
}

CSize VisualManager::GetNcBtnSize(BOOL bSmall) const
{
	return m_szNcBtnSize[bSmall ? 1 : 0];
}

void VisualManager::DrawSeparator(CDC* pDC, const CRect& rect, BOOL bHorz)
{
	DrawSeparator(pDC, rect, m_penSeparator, m_penSeparator2, bHorz);
}

void VisualManager::DrawSeparator(CDC* pDC, const CRect& rect, CPen& pen1, CPen& pen2, BOOL bHorz)
{
	CRect rect1(rect);
	CRect rect2;

	if (bHorz)
	{
		rect1.top += rect.Height() / 2 - 1;
		rect1.bottom = rect1.top;
		rect2 = rect1;
		rect2.OffsetRect(0, 1);
	}
	else
	{
		rect1.left += rect.Width() / 2 - 1;
		rect1.right = rect1.left;
		rect2 = rect1;
		rect2.OffsetRect(1, 0);
	}

	if (CMFCToolBarImages::m_bIsDrawOnGlass)
	{
		CDrawingManager dm(*pDC);

		LOGPEN logpen;

		pen1.GetLogPen(&logpen);
		dm.DrawLine(rect1.left, rect1.top, rect1.right, rect1.bottom, logpen.lopnColor);

		pen2.GetLogPen(&logpen);
		dm.DrawLine(rect2.left, rect2.top, rect2.right, rect2.bottom, logpen.lopnColor);
	}
	else
	{
		CPen* pOldPen = pDC->SelectObject(&pen1);
		pDC->MoveTo(rect1.TopLeft());
		pDC->LineTo(rect1.BottomRight());

		pDC->SelectObject(&pen2);
		pDC->MoveTo(rect2.TopLeft());
		pDC->LineTo(rect2.BottomRight());

		pDC->SelectObject(pOldPen);
	}
}

COLORREF VisualManager::GetCaptionBarTextColor(CMFCCaptionBar* pBar)
{
	if (!CanDrawImage())
	{
		return CMFCVisualManagerOffice2003::GetCaptionBarTextColor(pBar);
	}

	return m_clrCaptionBarText;
}

void VisualManager::OnDrawCaptionBarInfoArea(CDC* pDC, CMFCCaptionBar* pBar, CRect rect)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawCaptionBarInfoArea(pDC, pBar, rect);
		return;
	}

	ASSERT_VALID(pDC);

	CDrawingManager dm(*pDC);
	dm.FillGradient(rect, afxGlobalData.clrBarFace, RGB(255, 255, 255));

	pDC->Draw3dRect(rect, afxGlobalData.clrBarDkShadow, afxGlobalData.clrBarDkShadow);
}

/*!	\brief Check if the specified pBar_in type is handled by VisualManager::OnFillBarBackground()
 *	\param[in] pBar_in Object to check
 *
 *	If the specified pBar_in type is not handled the VisualManager::OnFillBarBackground() function should not be called.
 *	This function should be used in a derived function to check if the root class function should be called
 */
bool VisualManager::IsFillBarBackgroundIsHandled(const CBasePane *pBar_in)
{
	CRuntimeClass* pBarClass = pBar_in->GetRuntimeClass();

	BOOL CanDraw = CanDrawImage();
	BOOL isDialog = pBar_in->IsDialogControl();
	BOOL isDerived = pBarClass->IsDerivedFrom(RUNTIME_CLASS(CMFCColorBar));

	if (isDialog || pBar_in->IsKindOf(RUNTIME_CLASS(CMFCMenuBar)))
	{
		return true;
	}
	else if (!CanDraw || /*isDialog ||*/ isDerived)
	{
		return true;
	}
	if (pBar_in->IsKindOf(RUNTIME_CLASS(CMFCPopupMenuBar)))
	{
		return true;
	}
	else if (pBar_in->IsKindOf(RUNTIME_CLASS(CMFCStatusBar)))
	{
		return true;
	}

	return false;
}

void VisualManager::OnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip, BOOL bNCArea/* = FALSE*/)
{
	CRuntimeClass* pBarClass = pBar->GetRuntimeClass();

	BOOL CanDraw = CanDrawImage();
	BOOL isDialog = pBar->IsDialogControl();
	BOOL isDerived = pBarClass->IsDerivedFrom(RUNTIME_CLASS(CMFCColorBar));

	if (isDialog || pBar->IsKindOf(RUNTIME_CLASS(CMFCMenuBar)))
	{
		CDrawingManager dm(*pDC);

		dm.Fill4ColorsGradient(rectClient, m_clrMenuBarGradientDark, m_clrMenuBarGradientLight,
							   m_clrMenuBarGradientLight2, m_clrMenuBarGradientDark2, TRUE, 60);

		return;
	}
	else if (!CanDraw || /*isDialog ||*/ isDerived)
	{
		CMFCVisualManagerOffice2003::OnFillBarBackground(pDC, pBar, rectClient, rectClip, bNCArea);
		return;
	}
	if (pBar->IsKindOf(RUNTIME_CLASS(CMFCPopupMenuBar)))
	{
		pDC->FillRect(rectClip, &m_brMenuLight);

		CMFCPopupMenuBar* pMenuBar = DYNAMIC_DOWNCAST(CMFCPopupMenuBar, pBar);
		if (!pMenuBar->m_bDisableSideBarInXPMode)
		{
			CRect rectImages = rectClient;
			rectImages.right = rectImages.left + pMenuBar->GetGutterWidth();
			rectImages.DeflateRect (0, 1);

			pDC->FillRect(rectImages, &m_brBarBkgnd);

			rectImages.left = rectImages.right;
			rectImages.right += 2;
			DrawSeparator(pDC, rectImages, FALSE);
		}

		return;
	}
	else if (pBar->IsKindOf(RUNTIME_CLASS(CMFCStatusBar)))
	{
		CSize szSysBorder(GetSystemBorders());

		CRect rect(rectClient);
		CRect rectExt(0, 0, 0, 0);
		BOOL bExtended = ((CMFCStatusBar*)pBar)->GetExtendedArea(rectExt);

		if (bExtended)
		{
			rect.right = rectExt.left;
		}

		CWnd* pWnd = ((CMFCStatusBar*)pBar)->GetParent();
		ASSERT_VALID(pWnd);

		BOOL bActive = IsWindowActive(pWnd);

		rect.InflateRect(szSysBorder.cx, 0, szSysBorder.cx, szSysBorder.cy);
		m_ctrlStatusBarBack.Draw(pDC, rect, bActive ? 0 : 1);

		if (bExtended)
		{
			rectExt.InflateRect(0, 0, szSysBorder.cx, szSysBorder.cy);
			rectExt.left -= m_ctrlStatusBarBack_Ext.GetParams().m_rectCorners.left;
			m_ctrlStatusBarBack_Ext.Draw(pDC, rectExt, bActive ? 0 : 1);
		}

		return;
	}

	CMFCVisualManagerOffice2003::OnFillBarBackground(pDC, pBar, rectClient, rectClip, bNCArea);
}

void VisualManager::OnFillHighlightedArea(CDC* pDC, CRect rect, CBrush* pBrush, CMFCToolBarButton* pButton)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnFillHighlightedArea(pDC, rect, pBrush, pButton);
		return;
	}

	ASSERT_VALID(pDC);
	ASSERT_VALID(pBrush);

	BOOL bIsHorz = TRUE;

	COLORREF clr1 = (COLORREF)-1;
	COLORREF clr2 = (COLORREF)-1;

	if (pButton != NULL)
	{
		ASSERT_VALID(pButton);

		bIsHorz = pButton->IsHorizontal();

		CMFCToolBarMenuButton* pCustButton = DYNAMIC_DOWNCAST(CMFCCustomizeButton, pButton);

		if (pCustButton != NULL)
		{
			if (pButton->IsDroppedDown())
			{
				clr1 = m_clrHighlightDnGradientDark;
				clr2 = m_clrHighlightDnGradientLight;
			}
		}
	}

	if (pBrush == &m_brHighlight)
	{
		clr1 = m_clrHighlightGradientDark;
		clr2 = m_clrHighlightGradientLight;//bIsPopupMenu ? clr1 : m_clrHighlightGradientLight;
	}
	else if (pBrush == &m_brHighlightDn)
	{
		clr1 = m_clrHighlightDnGradientDark;//bIsPopupMenu ? m_clrHighlightDnGradientLight : m_clrHighlightDnGradientDark;
		clr2 = m_clrHighlightDnGradientLight;
	}
	else if (pBrush == &m_brHighlightChecked)
	{
		clr1 = m_clrHighlightCheckedGradientDark;//bIsPopupMenu ? m_clrHighlightCheckedGradientLight : m_clrHighlightCheckedGradientDark;
		clr2 = m_clrHighlightCheckedGradientLight;
	}

	if (clr1 == (COLORREF)-1 || clr2 == (COLORREF)-1)
	{
		CMFCVisualManagerOffice2003::OnFillHighlightedArea(pDC, rect, pBrush, pButton);
		return;
	}

	CDrawingManager dm(*pDC);
	dm.FillGradient(rect, clr1, clr2, bIsHorz);
}

void VisualManager::OnDrawBarGripper(CDC* pDC, CRect rectGripper, BOOL bHorz, CBasePane* pBar)
{
	if (!CanDrawImage() ||/* (pBar != NULL && pBar->IsDialogControl()) || */m_ToolBarGripper.GetCount() == 0)
	{
		CMFCVisualManagerOffice2003::OnDrawBarGripper(pDC, rectGripper, bHorz, pBar);
		return;
	}

	CSize szBox(m_ToolBarGripper.GetImageSize());

	if (szBox != CSize(0, 0))
	{
		if (bHorz)
		{
			rectGripper.left = rectGripper.right - szBox.cx;
		}
		else
		{
			rectGripper.top = rectGripper.bottom - szBox.cy;
		}

		CMFCToolBar* pToolBar = DYNAMIC_DOWNCAST(CMFCToolBar, pBar);
		if (pToolBar != NULL)
		{
			if (bHorz)
			{
				const int nHeight = CMFCToolBar::IsLargeIcons() ? pToolBar->GetRowHeight() : pToolBar->GetButtonSize().cy;
				const int nDelta = max(0, (nHeight - pToolBar->GetImageSize().cy) / 2);
				rectGripper.DeflateRect(0, nDelta);
			}
			else
			{
				const int nWidth = CMFCToolBar::IsLargeIcons() ? pToolBar->GetColumnWidth() : pToolBar->GetButtonSize().cx;
				const int nDelta = max(0, (nWidth - pToolBar->GetImageSize().cx) / 2);
				rectGripper.DeflateRect(nDelta, 0);
			}
		}

		const int nBoxesNumber = bHorz ? (rectGripper.Height() - szBox.cy) / szBox.cy : (rectGripper.Width() - szBox.cx) / szBox.cx;
		int nOffset = bHorz ? (rectGripper.Height() - nBoxesNumber * szBox.cy) / 2 : (rectGripper.Width() - nBoxesNumber * szBox.cx) / 2;

		for (int nBox = 0; nBox < nBoxesNumber; nBox++)
		{
			int x = bHorz ? rectGripper.left :
			rectGripper.left + nOffset;

			int y = bHorz ? rectGripper.top + nOffset :
			rectGripper.top;

			m_ToolBarGripper.DrawEx(pDC, CRect(CPoint(x, y), szBox), 0);
			nOffset += bHorz ? szBox.cy : szBox.cx;
		}
	}
}

void VisualManager::OnDrawSeparator(CDC* pDC, CBasePane* pBar, CRect rect, BOOL bHorz)
{
	ASSERT_VALID(pDC);

	if (!CanDrawImage() || pBar == NULL/* || pBar->IsDialogControl()*/)
	{
		CMFCVisualManagerOffice2003::OnDrawSeparator(pDC, pBar, rect, bHorz);
		return;
	}

	ASSERT_VALID(pBar);

	CRect rectSeparator(rect);

	if (pBar->IsKindOf(RUNTIME_CLASS(CMFCRibbonStatusBar)))
	{
		CMFCRibbonStatusBar* pRibbonStatusBar = DYNAMIC_DOWNCAST(CMFCRibbonStatusBar, pBar);

		rect.InflateRect(1, 5, 1, pRibbonStatusBar->IsBottomFrame() ? 2 : 5);

		m_StatusBarPaneBorder.DrawEx(pDC, rect, 0, CMFCToolBarImages::ImageAlignHorzRight, CMFCToolBarImages::ImageAlignVertStretch);
		return;
	}

	if (pBar->IsKindOf(RUNTIME_CLASS(CMFCRibbonBar)) || (bHorz && pBar->IsKindOf(RUNTIME_CLASS(CMFCRibbonPanelMenuBar))))
	{
		if (rect.Width() < m_RibbonPanelSeparator.GetImageSize().cx)
		{
			rect.left = rect.right - m_RibbonPanelSeparator.GetImageSize().cx;
		}

		m_RibbonPanelSeparator.DrawEx(pDC, rect, 0, CMFCToolBarImages::ImageAlignHorzCenter, CMFCToolBarImages::ImageAlignVertCenter);
		return;
	}

	BOOL bPopupMenu = FALSE;

	if (!bHorz)
	{
		BOOL bIsRibbon = FALSE;
		bIsRibbon = pBar->IsKindOf(RUNTIME_CLASS(CMFCRibbonPanelMenuBar));

		if (bIsRibbon &&((CMFCRibbonPanelMenuBar*) pBar)->IsDefaultMenuLook())
		{
			bIsRibbon = FALSE;
		}

		bPopupMenu = pBar->IsKindOf(RUNTIME_CLASS(CMFCPopupMenuBar));

		if (bPopupMenu && !bIsRibbon && !pBar->IsKindOf(RUNTIME_CLASS(CMFCColorBar)))
		{
			rectSeparator.left = rect.left + CMFCToolBar::GetMenuImageSize().cx + GetMenuImageMargin() + 1;

			CRect rectBar;
			pBar->GetClientRect(rectBar);

			if (rectBar.right - rectSeparator.right < 50) // Last item in row
			{
				rectSeparator.right = rectBar.right;
			}

			if (((CMFCPopupMenuBar*) pBar)->m_bDisableSideBarInXPMode)
			{
				rectSeparator.left = 0;
			}

			//---------------------------------
			// Maybe Quick Customize separator
			//---------------------------------
			if (bPopupMenu)
			{
				CWnd* pWnd = pBar->GetParent();
				if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CMFCPopupMenu)))
				{
					CMFCPopupMenu* pMenu = (CMFCPopupMenu*)pWnd;
					if (pMenu->IsCustomizePane())
					{
						rectSeparator.left = rect.left + 2 * CMFCToolBar::GetMenuImageSize().cx + 3 * GetMenuImageMargin() + 2;
					}
				}
			}
		}
	}

	if (bPopupMenu)
	{
		DrawSeparator(pDC, rectSeparator, !bHorz);
	}
	else
	{
		if (bHorz)
		{
			int nHeight = rectSeparator.Height() / 5;
			rectSeparator.top    += nHeight;
			rectSeparator.bottom -= nHeight;
		}
		else
		{
			int nWidth = rectSeparator.Width() / 5;
			rectSeparator.left  += nWidth;
			rectSeparator.right -= nWidth;
		}

		DrawSeparator(pDC, rectSeparator, m_penSeparatorDark, m_penSeparatorLight, !bHorz);
	}
}

COLORREF VisualManager::OnDrawPaneCaption(CDC* pDC, CDockablePane* pBar, BOOL bActive, CRect rectCaption, CRect rectButtons)
{
	if (!CanDrawImage() || pBar == NULL/* || pBar->IsDialogControl()*/)
	{
		return CMFCVisualManagerOffice2003::OnDrawPaneCaption(pDC, pBar, bActive, rectCaption, rectButtons);
	}

	ASSERT_VALID(pDC);
	COLORREF clrCaptionBorder = bActive ? m_clrPaneActiveCaptionBorder : m_clrPaneInactiveCaptionBorder;

	CPen pen(PS_SOLID, 1, clrCaptionBorder);
	CPen* pOldPen = pDC->SelectObject(&pen);

	rectCaption.bottom += 2;

	pDC->MoveTo(rectCaption.left, rectCaption.bottom);
	pDC->LineTo(rectCaption.left, rectCaption.top);

	pDC->MoveTo(rectCaption.left  + 1, rectCaption.top);
	pDC->LineTo(rectCaption.right - 1, rectCaption.top);

	pDC->MoveTo(rectCaption.right - 1, rectCaption.top + 1);
	pDC->LineTo(rectCaption.right - 1, rectCaption.bottom);

	pDC->SelectObject(pOldPen);

	rectCaption.DeflateRect(1, 1, 1, 0);

	if (bActive)
	{
		pDC->FillRect(rectCaption, &afxGlobalData.brActiveCaption);
	}
	else
	{
		CDrawingManager dm(*pDC);

		dm.Fill4ColorsGradient(rectCaption, m_clrMenuBarGradientDark, m_clrMenuBarGradientLight,
							   m_clrMenuBarGradientLight2, m_clrMenuBarGradientDark2, TRUE, 60);
	}

	return bActive ? afxGlobalData.clrCaptionText : afxGlobalData.clrInactiveCaptionText;
}

void VisualManager::OnDrawStatusBarPaneBorder(CDC* pDC, CMFCStatusBar* pBar, CRect rectPane, UINT uiID, UINT nStyle)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawStatusBarPaneBorder(pDC, pBar, rectPane, uiID, nStyle);
		return;
	}

	BOOL bExtended = pBar->GetDrawExtendedArea();
	if (!bExtended ||((nStyle & SBPS_STRETCH) == 0 && bExtended))
	{
		rectPane.OffsetRect(1, 0);
		m_StatusBarPaneBorder.DrawEx(pDC, rectPane, 0, CMFCToolBarImages::ImageAlignHorzRight, CMFCToolBarImages::ImageAlignVertStretch);
	}
}

void VisualManager::OnDrawStatusBarSizeBox(CDC* pDC, CMFCStatusBar* pStatBar, CRect rectSizeBox)
{
	if (!CanDrawImage() ||
		m_StatusBarSizeBox.GetCount() == 0)
	{
		CMFCVisualManagerOffice2003::OnDrawStatusBarSizeBox(pDC, pStatBar, rectSizeBox);
		return;
	}

	m_StatusBarSizeBox.DrawEx(pDC, rectSizeBox, 0, CMFCToolBarImages::ImageAlignHorzRight, CMFCToolBarImages::ImageAlignVertBottom);
}

/*!	\brief Check if the specified pBar_in type is handled by VisualManager::OnDrawComboDropButton()
	\param[in] pButton_in Combo box to check
	\returns true if the type of combo box is handled else false

	If the specified pButton_in type is not handled the VisualManager::OnDrawComboDropButton() function should not be called.
	This function should be used in a derived function to check if the root class function should be called
 */
bool VisualManager::IsDrawComboDropButtonIsHandled(const CMFCToolBarComboBoxButton* pButton_in)
{
	if (CanDrawImage() == false)
		return true;

	if(pButton_in && pButton_in->IsRibbonButton())
		return false; // Ribbon button are not supported

	return true;
}


void VisualManager::OnDrawComboDropButton(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted, CMFCToolBarComboBoxButton* pButton)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawComboDropButton(pDC, rect, bDisabled, bIsDropped, bIsHighlighted, pButton);
		return;
	}

	BOOL bActive = bIsHighlighted || bIsDropped;

	CMFCControlRenderer* pRenderer = &m_ctrlComboBoxBtn;

	if (!pRenderer->IsValid())
	{
		COLORREF color1 = m_clrComboBtnStart;
		COLORREF color2 = m_clrComboBtnFinish;
		COLORREF colorBorder = m_clrComboBtnBorder;
		if (bDisabled)
		{
			color1 = m_clrComboBtnDisabledStart;
			color2 = m_clrComboBtnDisabledFinish;
			colorBorder = m_clrComboBtnBorderDisabled;
		}
		else if (bActive)
		{
			if (bIsDropped)
			{
				color1 = m_clrComboBtnPressedStart;
				color2 = m_clrComboBtnPressedFinish;
				colorBorder = m_clrComboBtnBorderPressed;
			}
			else
			{
				color1 = m_clrComboBtnHighlightedStart;
				color2 = m_clrComboBtnHighlightedFinish;
				colorBorder = m_clrComboBtnBorderHighlighted;
			}
		}

		if (!bDisabled || (bDisabled && colorBorder != (COLORREF)(-1)))
		{
			if (!bDisabled)
			{
				rect.InflateRect(0, 1, 1, 1);
			}

			if (CMFCToolBarImages::m_bIsDrawOnGlass)
			{
				CDrawingManager dm(*pDC);
				dm.DrawRect(rect, (COLORREF)-1, colorBorder);
			}
			else
			{
				pDC->Draw3dRect(rect, colorBorder, colorBorder);
			}

			if (!bDisabled)
			{
				rect.DeflateRect(0, 1, 1, 1);
			}
		}

		if (bDisabled)
		{
			rect.DeflateRect(0, 1, 1, 1);
		}
		else if (bActive)
		{
			rect.DeflateRect(1, 0, 0, 0);
		}

		CDrawingManager dm(*pDC);
		dm.FillGradient(rect, color1, color2, TRUE);

		if (bDisabled)
		{
			rect.InflateRect(0, 1, 1, 1);
		}
		else if (bActive)
		{
			rect.InflateRect(1, 0, 0, 0);
		}
	}
	else
	{
		rect.InflateRect(0, 1, 1, 1);

		int nIndex = 0;
		if (bDisabled)
		{
			nIndex = 3;
		}
		else
		{
			if (bIsDropped)
			{
				nIndex = 2;
			}
			else if (bIsHighlighted)
			{
				nIndex = 1;
			}
		}

		pRenderer->Draw(pDC, rect, nIndex);

		rect.DeflateRect(0, 1, 1, 1);
	}

	rect.bottom -= 2;

	CMenuImages::Draw(pDC, CMenuImages::IdArrowDown, rect, bDisabled ? CMenuImages::ImageGray : CMenuImages::ImageBlack);
}

void VisualManager::OnDrawComboBorder(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted, CMFCToolBarComboBoxButton* pButton)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawComboBorder(pDC, rect, bDisabled, bIsDropped, bIsHighlighted, pButton);
		return;
	}

	rect.DeflateRect(1, 1);

	COLORREF colorBorder = m_clrComboBorder;

	if (bDisabled)
	{
		colorBorder = m_clrComboBorderDisabled;
	}
	else if (bIsHighlighted || bIsDropped)
	{
		colorBorder = bIsDropped ? m_clrComboBorderPressed : m_clrComboBorderHighlighted;
	}

	if (CMFCToolBarImages::m_bIsDrawOnGlass)
	{
		CDrawingManager dm(*pDC);
		dm.DrawRect(rect, (COLORREF)-1, colorBorder);
	}
	else
	{
		pDC->Draw3dRect(&rect, colorBorder, colorBorder);
	}
}

void VisualManager::OnDrawEditBorder(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsHighlighted, CMFCToolBarEditBoxButton* pButton)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawEditBorder(pDC, rect, bDisabled, bIsHighlighted, pButton);
		return;
	}

	rect.DeflateRect(1, 1);

	COLORREF colorBorder = m_clrEditBorder;

	if (bDisabled)
	{
		colorBorder = m_clrEditBorderDisabled;
	}
	else if (bIsHighlighted)
	{
		colorBorder = m_clrEditBorderHighlighted;
	}

	if (CMFCToolBarImages::m_bIsDrawOnGlass)
	{
		CDrawingManager dm(*pDC);
		dm.DrawRect(rect, (COLORREF)-1, colorBorder);
	}
	else
	{
		pDC->Draw3dRect(&rect, colorBorder, colorBorder);
	}
}

void VisualManager::OnDrawTearOffCaption(CDC* pDC, CRect rect, BOOL bIsActive)
{
	if (!CanDrawImage() || m_ToolBarTear.GetCount() == 0)
	{
		CMFCVisualManagerOffice2003::OnDrawTearOffCaption(pDC, rect, bIsActive);
		return;
	}

	pDC->FillRect(rect, &m_brBarBkgnd);
	if (bIsActive)
	{
		m_ctrlMenuHighlighted[0].Draw(pDC, rect);
	}

	m_ToolBarTear.DrawEx(pDC, rect, 0, CMFCToolBarImages::ImageAlignHorzCenter, CMFCToolBarImages::ImageAlignVertCenter);
}

void VisualManager::OnDrawMenuResizeBar(CDC* pDC, CRect rect, int nResizeFlags)
{
	CMFCToolBarImages& images = (nResizeFlags == (int) CMFCPopupMenu::MENU_RESIZE_BOTTOM_RIGHT) ? m_PopupResizeBar_HV :
	(nResizeFlags == (int) CMFCPopupMenu::MENU_RESIZE_TOP_RIGHT) ? m_PopupResizeBar_HVT : m_PopupResizeBar_V;

	if (!CanDrawImage() || !m_ctrlPopupResizeBar.IsValid() || !images.IsValid())
	{
		CMFCVisualManagerOffice2003::OnDrawMenuResizeBar(pDC, rect, nResizeFlags);
		return;
	}

	ASSERT_VALID(pDC);

	m_ctrlPopupResizeBar.Draw(pDC, rect);

	if (nResizeFlags == (int) CMFCPopupMenu::MENU_RESIZE_BOTTOM_RIGHT || nResizeFlags == (int) CMFCPopupMenu::MENU_RESIZE_TOP_RIGHT)
	{
		images.DrawEx(pDC, rect, 0, CMFCToolBarImages::ImageAlignHorzRight,
			nResizeFlags == (int) CMFCPopupMenu::MENU_RESIZE_TOP_RIGHT ? CMFCToolBarImages::ImageAlignVertTop : CMFCToolBarImages::ImageAlignVertBottom);
	}
	else
	{
		images.DrawEx(pDC, rect, 0, CMFCToolBarImages::ImageAlignHorzCenter, CMFCToolBarImages::ImageAlignVertCenter);
	}
}

void VisualManager::OnDrawMenuScrollButton(CDC* pDC, CRect rect, BOOL bIsScrollDown, BOOL bIsHighlited, BOOL bIsPressed, BOOL bIsDisabled)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawMenuScrollButton(pDC, rect, bIsScrollDown, bIsHighlited, bIsPressed, bIsDisabled);
		return;
	}

	ASSERT_VALID(pDC);

	CMFCControlRenderer* pRenderer = &m_ctrlMenuScrollBtn[0];

	if (bIsScrollDown && m_ctrlMenuScrollBtn[1].IsValid())
	{
		pRenderer = &m_ctrlMenuScrollBtn[1];
	}

	rect.top --;

	pRenderer->Draw(pDC, rect, bIsHighlited ? 1 : 0);

	CMenuImages::Draw(pDC, bIsScrollDown ? CMenuImages::IdArrowDown : CMenuImages::IdArrowUp, rect);
}

/*!	\brief Called by VisualManager::OnDrawMenuSystemButton()
	\param[in,out] pDC ???
	\param[in] rect ???
	\param[in] uiSystemCommand ???
	\param[in] nStyle ???
	\param[in] bHighlight ???
	\returns true if the draw has been handled else false

	The function content comes from the CMFCVisualManagerOffice2003::OnDrawMenuSystemButton()
	This function do exactly what the previous function do except that it returns false
	when it should do an unsupported operation by this version (e.g. : m_ctrlRibbonBtn[0].Draw(pDC, rect, index); )
 */
bool VisualManager::DrawMenuSystemButton(CDC* pDC, CRect rect, UINT uiSystemCommand, UINT nStyle, BOOL bHighlight)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawMenuSystemButton(pDC, rect, uiSystemCommand, nStyle, bHighlight);
		return true;
	}

	ASSERT_VALID(pDC);

	CMFCToolBarImages* pImage = NULL;

	switch(uiSystemCommand)
	{
	case SC_CLOSE:
		pImage = &m_SysBtnClose[0];
		break;

	case SC_MINIMIZE:
		pImage = &m_SysBtnMinimize[0];
		break;

	case SC_RESTORE:
		pImage = &m_SysBtnRestore[0];
		break;

	default:
		return true;
	}

	BOOL bDisabled = (nStyle & TBBS_DISABLED);
	BOOL bPressed = (nStyle & TBBS_PRESSED);

	CRect rtBtnImage(CPoint(0, 0), pImage->GetImageSize());

	int nImage = 0;
	if (bDisabled)
	{
		nImage = 3;
	}
	else if (bPressed || bHighlight)
	{
		int index = -1;
		if (bPressed)
		{
			if (bHighlight)
			{
				index = 1;
			}
		}
		else if (bHighlight)
		{
			index = 0;
		}

		if (index != -1)
		{
			return false;
		}
	}

	rtBtnImage.OffsetRect(0, pImage->GetImageSize().cy * nImage);
	pImage->DrawEx(pDC, rect, 0, CMFCToolBarImages::ImageAlignHorzCenter, CMFCToolBarImages::ImageAlignVertCenter, rtBtnImage);

	return true;
}
	

void VisualManager::OnDrawMenuSystemButton(CDC* pDC, CRect rect, UINT uiSystemCommand, UINT nStyle, BOOL bHighlight)
{
	DrawMenuSystemButton(pDC, rect, uiSystemCommand, nStyle, bHighlight);
}

void VisualManager::OnFillButtonInterior(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnFillButtonInterior(pDC, pButton, rect, state);
		return;
	}

	CMFCCustomizeButton* pCustButton = DYNAMIC_DOWNCAST(CMFCCustomizeButton, pButton);

	if (pCustButton == NULL)
	{
		if (CMFCToolBar::IsCustomizeMode() && !CMFCToolBar::IsAltCustomizeMode() && !pButton->IsLocked())
		{
			return;
		}

		CMFCControlRenderer* pRenderer = NULL;
		int index = 0;

		BOOL bDisabled = (pButton->m_nStyle & TBBS_DISABLED) == TBBS_DISABLED;
		BOOL bPressed  = (pButton->m_nStyle & TBBS_PRESSED ) == TBBS_PRESSED;
		BOOL bChecked  = (pButton->m_nStyle & TBBS_CHECKED ) == TBBS_CHECKED;
		BOOL bHandled  = FALSE;

		CBasePane* pBar = DYNAMIC_DOWNCAST(CBasePane, pButton->GetParentWnd());

		CMFCToolBarMenuButton* pMenuButton = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, pButton);
		if (pMenuButton != NULL && pBar != NULL)
		{
			if (pBar->IsKindOf(RUNTIME_CLASS(CMFCMenuBar)))
			{
				if (state == ButtonsIsPressed || state == ButtonsIsHighlighted)
				{
					if (pMenuButton->IsDroppedDown())
					{
						ExtendMenuButton(pMenuButton, rect);
						index = 1;
					}

					pRenderer = &m_ctrlMenuBarBtn;

					bHandled = TRUE;
				}
				else
				{
					return;
				}

				bHandled = TRUE;
			}
			else if (pBar->IsKindOf(RUNTIME_CLASS(CMFCPopupMenuBar)))
			{
				if (bChecked)
				{
					pRenderer = &m_ctrlMenuItemBack;

					if (bDisabled)
					{
						index = 1;
					}

					rect.InflateRect(0, 0, 0, 1);
					bHandled = TRUE;
				}
				else if (state == ButtonsIsPressed || state == ButtonsIsHighlighted)
				{
					pRenderer = &m_ctrlMenuHighlighted[bDisabled ? 1 : 0];
					bHandled = TRUE;
				}
				else
				{
					return;
				}
			}
			else if (pBar->IsKindOf(RUNTIME_CLASS(CMFCToolBar)))
			{
				if (pMenuButton->IsDroppedDown())
				{
					ExtendMenuButton(pMenuButton, rect);
				}
			}
		}
		else if (pBar != NULL && pBar->IsKindOf(RUNTIME_CLASS(CMFCColorBar)))
		{
			if (bChecked)
			{
				pRenderer = &m_ctrlMenuItemBack;

				if (bDisabled)
				{
					index = 1;
				}
			}


			if (!bDisabled)
			{
				if (state == ButtonsIsHighlighted)
				{
					pRenderer = &m_ctrlMenuHighlighted[0];
					index = 0;
				}
			}

			bHandled = TRUE;
		}
		else if (pBar != NULL && pBar->IsKindOf(RUNTIME_CLASS(CMFCOutlookBarToolBar)))
		{
			bHandled = TRUE;
		}

		if (!bHandled)
		{
			index = -1;

			if (bChecked)
			{
				if (bDisabled)
				{
					index = 0;
				}
				else if (state == ButtonsIsPressed || state == ButtonsIsHighlighted)
				{
					index = 3;
				}
			}

			if (!bDisabled)
			{
				if (bPressed)
				{
					index = 2;
				}
				else if (state == ButtonsIsHighlighted)
				{
					if (index == -1)
					{
						index = 0;
					}

					index++;
				}
			}

			if (index == -1)
			{
				return;
			}

			pRenderer = &m_ctrlToolBarBtn;
		}

		if (pRenderer != NULL)
		{
			pRenderer->Draw(pDC, rect, index);
			return;
		}
	}

	CMFCVisualManagerOffice2003::OnFillButtonInterior(pDC, pButton, rect, state);
}

void VisualManager::OnDrawButtonBorder(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawButtonBorder(pDC, pButton, rect, state);
		return;
	}

	//------------------------------------------------
	// Draw shadow under the dropped-down menu button:
	//------------------------------------------------
	if (state != ButtonsIsPressed && state != ButtonsIsHighlighted)
	{
		return;
	}

	if (!m_bShdowDroppedDownMenuButton || !CMFCMenuBar::IsMenuShadows() || CMFCToolBar::IsCustomizeMode())
	{
		return;
	}

	CMFCToolBarMenuButton* pMenuButton = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, pButton);
	if (pMenuButton == NULL || !pMenuButton->IsDroppedDown())
	{
		return;
	}

	BOOL bIsPopupMenu = pMenuButton->GetParentWnd() != NULL && pMenuButton->GetParentWnd()->IsKindOf(RUNTIME_CLASS(CMFCPopupMenuBar));

	if (bIsPopupMenu)
	{
		return;
	}

	CMFCPopupMenu* pPopupMenu= pMenuButton->GetPopupMenu();
	if (pPopupMenu != NULL && (pPopupMenu->IsWindowVisible() || pPopupMenu->IsShown()) && !pPopupMenu->IsRightAlign() && !(pPopupMenu->GetExStyle() & WS_EX_LAYOUTRTL))
	{
		ExtendMenuButton(pMenuButton, rect);

		CDrawingManager dm(*pDC);

		dm.DrawShadow(rect, m_nMenuShadowDepth, 100, 75, NULL, NULL, m_clrMenuShadowBase);
	}
}

void VisualManager::OnDrawButtonSeparator(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state, BOOL bHorz)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawButtonSeparator(pDC, pButton, rect, state, bHorz);
		return;
	}

	CPen* pPen = &m_penMenuItemBorder;

	CPen* pOldPen = pDC->SelectObject(pPen);
	ENSURE(pOldPen != NULL);

	if (bHorz)
	{
		pDC->MoveTo(rect.left, rect.top + 2);
		pDC->LineTo(rect.left, rect.bottom - 2);
	}
	else
	{
		pDC->MoveTo(rect.left  + 2, rect.top);
		pDC->LineTo(rect.right - 2, rect.top);
	}

	pDC->SelectObject(pOldPen);
}

void VisualManager::OnHighlightMenuItem(CDC *pDC, CMFCToolBarMenuButton* pButton, CRect rect, COLORREF& clrText)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnHighlightMenuItem(pDC, pButton, rect, clrText);
		return;
	}

	m_ctrlMenuHighlighted[(pButton->m_nStyle & TBBS_DISABLED) == TBBS_DISABLED ? 1 : 0].Draw(pDC, rect);
}

void VisualManager::OnHighlightRarelyUsedMenuItems(CDC* pDC, CRect rectRarelyUsed)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnHighlightRarelyUsedMenuItems(pDC, rectRarelyUsed);
	}

	rectRarelyUsed.left --;
	rectRarelyUsed.right = rectRarelyUsed.left + CMFCToolBar::GetMenuImageSize().cx + 2 * GetMenuImageMargin() + 2;

	pDC->FillRect(rectRarelyUsed, &m_brMenuRarelyUsed);
}

void VisualManager::OnDrawMenuCheck(CDC* pDC, CMFCToolBarMenuButton* pButton, CRect rect, BOOL bHighlight, BOOL bIsRadio)
{
	ASSERT_VALID(pButton);

	CMFCToolBarImages& img = bIsRadio ? m_MenuItemMarkerR : m_MenuItemMarkerC;

	if (!CanDrawImage() || img.GetCount() == 0)
	{
		CMFCVisualManagerOffice2003::OnDrawMenuCheck(pDC, pButton, rect, bHighlight, bIsRadio);
		return;
	}

	CSize size(img.GetImageSize());
	CRect rectImage(0, 0, size.cx, size.cy);

	if ((pButton->m_nStyle & TBBS_DISABLED) == TBBS_DISABLED)
	{
		rectImage.OffsetRect(0, size.cy);
	}

	if (afxGlobalData.m_bIsRTL)
	{
		img.Mirror();
	}

	img.DrawEx(pDC, rect, 0, CMFCToolBarImages::ImageAlignHorzCenter, CMFCToolBarImages::ImageAlignVertCenter, rectImage);

	if (afxGlobalData.m_bIsRTL)
	{
		img.Mirror();
	}
}

void VisualManager::OnDrawMenuItemButton(CDC* pDC, CMFCToolBarMenuButton* pButton, CRect rectButton, BOOL bHighlight, BOOL bDisabled)
{
	if (!CanDrawImage() || !m_ctrlMenuButtonBorder.IsValid())
	{
		CMFCVisualManagerOffice2003::OnDrawMenuItemButton(pDC, pButton, rectButton, bHighlight, bDisabled);
		return;
	}

	ASSERT_VALID(pDC);

	CRect rect = rectButton;
	rect.right = rect.left + 1;
	rect.left--;
	rect.DeflateRect(0, 1);

	if (bHighlight)
	{
		m_ctrlMenuButtonBorder.Draw(pDC, rect);
	}
	else
	{
		CBrush br(afxGlobalData.clrBtnShadow);

		rect.DeflateRect(0, 3);
		rect.right--;
		pDC->FillRect(rect, &br);
	}
}

void VisualManager::OnDrawShowAllMenuItems(CDC* pDC, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state)
{
	if (!CanDrawImage() || !m_ctrlMenuItemShowAll.IsValid())
	{
		CMFCVisualManagerOffice2003::OnDrawShowAllMenuItems(pDC, rect, state);
		return;
	}

	m_ctrlMenuItemShowAll.FillInterior(pDC, rect, CMFCToolBarImages::ImageAlignHorzCenter, CMFCToolBarImages::ImageAlignVertCenter, state == ButtonsIsHighlighted ? 1 : 0);
}

int VisualManager::GetShowAllMenuItemsHeight(CDC* pDC, const CSize& sizeDefault)
{
	return(CanDrawImage() && m_ctrlMenuItemShowAll.IsValid()) ?
		m_ctrlMenuItemShowAll.GetParams().m_rectImage.Size().cy + 2 * AFX_TEXT_MARGIN : CMFCVisualManagerOffice2003::GetShowAllMenuItemsHeight(pDC, sizeDefault);
}

COLORREF VisualManager::OnFillMiniFrameCaption(CDC* pDC, CRect rectCaption, CPaneFrameWnd* pFrameWnd, BOOL bActive)
{
	if (!CanDrawImage())
	{
		return CMFCVisualManagerOffice2003::OnFillMiniFrameCaption(pDC, rectCaption, pFrameWnd, bActive);
	}

	ASSERT_VALID(pDC);
	ASSERT_VALID(pFrameWnd);

	BOOL bIsToolBar = FALSE;

	if (DYNAMIC_DOWNCAST(CMFCBaseToolBar, pFrameWnd->GetPane()) != NULL)
	{
		bActive = FALSE;
		bIsToolBar = TRUE;
	}

	pDC->FillRect(rectCaption, bActive ? &afxGlobalData.brActiveCaption : &afxGlobalData.brInactiveCaption);

	// get the text color
	return bActive ? afxGlobalData.clrCaptionText : afxGlobalData.clrInactiveCaptionText;
}

void VisualManager::OnDrawMiniFrameBorder(CDC* pDC, CPaneFrameWnd* pFrameWnd, CRect rectBorder, CRect rectBorderSize)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawMiniFrameBorder(pDC, pFrameWnd, rectBorder, rectBorderSize);
		return;
	}

	ASSERT_VALID(pDC);
	ASSERT_VALID(pFrameWnd);

	BOOL bIsTasksPane = pFrameWnd->IsKindOf( RUNTIME_CLASS( CMFCTasksPaneFrameWnd ) );

	if (bIsTasksPane)
	{
		CBrush* pOldBrush = pDC->SelectObject(&m_brFloatToolBarBorder);
		ENSURE(pOldBrush != NULL);

		pDC->PatBlt(rectBorder.left, rectBorder.top, rectBorderSize.left, rectBorder.Height(), PATCOPY);
		pDC->PatBlt(rectBorder.left, rectBorder.top, rectBorder.Width(), rectBorderSize.top, PATCOPY);
		pDC->PatBlt(rectBorder.right - rectBorderSize.right, rectBorder.top, rectBorderSize.right, rectBorder.Height(), PATCOPY);
		pDC->PatBlt(rectBorder.left, rectBorder.bottom - rectBorderSize.bottom, rectBorder.Width(), rectBorderSize.bottom, PATCOPY);

		rectBorderSize.DeflateRect(2, 2);
		rectBorder.DeflateRect(2, 2);

		pDC->SelectObject(&afxGlobalData.brBarFace);

		pDC->PatBlt(rectBorder.left, rectBorder.top + 1, rectBorderSize.left, rectBorder.Height() - 1, PATCOPY);
		pDC->PatBlt(rectBorder.left + 1, rectBorder.top, rectBorder.Width() - 2, rectBorderSize.top, PATCOPY);
		pDC->PatBlt(rectBorder.right - rectBorderSize.right, rectBorder.top + 1, rectBorderSize.right, rectBorder.Height() - 1, PATCOPY);
		pDC->PatBlt(rectBorder.left + 1, rectBorder.bottom - rectBorderSize.bottom, rectBorder.Width() - 2, rectBorderSize.bottom, PATCOPY);

		pDC->SelectObject(pOldBrush);
	}
	else
	{
		CMFCVisualManagerOffice2003::OnDrawMiniFrameBorder(pDC, pFrameWnd, rectBorder, rectBorderSize);
	}
}

void VisualManager::OnDrawFloatingToolbarBorder(CDC* pDC, CMFCBaseToolBar* pToolBar, CRect rectBorder, CRect rectBorderSize)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawFloatingToolbarBorder(pDC, pToolBar, rectBorder, rectBorderSize);
		return;
	}

	ASSERT_VALID(pDC);

	CBrush* pOldBrush = pDC->SelectObject(&m_brFloatToolBarBorder);
	ENSURE(pOldBrush != NULL);

	pDC->PatBlt(rectBorder.left, rectBorder.top, rectBorderSize.left, rectBorder.Height(), PATCOPY);
	pDC->PatBlt(rectBorder.left, rectBorder.top, rectBorder.Width(), rectBorderSize.top, PATCOPY);
	pDC->PatBlt(rectBorder.right - rectBorderSize.right, rectBorder.top, rectBorderSize.right, rectBorder.Height(), PATCOPY);
	pDC->PatBlt(rectBorder.left, rectBorder.bottom - rectBorderSize.bottom, rectBorder.Width(), rectBorderSize.bottom, PATCOPY);

	rectBorderSize.DeflateRect(2, 2);
	rectBorder.DeflateRect(2, 2);

	pDC->SelectObject(&afxGlobalData.brBarFace);

	pDC->PatBlt(rectBorder.left, rectBorder.top + 1, rectBorderSize.left, rectBorder.Height() - 1, PATCOPY);
	pDC->PatBlt(rectBorder.left + 1, rectBorder.top, rectBorder.Width() - 2, rectBorderSize.top, PATCOPY);
	pDC->PatBlt(rectBorder.right - rectBorderSize.right, rectBorder.top + 1, rectBorderSize.right, rectBorder.Height() - 1, PATCOPY);
	pDC->PatBlt(rectBorder.left + 1, rectBorder.bottom - rectBorderSize.bottom, rectBorder.Width() - 2, rectBorderSize.bottom, PATCOPY);

	pDC->SelectObject(pOldBrush);
}

BOOL VisualManager::IsOwnerDrawMenuCheck()
{
	return CanDrawImage() ? FALSE : CMFCVisualManagerOffice2003::IsOwnerDrawMenuCheck();
}

BOOL VisualManager::IsHighlightWholeMenuItem()
{
	return CanDrawImage() ? TRUE : CMFCVisualManagerOffice2003::IsHighlightWholeMenuItem();
}

COLORREF VisualManager::GetStatusBarPaneTextColor(CMFCStatusBar* pStatusBar, CMFCStatusBarPaneInfo* pPane)
{
	if (!CanDrawImage())
	{
		return CMFCVisualManagerOffice2003::GetStatusBarPaneTextColor(pStatusBar, pPane);
	}

	ENSURE(pPane != NULL);

	return(pPane->nStyle & SBPS_DISABLED) ? m_clrStatusBarTextDisabled : pPane->clrText == (COLORREF)-1 ? m_clrStatusBarText : pPane->clrText;
}

COLORREF VisualManager::GetToolbarButtonTextColor(CMFCToolBarButton* pButton, CMFCVisualManager::AFX_BUTTON_STATE state)
{
	if (!CanDrawImage())
	{
		return CMFCVisualManagerOffice2003::GetToolbarButtonTextColor(pButton, state);
	}

	ASSERT_VALID(pButton);

	BOOL bDisabled = (CMFCToolBar::IsCustomizeMode() && !pButton->IsEditable()) || (!CMFCToolBar::IsCustomizeMode() &&(pButton->m_nStyle & TBBS_DISABLED));

	if (pButton->GetParentWnd() != NULL && pButton->GetParentWnd()->IsKindOf(RUNTIME_CLASS(CMFCMenuBar)))
	{
		if (CMFCToolBar::IsCustomizeMode())
		{
			return m_clrMenuBarBtnText;
		}

		return bDisabled ? m_clrMenuBarBtnTextDisabled :
			((state == ButtonsIsHighlighted || state == ButtonsIsPressed || pButton->IsDroppedDown()) ? m_clrMenuBarBtnTextHighlighted : m_clrMenuBarBtnText);
	}

	return bDisabled ? m_clrToolBarBtnTextDisabled :
		((state == ButtonsIsHighlighted || state == ButtonsIsPressed) ? m_clrToolBarBtnTextHighlighted : m_clrToolBarBtnText);
}

COLORREF VisualManager::GetMenuItemTextColor(CMFCToolBarMenuButton* pButton, BOOL bHighlighted, BOOL bDisabled)
{
	if (!CanDrawImage())
	{
		return CMFCVisualManagerOffice2003::GetMenuItemTextColor(pButton, bHighlighted, bDisabled);
	}

	return bDisabled ? m_clrMenuTextDisabled : m_clrMenuText;
}

COLORREF VisualManager::GetHighlightedMenuItemTextColor(CMFCToolBarMenuButton* pButton)
{
	if (!CanDrawImage())
	{
		return CMFCVisualManagerOffice2003::GetHighlightedMenuItemTextColor(pButton);
	}

	return m_clrMenuTextHighlighted;
}

void VisualManager::GetTabFrameColors(const CMFCBaseTabCtrl* pTabWnd, COLORREF& clrDark, COLORREF& clrBlack, COLORREF& clrHighlight,
	COLORREF& clrFace, COLORREF& clrDarkShadow, COLORREF& clrLight, CBrush*& pbrFace, CBrush*& pbrBlack)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::GetTabFrameColors(pTabWnd, clrDark, clrBlack, clrHighlight, clrFace, clrDarkShadow, clrLight, pbrFace, pbrBlack);
		return;
	}

	ASSERT_VALID(pTabWnd);

	CMFCVisualManagerOffice2003::GetTabFrameColors(pTabWnd, clrDark, clrBlack, clrHighlight, clrFace, clrDarkShadow, clrLight, pbrFace, pbrBlack);

	if (pTabWnd->IsFlatTab() /*&& !pTabWnd->IsDialogControl()*/)
	{
		if (m_clrTabFlatBlack != CLR_DEFAULT)
		{
			clrBlack = m_clrTabFlatBlack;
		}

		if (m_clrTabFlatHighlight != CLR_DEFAULT)
		{
			clrHighlight = m_clrTabFlatHighlight;
		}
	}
}

void VisualManager::OnEraseTabsArea(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd)
{
	ASSERT_VALID(pDC);
	ASSERT_VALID(pTabWnd);

	if (!CanDrawImage()/* || pTabWnd->IsDialogControl()*/)
	{
		CMFCVisualManagerOffice2003::OnEraseTabsArea(pDC, rect, pTabWnd);
		return;
	}

	if (pTabWnd->IsOneNoteStyle() || pTabWnd->IsColored() || pTabWnd->IsVS2005Style() || pTabWnd->IsLeftRightRounded())
	{
		CMFCVisualManagerOffice2003::OnEraseTabsArea(pDC, rect, pTabWnd);
		return;
	}

	const BOOL bBottom = pTabWnd->GetLocation() == CMFCTabCtrl::LOCATION_BOTTOM;

	if (pTabWnd->IsFlatTab())
	{
		m_ctrlTabFlat[bBottom ? 1 : 0].Draw(pDC, rect);
	}
	else
	{
		CDrawingManager dm(*pDC);

		COLORREF clr1 = m_clrBarGradientDark;
		COLORREF clr2 = m_clrBarGradientLight;

		if (bBottom)
		{
			dm.FillGradient(rect, clr1, clr2, TRUE);
		}
		else
		{
			dm.FillGradient(rect, clr2, clr1, TRUE);
		}
	}
}

void VisualManager::OnDrawTab(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd)
{
	ASSERT_VALID(pTabWnd);
	ASSERT_VALID(pDC);

	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawTab(pDC, rectTab, iTab, bIsActive, pTabWnd);
		return;
	}

	if (pTabWnd->IsOneNoteStyle() || pTabWnd->IsColored() || pTabWnd->IsVS2005Style() || pTabWnd->IsLeftRightRounded())
	{
		CMFCVisualManagerOffice2003::OnDrawTab(pDC, rectTab, iTab, bIsActive, pTabWnd);
		return;
	}

	const BOOL bBottom = pTabWnd->GetLocation() == CMFCTabCtrl::LOCATION_BOTTOM;
	COverlayTabCtrl *pOverlayedTab = DYNAMIC_DOWNCAST(COverlayTabCtrl, pTabWnd);
	const BOOL bIsHighlight = iTab == pTabWnd->GetHighlightedTab();
	COLORREF clrText = m_clrTabTextActive;

	// Customized
	if (bIsHighlight && bIsActive == false)
		clrText = m_clrTabTextHighlight;
	else if (!bIsActive)
		clrText = m_clrTabTextInactive;

	if (pTabWnd->IsFlatTab())
	{
		int nImage = (bIsActive || bIsHighlight) ? 2 : 1;

		CRgn rgn;

		POINT pts[4];

		if (bBottom)
		{
			rectTab.bottom++;

			pts[0].x = rectTab.left;
			pts[0].y = rectTab.bottom + 1;
			pts[1].x = rectTab.left;
			pts[1].y = rectTab.top;
			pts[2].x = rectTab.right + 1;
			pts[2].y = rectTab.top;
			pts[3].x = rectTab.right - rectTab.Height() + 1;
			pts[3].y = rectTab.bottom + 1;

			rectTab.top++;
		}
		else
		{
			pts[0].x = rectTab.left;
			pts[0].y = rectTab.bottom + 1;
			pts[1].x = rectTab.left;
			pts[1].y = rectTab.top;
			pts[2].x = rectTab.right - rectTab.Height() + 1;
			pts[2].y = rectTab.top;
			pts[3].x = rectTab.right + 1;
			pts[3].y = rectTab.bottom + 1;
		}

		rgn.CreatePolygonRgn(pts, 4, WINDING);

		int isave = pDC->SaveDC();

		pDC->SelectClipRgn(&rgn, RGN_AND);

		m_ctrlTabFlat[bBottom ? 1 : 0].Draw(pDC, rectTab, nImage);

		CPen* pOldPen = pDC->SelectObject(&m_penTabFlatOuter[bIsActive ? 1 : 0]);

		if (bBottom)
		{
			pDC->MoveTo(pts[2].x, pts[2].y);
			pDC->LineTo(pts[3].x, pts[3].y - 1);
		}
		else
		{
			pDC->MoveTo(pts[2].x - 1, pts[2].y);
			pDC->LineTo(pts[3].x - 1, pts[3].y - 1);
		}

		pDC->SelectObject(&m_penTabFlatInner[bIsActive ? 1 : 0]);

		if (bBottom)
		{
			pDC->MoveTo(pts[2].x - 2, pts[2].y + 1);
			pDC->LineTo(pts[3].x, pts[3].y - 2);
		}
		else
		{
			pDC->MoveTo(pts[2].x - 1, pts[2].y + 1);
			pDC->LineTo(pts[3].x - 2, pts[3].y - 1);
		}

		pDC->SelectObject(pOldPen);

		pDC->SelectClipRgn(NULL);

		clrText = afxGlobalData.clrBarText;
		pDC->RestoreDC(isave);
	}
	else
	{
		if (clrText == (COLORREF)-1)
		{
			clrText = bIsActive ? m_clrMenuBarBtnTextHighlighted : m_clrMenuBarBtnText;
		}

		int nImage = bIsActive ? 3 : 0;
		if (bIsHighlight)
		{
			nImage += 1;
		}

		m_ctrlTab3D[bBottom ? 1 : 0].Draw(pDC, rectTab, nImage);
	}

	// if the tab control supports overlay
	if (pOverlayedTab != NULL)
	{
		// give it a chance to alter the text color
		pOverlayedTab->TextColorModifier(clrText, iTab, bIsActive, bIsHighlight);
	}

	OnDrawTabContent(pDC, rectTab, iTab, bIsActive, pTabWnd, clrText);

	// if the tab control supports overlay
	if (pOverlayedTab != NULL)
	{
		// draw the overlay after everything was painted
		pOverlayedTab->DrawOverlay(pDC, rectTab, iTab, bIsActive, bIsHighlight);
	}
}

// copied from C:\Program Files\Microsoft Visual Studio 10.0\VC\atlmfc\include\afxvisualmanager.h
void VisualManager::OnDrawTabContent(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd, COLORREF clrText)
{
	ASSERT_VALID(pTabWnd);
	ASSERT_VALID(pDC);

	if (pTabWnd->IsActiveTabCloseButton() && bIsActive)
	{
		CRect rectClose = pTabWnd->GetTabCloseButton();
		rectTab.right = rectClose.left;

		OnDrawTabCloseButton(pDC, rectClose, pTabWnd, pTabWnd->IsTabCloseButtonHighlighted(), pTabWnd->IsTabCloseButtonPressed(), FALSE /* Disabled */);
	}

	CString strText;
	pTabWnd->GetTabLabel(iTab, strText);

	if (pTabWnd->IsFlatTab())
	{
		//---------------
		// Draw tab text:
		//---------------
		UINT nFormat = DT_SINGLELINE | DT_CENTER | DT_VCENTER;
		if (pTabWnd->IsDrawNoPrefix())
		{
			nFormat |= DT_NOPREFIX;
		}

		pDC->DrawText(strText, rectTab, nFormat);
	}
	else
	{
		CSize sizeImage = pTabWnd->GetImageSize();
		UINT uiIcon = pTabWnd->GetTabIcon(iTab);
		HICON hIcon = pTabWnd->GetTabHicon(iTab);

		if (uiIcon == (UINT)-1 && hIcon == NULL)
		{
			sizeImage.cx = 0;
		}

#ifndef _AFXDLL
		if (sizeImage.cx + 2 * CMFCBaseTabCtrl::AFX_TAB_IMAGE_MARGIN <= rectTab.Width())
#else
		if (sizeImage.cx + 2 * 4 <= rectTab.Width())
#endif // _AFXDLL
		{
			if (hIcon != NULL)
			{
				//---------------------
				// Draw the tab's icon:
				//---------------------
				CRect rectImage = rectTab;

				rectImage.top += (rectTab.Height() - sizeImage.cy) / 2;
				rectImage.bottom = rectImage.top + sizeImage.cy;

				rectImage.left += AFX_IMAGE_MARGIN;
				rectImage.right = rectImage.left + sizeImage.cx;

				pDC->DrawState(rectImage.TopLeft(), rectImage.Size(), hIcon, DSS_NORMAL, (HBRUSH) NULL);
			}
			else
			{
				const CImageList* pImageList = pTabWnd->GetImageList();
				if (pImageList != NULL && uiIcon != (UINT)-1)
				{
					//----------------------
					// Draw the tab's image:
					//----------------------
					UINT IconCount = pImageList->GetImageCount();
					CRect rectImage = rectTab;

					rectImage.top += (rectTab.Height() - sizeImage.cy) / 2;
					rectImage.bottom = rectImage.top + sizeImage.cy;

					rectImage.left += AFX_IMAGE_MARGIN;
					rectImage.right = rectImage.left + sizeImage.cx;

					// the image list contains the normal and active images
					// if the tab is active, shift the icon index by 
					// half the number of images to obtain the active icon index
					ASSERT(IconCount >= pTabWnd->GetTabsNum() * 2U);
					// shift icon index
					if (bIsActive)
						uiIcon += (UINT)(IconCount / 2);
					// make sure the icon index doesn't overflow
					ASSERT(uiIcon < IconCount);

					ASSERT_VALID(pImageList);
					const_cast<CImageList*>(pImageList)->Draw(pDC, uiIcon, rectImage.TopLeft(), ILD_TRANSPARENT);
				}
			}

			//------------------------------
			// Finally, draw the tab's text:
			//------------------------------
			CRect rcText = rectTab;
			rcText.left += sizeImage.cx + 2 * AFX_TEXT_MARGIN;

			if (rcText.Width() < sizeImage.cx * 2 && !pTabWnd->IsLeftRightRounded())
			{
				rcText.right -= AFX_TEXT_MARGIN;
			}

			if (clrText == (COLORREF)-1)
			{
				clrText = GetTabTextColor(pTabWnd, iTab, bIsActive);
			}

			if (clrText != (COLORREF)-1)
			{
				pDC->SetTextColor(clrText);
			}

			UINT nFormat = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;
			if (pTabWnd->IsDrawNoPrefix())
			{
				nFormat |= DT_NOPREFIX;
			}

			if (pTabWnd->IsOneNoteStyle() || pTabWnd->IsVS2005Style())
			{
				nFormat |= DT_CENTER;
			}
			else
			{
				nFormat |= DT_LEFT;
			}

			// trim the spaces used to pad the tab width
			strText.Trim();
			pDC->DrawText(strText, rcText, nFormat);
		}
	}
}

void VisualManager::OnFillTab(CDC* pDC, CRect rectFill, CBrush* pbrFill, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd)
{
	ASSERT_VALID(pTabWnd);

	if (!CanDrawImage()/* || pTabWnd->IsDialogControl()*/)
	{
		CMFCVisualManagerOffice2003::OnFillTab(pDC, rectFill, pbrFill, iTab, bIsActive, pTabWnd);
		return;
	}

	if (pTabWnd->IsFlatTab() || pTabWnd->IsOneNoteStyle() || pTabWnd->IsColored() || pTabWnd->IsVS2005Style() || pTabWnd->IsLeftRightRounded())
	{
		CMFCVisualManagerOffice2003::OnFillTab(pDC, rectFill, pbrFill, iTab, bIsActive, pTabWnd);
		return;
	}

	ASSERT_VALID(pDC);

	const BOOL bBottom = pTabWnd->GetLocation() == CMFCTabCtrl::LOCATION_BOTTOM;
	const BOOL bIsHighlight = iTab == pTabWnd->GetHighlightedTab();

	if (bIsActive || bIsHighlight)
	{
		int nImage = bIsActive ?2:-1;

		if (bIsHighlight)
		{
			nImage += 1;
		}

		m_ctrlTab3D[bBottom ? 1 : 0].Draw(pDC, rectFill, nImage);
	}
}

COLORREF VisualManager::GetTabTextColor(const CMFCBaseTabCtrl* pTabWnd, int iTab, BOOL bIsActive)
{
	if (!CanDrawImage()/* || pTabWnd->IsDialogControl()*/)
	{
		return CMFCVisualManagerOffice2003::GetTabTextColor(pTabWnd, iTab, bIsActive);
	}

	ASSERT_VALID(pTabWnd);

	if (pTabWnd->IsOneNoteStyle() || pTabWnd->GetTabBkColor(iTab) != (COLORREF)-1)
	{
		return CMFCVisualManagerOffice2003::GetTabTextColor(pTabWnd, iTab, bIsActive);
	}

	return bIsActive ? m_clrTabTextActive : m_clrTabTextInactive;
}

int VisualManager::GetTabHorzMargin(const CMFCBaseTabCtrl* pTabWnd)
{
	if (!CanDrawImage())
	{
		return CMFCVisualManagerOffice2003::GetTabHorzMargin(pTabWnd);
	}

	CMFCControlRenderer* pRenderer = pTabWnd->IsFlatTab() ? &m_ctrlTabFlat[0] : &m_ctrlTab3D[0];

	if (pTabWnd->IsOneNoteStyle() || pTabWnd->IsColored() || pTabWnd->IsVS2005Style() || pTabWnd->IsLeftRightRounded() || !pRenderer->IsValid())
	{
		return CMFCVisualManagerOffice2003::GetTabHorzMargin(pTabWnd);
	}

	return pRenderer->GetParams().m_rectSides.right / 2;
}

BOOL VisualManager::OnEraseTabsFrame(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd)
{
	ASSERT_VALID(pDC);
	ASSERT_VALID(pTabWnd);

	if (!CanDrawImage()/* || pTabWnd->IsDialogControl()*/)
	{
		return CMFCVisualManagerOffice2003::OnEraseTabsFrame(pDC, rect, pTabWnd);
	}

	if (pTabWnd->IsOneNoteStyle() || pTabWnd->IsColored() || pTabWnd->IsVS2005Style() || pTabWnd->IsLeftRightRounded())
	{
		return CMFCVisualManagerOffice2003::OnEraseTabsFrame(pDC, rect, pTabWnd);
	}

	if (pTabWnd->IsFlatTab())
	{
		pDC->FillRect(rect, &afxGlobalData.brWindow);

		if (pTabWnd->GetLocation() != CMFCTabCtrl::LOCATION_BOTTOM)
		{
			CPen pen(PS_SOLID, 1, m_clrTabFlatBlack);
			CPen* pOldPen = pDC->SelectObject(&pen);

			pDC->MoveTo(rect.left, rect.top + pTabWnd->GetTabsHeight() + 1);
			pDC->LineTo(rect.right, rect.top + pTabWnd->GetTabsHeight() + 1);

			pDC->SelectObject(pOldPen);
		}

		return TRUE;
	}

	return FALSE;
}

void VisualManager::OnEraseTabsButton(CDC* pDC, CRect rect, CMFCButton* pButton, CMFCBaseTabCtrl* pBaseTab)
{
	ASSERT_VALID(pDC);
	ASSERT_VALID(pButton);
	ASSERT_VALID(pBaseTab);

	CMFCTabCtrl* pWndTab = DYNAMIC_DOWNCAST(CMFCTabCtrl, pBaseTab);

	if (!CanDrawImage() || pWndTab == NULL/* || pBaseTab->IsDialogControl()*/)
	{
		CMFCVisualManagerOffice2003::OnEraseTabsButton(pDC, rect, pButton, pBaseTab);
		return;
	}

	if (pBaseTab->IsFlatTab() || pBaseTab->IsOneNoteStyle() || pBaseTab->IsColored() ||
		pBaseTab->IsVS2005Style() || pBaseTab->IsLeftRightRounded() || (!pButton->IsPressed() && !pButton->IsHighlighted()))
	{
		CMFCVisualManagerOffice2003::OnEraseTabsButton(pDC, rect, pButton, pBaseTab);
		return;
	}

	CRgn rgn;
	rgn.CreateRectRgnIndirect(rect);

	pDC->SelectClipRgn(&rgn);

	CRect rectTabs;
	pWndTab->GetClientRect(&rectTabs);

	CRect rectTabArea;
	pWndTab->GetTabsRect(rectTabArea);

	if (pWndTab->GetLocation() == CMFCBaseTabCtrl::LOCATION_BOTTOM)
	{
		rectTabs.top = rectTabArea.top;
	}
	else
	{
		rectTabs.bottom = rectTabArea.bottom;
	}

	pWndTab->MapWindowPoints(pButton, rectTabs);
	OnEraseTabsArea(pDC, rectTabs, pWndTab);

	pDC->SelectClipRgn(NULL);

	int index = pButton->IsPressed() ? 2 : 1;
	m_ctrlToolBarBtn.Draw(pDC, rect, index);
}

void VisualManager::OnDrawTabsButtonBorder(CDC* pDC, CRect& rect, CMFCButton* pButton, UINT uiState, CMFCBaseTabCtrl* pWndTab)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawTabsButtonBorder(pDC, rect, pButton, uiState, pWndTab);
	}
}

void VisualManager::OnDrawTasksGroupCaption(CDC* pDC, CMFCTasksPaneTaskGroup* pGroup, BOOL bIsHighlighted /*= FALSE*/, BOOL bIsSelected /*= FALSE*/, BOOL bCanCollapse /*= FALSE*/)
{
	ASSERT_VALID(pDC);
	ASSERT_VALID(pGroup);
	ASSERT_VALID(pGroup->m_pPage);

	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawTasksGroupCaption(pDC, pGroup, bIsHighlighted, bIsSelected, bCanCollapse);
		return;
	}

	CRect rectGroup = pGroup->m_rect;

	// -----------------------
	// Draw caption background
	// -----------------------
	CDrawingManager dm(*pDC);

	if (pGroup->m_bIsSpecial)
	{
		dm.FillGradient(pGroup->m_rect, bIsHighlighted ? m_clrTaskPaneGroupCaptionHighSpecLight : m_clrTaskPaneGroupCaptionSpecLight,
			bIsHighlighted ? m_clrTaskPaneGroupCaptionHighSpecDark : m_clrTaskPaneGroupCaptionSpecDark, TRUE);
	}
	else
	{
		dm.FillGradient(pGroup->m_rect, bIsHighlighted ? m_clrTaskPaneGroupCaptionHighLight : m_clrTaskPaneGroupCaptionLight,
			bIsHighlighted ? m_clrTaskPaneGroupCaptionHighDark : m_clrTaskPaneGroupCaptionDark, TRUE);
	}

	// ---------------------------
	// Draw an icon if it presents
	// ---------------------------
	BOOL bShowIcon = (pGroup->m_hIcon != NULL && pGroup->m_sizeIcon.cx < rectGroup.Width() - rectGroup.Height());
	if (bShowIcon)
	{
		OnDrawTasksGroupIcon(pDC, pGroup, 5, bIsHighlighted, bIsSelected, bCanCollapse);
	}

	// -----------------------
	// Draw group caption text
	// -----------------------
	CFont* pFontOld = pDC->SelectObject(&afxGlobalData.fontBold);
	COLORREF clrTextOld = pDC->GetTextColor();

	if (bCanCollapse && bIsHighlighted)
	{
		pDC->SetTextColor(pGroup->m_clrTextHot == (COLORREF)-1 ?
			(pGroup->m_bIsSpecial ? m_clrTaskPaneGroupCaptionTextHighSpec : m_clrTaskPaneGroupCaptionTextHigh) : pGroup->m_clrTextHot);
	}
	else
	{
		pDC->SetTextColor(pGroup->m_clrText == (COLORREF)-1 ?
			(pGroup->m_bIsSpecial ? m_clrTaskPaneGroupCaptionTextSpec : m_clrTaskPaneGroupCaptionText) : pGroup->m_clrText);
	}

	int nBkModeOld = pDC->SetBkMode(TRANSPARENT);

	int nTaskPaneHOffset = pGroup->m_pPage->m_pTaskPane->GetGroupCaptionHorzOffset();
	int nTaskPaneVOffset = pGroup->m_pPage->m_pTaskPane->GetGroupCaptionVertOffset();
	int nCaptionHOffset = (nTaskPaneHOffset != -1 ? nTaskPaneHOffset : m_nGroupCaptionHorzOffset);

	CRect rectText = rectGroup;
	rectText.left += (bShowIcon ? pGroup->m_sizeIcon.cx + 5: nCaptionHOffset);
	rectText.top += (nTaskPaneVOffset != -1 ? nTaskPaneVOffset : m_nGroupCaptionVertOffset);
	rectText.right = max(rectText.left, rectText.right -(bCanCollapse ? rectGroup.Height() : nCaptionHOffset));

	pDC->DrawText(pGroup->m_strName, rectText, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

	pDC->SetBkMode(nBkModeOld);
	pDC->SelectObject(pFontOld);
	pDC->SetTextColor(clrTextOld);

	// -------------------------
	// Draw group caption button
	// -------------------------
	if (bCanCollapse && !pGroup->m_strName.IsEmpty())
	{
		CSize sizeButton = CMenuImages::Size();
		CRect rectButton = rectGroup;
		rectButton.left = max(rectButton.left, rectButton.right -(rectButton.Height() + 1) / 2 -(sizeButton.cx + 1) / 2);
		rectButton.top = max(rectButton.top, rectButton.bottom -(rectButton.Height() + 1) / 2 -(sizeButton.cy + 1) / 2);
		rectButton.right = rectButton.left + sizeButton.cx;
		rectButton.bottom = rectButton.top + sizeButton.cy;

		if (rectButton.right <= rectGroup.right && rectButton.bottom <= rectGroup.bottom)
		{
			if (bIsHighlighted)
			{
				// Draw button frame
				CBrush* pBrushOld = (CBrush*) pDC->SelectObject(&afxGlobalData.brBarFace);
				COLORREF clrBckOld = pDC->GetBkColor();

				pDC->Draw3dRect(&rectButton, afxGlobalData.clrWindow, afxGlobalData.clrBarShadow);

				pDC->SetBkColor(clrBckOld);
				pDC->SelectObject(pBrushOld);
			}

			CMenuImages::Draw(pDC, pGroup->m_bIsCollapsed ? CMenuImages::IdArrowDown : CMenuImages::IdArrowUp, rectButton.TopLeft(), CMenuImages::ImageBlack);
		}
	}
}

void VisualManager::OnDrawTask(CDC* pDC, CMFCTasksPaneTask* pTask, CImageList* pIcons, BOOL bIsHighlighted /*= FALSE*/, BOOL bIsSelected /*= FALSE*/)
{
	ASSERT_VALID(pTask);

	if (CanDrawImage() && pTask->m_bIsSeparator)
	{
		DrawSeparator(pDC, pTask->m_rect, TRUE);
		return;
	}

	CMFCVisualManagerOffice2003::OnDrawTask(pDC, pTask, pIcons, bIsHighlighted, bIsSelected);
}

void VisualManager::OnDrawScrollButtons(CDC* pDC, const CRect& rect, const int nBorderSize, int iImage, BOOL bHilited)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawScrollButtons(pDC, rect, nBorderSize, iImage, bHilited);
		return;
	}

	CRect rt(rect);
	rt.top--;
	m_ctrlTaskScrollBtn.Draw(pDC, rt, bHilited ? 1 : 0);

	CMenuImages::Draw(pDC, (CMenuImages::IMAGES_IDS) iImage, rect);
}

void VisualManager::OnDrawHeaderCtrlBorder(CMFCHeaderCtrl* pCtrl, CDC* pDC, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawHeaderCtrlBorder(pCtrl, pDC, rect, bIsPressed, bIsHighlighted);
		return;
	}

	COLORREF clrStart  = m_clrHeaderNormalStart;
	COLORREF clrFinish = m_clrHeaderNormalFinish;
	COLORREF clrBorder = m_clrHeaderNormalBorder;

	if (bIsPressed)
	{
		clrStart  = m_clrHeaderPressedStart;
		clrFinish = m_clrHeaderPressedFinish;
		clrBorder = m_clrHeaderPressedBorder;
	}
	else if (bIsHighlighted)
	{
		clrStart  = m_clrHeaderHighlightedStart;
		clrFinish = m_clrHeaderHighlightedFinish;
		clrBorder = m_clrHeaderHighlightedBorder;
	}

	{
		CDrawingManager dm(*pDC);
		dm.FillGradient(rect, clrFinish, clrStart);
	}

	CPen pen(PS_SOLID, 0, clrBorder);
	CPen* pOldPen = pDC->SelectObject(&pen);

	if (bIsPressed || bIsHighlighted)
	{
		pDC->MoveTo(rect.right - 1, rect.top);
		pDC->LineTo(rect.right - 1, rect.bottom - 1);
		pDC->LineTo(rect.left, rect.bottom - 1);
		pDC->LineTo(rect.left, rect.top - 1);
	}
	else
	{
		pDC->MoveTo(rect.right - 1, rect.top);
		pDC->LineTo(rect.right - 1, rect.bottom - 1);
		pDC->LineTo(rect.left - 1, rect.bottom - 1);
	}

	pDC->SelectObject(pOldPen);
}

/*!	\brief Called by VisualManager::OnDrawCheckBoxEx()
	\param[in,out] pDC ???
	\param[in] rect ???
	\param[in] nStyle ???
	\param[in] bHighlighted ???
	\param[in] bPressed ???
	\param[in] bEnabled ???

	Handle the basic draw but do not handle the ribbon draw
 */
void VisualManager::DrawCheckBoxEx(CDC *pDC, CRect rect, int nState, BOOL bHighlighted, BOOL bPressed, BOOL bEnabled)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawCheckBoxEx(pDC, rect, nState, bHighlighted, bPressed, bEnabled);
		return;
	}

	int index = nState * 4;

	if (!bEnabled)
	{
		index += 3;
	}
	else if (bPressed)
	{
		if (bHighlighted)
		{
			index += 2;
		}
	}
	else if (bHighlighted)
	{
		index += 1;
	}
}

void VisualManager::OnDrawCheckBoxEx(CDC *pDC, CRect rect, int nState, BOOL bHighlighted, BOOL bPressed, BOOL bEnabled)
{
	DrawCheckBoxEx(pDC, rect, nState, bHighlighted, bPressed, bEnabled);
}

CSize VisualManager::GetSystemBorders(void) const
{
	CSize size(::GetSystemMetrics(SM_CYSIZEFRAME), ::GetSystemMetrics(SM_CXSIZEFRAME));

	return size;
}

BOOL VisualManager::OnEraseMDIClientArea(CDC* pDC, CRect rectClient)
{
	if (!CanDrawImage() || m_brMainClientArea.GetSafeHandle() == NULL)
	{
		return CMFCVisualManagerOffice2003::OnEraseMDIClientArea(pDC, rectClient);
	}

	pDC->FillRect(rectClient, &m_brMainClientArea);
	return TRUE;
}

BOOL VisualManager::GetToolTipInfo(CMFCToolTipInfo& params, UINT /*nType*/ /*= (UINT)(-1)*/)
{
	if (!CanDrawImage() || !m_bToolTipParams)
	{
		return CMFCVisualManagerOffice2003::GetToolTipInfo(params);
	}

	params = m_ToolTipParams;
	return TRUE;
}

void VisualManager::OnFillPopupWindowBackground(CDC* pDC, CRect rect)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnFillPopupWindowBackground(pDC, rect);
		return;
	}

	ASSERT_VALID(pDC);

	CDrawingManager dm(*pDC);
	dm.FillGradient(rect, m_clrPopupGradientDark, m_clrPopupGradientLight);
}

COLORREF VisualManager::OnDrawPopupWindowCaption(CDC* pDC, CRect rectCaption, CMFCDesktopAlertWnd* pPopupWnd)
{
	COLORREF clrText = CMFCVisualManagerOffice2003::OnDrawPopupWindowCaption(pDC, rectCaption, pPopupWnd);

	if (CanDrawImage())
	{
		clrText = m_clrCaptionBarText;
	}

	return clrText;
}

COLORREF VisualManager::OnDrawMenuLabel(CDC* pDC, CRect rect)
{
	ASSERT_VALID(pDC);

	pDC->FillRect(rect, m_brGroupBackground.GetSafeHandle() != NULL ? &m_brGroupBackground : &m_brBarBkgnd);

	CRect rectSeparator = rect;
	rectSeparator.top = rectSeparator.bottom - 2;

	DrawSeparator(pDC, rectSeparator, TRUE);

	return m_clrGroupText != (COLORREF)-1 ? m_clrGroupText : m_clrMenuText;
}

COLORREF VisualManager::OnFillCaptionBarButton(CDC* pDC, CMFCCaptionBar* pBar, CRect rect,
	BOOL bIsPressed, BOOL bIsHighlighted, BOOL bIsDisabled, BOOL bHasDropDownArrow, BOOL bIsSysButton)
{
	COLORREF clrText = CMFCVisualManagerOffice2003::OnFillCaptionBarButton(pDC, pBar, rect, bIsPressed, bIsHighlighted, bIsDisabled, bHasDropDownArrow, bIsSysButton);

	ASSERT_VALID(pBar);

	if (CanDrawImage() && pBar->IsMessageBarMode() && bIsSysButton && !bIsHighlighted)
	{
		clrText = m_clrMenuBarBtnText;
	}

	return clrText;
}

AFX_SMARTDOCK_THEME VisualManager::GetSmartDockingTheme()
{
	if (afxGlobalData.m_nBitsPerPixel <= 8 || afxGlobalData.IsHighContrastMode() || !afxGlobalData.IsWindowsLayerSupportAvailable() || !afxGlobalData.bIsWindowsVista)
	{
		return CMFCVisualManagerOffice2003::GetSmartDockingTheme();
	}

	return AFX_SDT_VS2008;
}

BOOL VisualManager::DrawPushButton(CDC* pDC, CRect rectClient, CMFCButton* pButton, UINT uiState)
{
	// redraw the background of the parent corresponding to the button area
	pButton->OnDrawParentBackground(pDC, rectClient);

	if (pButton->m_bTransparent)
		return TRUE;

	VisualDialog *pParent = DYNAMIC_DOWNCAST(VisualDialog, pButton->GetParent());
	bool IsDefault = (pParent != NULL && pParent->IsDefaultButton(pButton->GetDlgCtrlID()));
	bool IsPressed = (pButton->IsPressed() || pButton->IsPushed());
	bool IsHighlighted = (pButton->IsHighlighted() != FALSE);

	DrawButtonFrame(pDC, rectClient, IsDefault, IsPressed, IsHighlighted);

	return TRUE;
}

/*! \brief Computes a gray color from the average of r,g,b values
	\param[in] Color_in : the color used to compute the gray color
	\return a gray color from the average of r,g,b values
*/
COLORREF VisualManager::GrayAverage(COLORREF Color_in)
{
	BYTE Red = (BYTE)(Color_in);
	BYTE Green = (BYTE)(Color_in >> 8);
	BYTE Blue = (BYTE)(Color_in >> 16);
	BYTE Average = (Red + Green + Blue) / 3;

	return RGB(Average, Average, Average);
}

/*! \brief Computes the average color from two colors
	\param[in] Color1_in : the color used to compute the gray color
	\param[in] Color1_in : the color used to compute the gray color
	\return the average color
*/
COLORREF VisualManager::ColorAverage(COLORREF Color1_in, COLORREF Color2_in)
{
	BYTE Red = (BYTE)(Color1_in);
	BYTE Green = (BYTE)(Color1_in >> 8);
	BYTE Blue = (BYTE)(Color1_in >> 16);
	BYTE Red2 = (BYTE)(Color2_in);
	BYTE Green2 = (BYTE)(Color2_in >> 8);
	BYTE Blue2 = (BYTE)(Color2_in >> 16);

	return RGB((Red + Red2) / 2, (Green + Green2) / 2, (Blue + Blue2) / 2);
}

void VisualManager::DrawButtonFrame(CDC* pDC_in, const CRect &Rect_in, bool IsDefault_in,
								   bool IsPressed_in, bool IsHighlighted_in, bool IsDisabled_in,
								   bool IsChecked_in, bool IsFlat_in, bool NoBorder_in) const
{
	CRect RegionRect(Rect_in);
	CDrawingManager dm(*pDC_in);
	COLORREF BorderPenColor;
	HRGN hRegion;
	
	// draw the border -- black for the default button
	if (IsDefault_in || IsPressed_in)
		BorderPenColor = m_clrDefaultButtonBorder;
	else
		BorderPenColor = m_clrButtonBorder;

	// normal buttons
	if (NoBorder_in == false)
	{
		CPen pen(PS_SOLID, 1, BorderPenColor), *pOldPen;

		// draw the border
		pOldPen = pDC_in->SelectObject(&pen);
		pDC_in->RoundRect(Rect_in, CPoint(5, 6));
		pOldPen = pDC_in->SelectObject(pOldPen);
		DeleteObject(pOldPen);
	}

	// create a rounded region for clipping in order to draw the gradient
	RegionRect.DeflateRect(1, 1, 0, 0);
	hRegion = CreateRoundRectRgn(RegionRect.left, RegionRect.top,
								 RegionRect.right, RegionRect.bottom, 4, 3);
	// start clipping
	::SelectClipRgn(pDC_in->GetSafeHdc(), hRegion);	

	if (IsDisabled_in)
	{
		CBrush Color(m_clrDisabledButton);

		pDC_in->FillRect(Rect_in, &Color);
	}
	else if (IsPressed_in || IsChecked_in)
	{
		if (IsChecked_in && IsPressed_in)
		{
			CBrush Color(m_clrPressedButton);

			pDC_in->FillRect(Rect_in, &Color);
		}
		else if (IsFlat_in)
		{
			dm.DrawRect(Rect_in, m_clrButtonGradientLight, m_clrButtonBorder);
		}
		else
		{
			CBrush Color(m_clrButtonGradientDark);

			pDC_in->FillRect(Rect_in, &Color);
		}
	}
	else if (IsFlat_in)
	{
		CBrush Color(m_clrButtonGradientDark);

		pDC_in->FillRect(Rect_in, &Color);
	}
	else
		dm.FillGradient(Rect_in, m_clrButtonGradientDark, m_clrButtonGradientLight);

	if (IsHighlighted_in && IsPressed_in == false && IsDisabled_in == false)
	{
		// highlight the button when the mouse is hovering it
		dm.HighlightRect(Rect_in);
	}

	// Cleanup
	DeleteObject(hRegion);
}

void VisualManager::DrawButtonText(CDC* pDC_in, const CString &Text_in, COLORREF Color_in,
								  const CRect &Rect_in, int AlignStyle_in, bool IsDisabled_in)
{
	CRect TextRect(Rect_in);

	pDC_in->SetBkMode(TRANSPARENT);
	
	UINT uiDTFlags = DT_END_ELLIPSIS;
	BOOL bIsSingleLine = FALSE;

	if (Text_in.Find(_T('\n')) < 0)
	{
		uiDTFlags |= DT_VCENTER | DT_SINGLELINE;
		bIsSingleLine = TRUE;
	}
	else
	{
		TextRect.DeflateRect(0, 2);
	}

	switch (AlignStyle_in)
	{
	case CMFCButton::ALIGN_LEFT:
		uiDTFlags |= DT_LEFT;
		TextRect.left += 5;
		break;

	case CMFCButton::ALIGN_RIGHT:
		uiDTFlags |= DT_RIGHT;
		TextRect.right -= 5;
		break;

	case CMFCButton::ALIGN_CENTER:
		uiDTFlags |= DT_CENTER;
	}

	if (IsDisabled_in)
	{
		pDC_in->SetTextColor(afxGlobalData.clrBtnHilite);

		CRect rectShft = TextRect;
		rectShft.OffsetRect(1, 1);
		pDC_in->DrawText(Text_in, rectShft, uiDTFlags);

		Color_in = afxGlobalData.clrGrayedText;
	}

	pDC_in->SetTextColor(Color_in);
	pDC_in->DrawText(Text_in, TextRect, uiDTFlags);
}

/*! \brief Maps the color of the specified bitmap to system colors
	\param[in] hBmp : the handle of the bitmap
	\return TRUE if the image was processed; FALSE otherwise
	\remarks Partially copied from CMFCToolBarImages::MapToGray(...)
*/
BOOL VisualManager::MapToGray(CDC *pDC, HBITMAP& hBmp)
{
	if (hBmp == NULL)
		return FALSE;

	// Create memory source DC and select an original bitmap:
	CDC memDCSrc;

	memDCSrc.CreateCompatibleDC(pDC);

	HBITMAP hOldBitmapSrc = NULL;

	int iBitmapWidth;
	int iBitmapHeight;

	// Get original bitmap attributes:
	BITMAP bmp;
	if (::GetObject(hBmp, sizeof(BITMAP), &bmp) == 0)
		return FALSE;

	hOldBitmapSrc = (HBITMAP) memDCSrc.SelectObject(hBmp);

	iBitmapWidth = bmp.bmWidth;
	iBitmapHeight = bmp.bmHeight;

	// Create a new bitmap compatible with the source memory DC:
	// (original bitmap SHOULD BE ALREADY SELECTED!):
	HBITMAP hNewBitmap = (HBITMAP) ::CreateCompatibleBitmap(memDCSrc, iBitmapWidth, iBitmapHeight);
	if (hNewBitmap == NULL)
	{
		memDCSrc.SelectObject(hOldBitmapSrc);
		return FALSE;
	}

	// Create memory destination DC:
	CDC memDCDst;
	memDCDst.CreateCompatibleDC(&memDCSrc);

	HBITMAP hOldBitmapDst = (HBITMAP) memDCDst.SelectObject(hNewBitmap);
	if (hOldBitmapDst == NULL)
	{
		memDCSrc.SelectObject(hOldBitmapSrc);
		::DeleteObject(hNewBitmap);
		return FALSE;
	}

	// Copy original bitmap to new:
	memDCDst.BitBlt(0, 0, iBitmapWidth, iBitmapHeight, &memDCSrc, 0, 0, SRCCOPY);

	COLORREF clrOrig, clrNew;

	// Change a specific colors to system:
	for (int x = 0; x < iBitmapWidth; x ++)
	{
		for (int y = 0; y < iBitmapHeight; y ++)
		{
			clrOrig = ::GetPixel(memDCDst, x, y);
			clrNew = GrayAverage(clrOrig);

			if (clrOrig != clrNew && clrOrig != 0UL)
				::SetPixel(memDCDst, x, y, clrNew);
		}
	}

	memDCDst.SelectObject(hOldBitmapDst);
	memDCSrc.SelectObject(hOldBitmapSrc);

	::DeleteObject(hBmp);
	hBmp = hNewBitmap;

	return TRUE;
}

// copy of AfxLoadSystemLibraryUsingFullPath(...) from afxglobals.cpp
HMODULE LoadModuleFullPath(_In_z_ const WCHAR *pszLibrary)
{
	WCHAR wszLoadPath[MAX_PATH+1];
	if (::GetSystemDirectoryW(wszLoadPath, _countof(wszLoadPath)) == 0)
	{
		return NULL;
	}

	if (wszLoadPath[wcslen(wszLoadPath)-1] != L'\\')
	{
		if (wcscat_s(wszLoadPath, _countof(wszLoadPath), L"\\") != 0)
		{
			return NULL;
		}
	}

	if (wcscat_s(wszLoadPath, _countof(wszLoadPath), pszLibrary) != 0)
	{
		return NULL;
	}

	return(::AfxCtxLoadLibraryW(wszLoadPath));
}

/*! \brief Prevents the DWM to redraw the non-client area of frames (Windows Vista+)
	\param[in] hWnd_in : the handle of the window for which the rendering is disabled
	\return true if the rendering was disabled
	\remarks Retrieves a function pointer to DwmSetWindowAttribute from dwmapi.dll
			 which is only available from Windows Vista or superior
*/
bool VisualManager::DisableDwmNcRendering(HWND hWnd_in)
{
	HMODULE hDLL = LoadModuleFullPath(_T("dwmapi.dll"));
	bool Result = false;

	if (hDLL != NULL)
	{
		// define the type of a function pointer to DwmSetWindowAttribute
		typedef HRESULT (__stdcall *DWMSETWINDOWATRRIBUTE)(HWND, DWORD, LPCVOID, DWORD);
		// retrieve the function pointer to DwmSetWindowAttribute from dwmapi.dll (available from Windows Vista)
		DWMSETWINDOWATRRIBUTE m_pfDwmSetWindowAttribute = (DWMSETWINDOWATRRIBUTE)::GetProcAddress(hDLL, "DwmSetWindowAttribute");

		if (m_pfDwmSetWindowAttribute != NULL)
		{
			DWMNCRENDERINGPOLICY Policy = DWMNCRP_DISABLED;
			// disabled the non-client area rendering
			HRESULT hResult = m_pfDwmSetWindowAttribute(hWnd_in, DWMWA_NCRENDERING_POLICY,
														&Policy, sizeof(Policy));
			ASSERT(SUCCEEDED(hResult));

			Result = true;
		}
		// unload the library
		::FreeLibrary(hDLL);
	}

	return Result;
}

/*! \brief Checks if the specifying DPI matches the current one
	\param[in] DPI_in : the DPI value to check against the current one
	\return true if the DPI values match; false otherwise
*/
bool VisualManager::IsDPI(int DPI_in)
{
	bool Result = false;
	int DPIX, DPIY;

	if (GetDPI(DPIX, DPIY))
	{
		// check the DPI values => with all Windows versions to date the X and Y DPI will be identical
		Result = (DPIX == DPI_in && DPIY == DPI_in);
	}

	return Result;
}

/*! \brief Retrieves the DPI values using GetDeviceCaps
	\param[in] DPIX_out : receives the number of pixels per logical inch along the screen width
	\param[in] DPIY_out : receives the number of pixels per logical inch along the screen height
	\return true if the values were retrieved; false otherwise
*/
bool __stdcall VisualManager::GetDPI(int &DPIX_out, int &DPIY_out)
{
	HDC hdc = GetDC(NULL);
	bool Result = false;

	DPIX_out = DPIY_out = 0;

	if (hdc)
	{
		// retrieve the DPI values => with all Windows versions to date the X and Y DPI will be identical
		DPIX_out = GetDeviceCaps(hdc, LOGPIXELSX);
		DPIY_out = GetDeviceCaps(hdc, LOGPIXELSY);

		ReleaseDC(NULL, hdc);

		return true;
	}

	return false;
}