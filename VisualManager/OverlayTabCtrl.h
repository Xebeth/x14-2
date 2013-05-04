/**************************************************************************
	created		:	2012-02-10
	filename	: 	OverlayTabCtrl.h
	author		:	Xebeth`
	copyright	:	North Edge (2012)
	purpose		:	CMFCTabCtrl subclass to support overlay on tabs
**************************************************************************/
#ifndef _VISUAL_MANAGER_SDK_INCLUDE_
	#error Please include the global header file 'VisualManager.h'
#endif

#ifndef __OVERLAY_TABCTRL_H__
#define __OVERLAY_TABCTRL_H__

/*! \brief CMFCTabCtrl subclass to support overlay on tabs */
class APPVIMGR_API COverlayTabCtrl : public CMFCTabCtrl
{
	DECLARE_DYNCREATE(COverlayTabCtrl)
public:
	/*! \brief Function called before the content of the tab has been drawn
		\param[in,out] TextColor_in_out : the color of the text (determined by the framework)
		\param[in] TabIndex_in : the index of the tab
		\param[in] IsActive_in : flag specifying if the tab is active
		\param[in] IsHighlight_in : flag specifying if the tab is highlighted
	*/
	virtual void TextColorModifier(COLORREF &TextColor_in_out, int TabIndex_in,
								   BOOL IsActive_in, BOOL IsHighlight_in) {}
	/*! \brief Function called after the content of the tab has been drawn
		\param[in] pDC_in : the drawing context
		\param[in] TabRect_in : the area of the tab
		\param[in] TabIndex_in : the index of the tab
		\param[in] IsActive_in : flag specifying if the tab is active
		\param[in] IsHighlight_in : flag specifying if the tab is highlighted
	*/
	virtual void DrawOverlay(CDC *pDC_in, CRect &TabRect_in, int TabIndex_in,
							 BOOL IsActive_in, BOOL IsHighlight_in) {}

	// overloaded functions

	/*! \brief Overloaded to prevent the tab control from restoring its serialized tab info
		\param[in] bUseTabIndexes : unused
	*/
	virtual void ApplyRestoredTabInfo(BOOL bUseTabIndexes = FALSE) {}
	/*! \brief Overloaded to prevent the tab control from serializing and storing its tab info
		\param[out] ar : unused
	*/
	virtual void Serialize(CArchive& ar) {}
};

#endif//__OVERLAY_TABCTRL_H__