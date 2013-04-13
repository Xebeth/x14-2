/**************************************************************************
	created		:	2013-03-05
	filename	: 	UiWindow.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __UI_WINDOW_H__
#define __UI_WINDOW_H__

#ifndef _UIAL_SDK_INCLUDE_
	#error Please include the global header file 'UIAL.h'
#endif

#include "IWindowRenderer.h"

namespace UIAL
{
	class CUiTextElement;

	template <typename T = long, typename U = FRHCS> class CUiWindow : public CUiElement<T, U>
	{
	public:
		CUiWindow(unsigned long ID_in, const CUiTextElement &Title_in, const CUiColor BckgColor_in, IWindowRenderer<T,U> *pRenderer_in = NULL):
			: CUiElement<T,U>(ID_in), m_pRenderer(pRenderer_in), m_Title(Title_in),
			  m_BackgroundColor(BckgColor_in) {}

		CUiWindow(unsigned long ID_in, const string_t &Title_in, const CUiFont &Font_in, const CUiColor &TextColor_in,
				  const CUiColor BckgColor_in, IWindowRenderer<T,U> *pRenderer_in = NULL)
			: CUiElement<T,U>(ID_in), m_pRenderer(pRenderer_in), m_Title(Title_in, Font_in, TextColor_in),
			  m_BackgroundColor(BckgColor_in) {}

		CUiWindow(unsigned long ID_in, const string_t& Name_in, const CUiRect& Rect_in, 
				  bool Visible_in, const CUiTextElement &Title_in,
				  const CUiColor BckgColor_in, IWindowRenderer<T,U> *pRenderer_in = NULL)
			: CUiElement<T,U>(ID_in, Name_in, Rect_in, Visible_in), m_pRenderer(pRenderer_in),
			  m_Title(Title_in), m_BackgroundColor(BckgColor_in) {}

		CUiWindow(unsigned long ID_in, const string_t& Name_in, const CUiRect& Rect_in,
				  bool Visible_in, const string_t &Title_in, const CUiFont &Font_in,
				  const CUiColor &TextColor_in, const CUiColor BckgColor_in, IWindowRenderer<T,U> *pRenderer_in = NULL)
			: CUiElement<T,U>(ID_in, Name_in, Rect_in, Visible_in), m_pRenderer(pRenderer_in),
			  m_Title(Title_in, Font_in, TextColor_in), m_BackgroundColor(BckgColor_in) {}

		CUiWindow(unsigned long ID_in, const string_t& Name_in, const CUiPoint& TopLeft_in,
				  const CUiPoint& BottomRight_in, bool Visible_in, const CUiTextElement &Title_in,
				  const CUiColor BckgColor_in, IWindowRenderer<T,U> *pRenderer_in = NULL)
		  : CUiElement<T,U>(ID_in, Name_in, TopLeft_in, BottomRight_in, Visible_in), m_pRenderer(pRenderer_in),
		    m_Title(Title_in), m_BackgroundColor(BckgColor_in) {}  

		CUiWindow(unsigned long ID_in, const string_t& Name_in, const CUiPoint& TopLeft_in,
				  const CUiPoint& BottomRight_in, bool Visible_in, const string_t &Title_in,
				  const CUiFont &Font_in, const CUiColor &TextColor_in, const CUiColor BckgColor_in,
				  IWindowRenderer<T,U> *pRenderer_in = NULL)
			: CUiElement<T,U>(ID_in, Name_in, TopLeft_in, BottomRight_in, Visible_in), m_pRenderer(pRenderer_in),
			  m_Title(Title_in, Font_in, TextColor_in), m_BackgroundColor(BckgColor_in) {}

		CUiWindow(unsigned long ID_in, const string_t& Name_in, const CUiPoint& Pos_in,
				  T W_in, T H_in, bool Visible_in, const CUiTextElement &Title_in,
				  const CUiColor BckgColor_in, IWindowRenderer<T,U> *pRenderer_in = NULL)
			: CUiElement<T,U>(ID_in, Name_in, Pos_in, W_in, H_in, Visible_in), m_pRenderer(pRenderer_in),
			  m_Title(Title_in), m_BackgroundColor(BckgColor_in) {}

		CUiWindow(unsigned long ID_in, const string_t& Name_in, const CUiPoint& Pos_in,
				  T W_in, T H_in, bool Visible_in, const string_t &Title_in, const CUiFont &Font_in,
				  const CUiColor &TextColor_in, const CUiColor BckgColor_in, IWindowRenderer<T,U> *pRenderer_in = NULL)
			: CUiElement<T,U>(ID_in, Name_in, Pos_in, W_in, H_in, Visible_in), m_pRenderer(pRenderer_in),
			  m_Title(Title_in, Font_in, TextColor_in), m_BackgroundColor(BckgColor_in) {}

		CUiWindow(unsigned long ID_in, const string_t& Name_in, T X_in, T Y_in,
				  T W_in, T H_in, bool Visible_in, const CUiTextElement &Title_in,
				  const CUiColor BckgColor_in, IWindowRenderer<T,U> *pRenderer_in = NULL)
			: CUiElement<T,U>(ID_in, Name_in, X_in, Y_in, W_in, H_in, Visible_in), m_pRenderer(pRenderer_in),
			  m_Title(Title_in), m_BackgroundColor(BckgColor_in) {}

		CUiWindow(unsigned long ID_in, const string_t& Name_in, T X_in, T Y_in, T W_in, T H_in,
				  bool Visible_in, const string_t &Title_in, const CUiFont &Font_in,
				  const CUiColor &TextColor_in, const CUiColor BckgColor_in, IWindowRenderer<T,U> *pRenderer_in = NULL)
			: CUiElement<T,U>(ID_in, Name_in, X_in, Y_in, W_in, H_in, Visible_in), m_pRenderer(pRenderer_in),
			  m_Title(Title_in, Font_in, TextColor_in), m_BackgroundColor(BckgColor_in) {}

		explicit CUiWindow(unsigned long ID_in) : CUiElement<T,U>(ID_in) {}

		void SetRenderer(IWindowRenderer<T,U> *pRenderer_in) { m_pRenderer = pRenderer_in; }
		const IWindowRenderer<T,U>* GetRenderer() const { return m_pRenderer; }

		void SetBackgroundColor(unsigned char Red_in, unsigned char Green_in,
								unsigned char Blue_in, unsigned char Alpha_in = 255U)
		{ m_BackgroundColor.SetColor(Red_in, Green_in, Blue_in, Alpha_in); }
		void SetBackgroundColor(const CUiColor &Color_in) { m_BackgroundColor = Color_in; }
		void SetBackgroundColor(unsigned long ARGB_in) { m_BackgroundColor.SetColor(ARGB_in); }
		const CUiColor& GetBackgroundColor() const { return m_BackgroundColor; }

		void SetTitleText(const string_t &Title_in) { m_Title.SetText(Title_in); }
		const string_t& GetTitleText() const { return m_Title.GetText(); }

		void SetTitleColor(unsigned char Red_in, unsigned char Green_in,
						   unsigned char Blue_in, unsigned char Alpha_in = 255U)
		{ m_Title.SetTextColor(Red_in, Green_in, Blue_in, Alpha_in); }
		void SetTitleColor(const CUiColor &Color_in) { m_Title.SetTextColor(Color_in); }
		void SetTitleColor(unsigned long ARGB_in) { m_Title.SetTextColor(ARGB_in); }
		const CUiColor& GetTitleColor() const { return m_Title.GetTextColor(); }

		void SetTitleFont(const string_t &FontName_in, unsigned short FontSize_in = DEFAULT_FONT_SIZE,
						  bool bBold_in = false, bool bItalic_in = false)
		{ m_Title.SetTextFont(FontName_in, FontSize_in, bBold_in, bItalic_in); }
		void SetTitleFont(const CUiFont &Font_in) { m_Title.SetTextFont(Font_in); }
		const CUiFont& GetTitleFont() const { return m_Title.GetTextFont(); }

		virtual bool Draw()
		{
			if (m_Visible && m_pRenderer != NULL)
			{
				// before render event
				m_pRenderer->BeforeRender(*this);
				// render the window
				m_pRenderer->Render(*this);
				// reset the dirty flag
				SetDirty(false);
				// after render event
				m_pRenderer->AfterRender(*this);

				return true;
			}

			return false;
		}

	protected:
		//! title of the window
		CUiTextElement m_Title;
		//! color of the background
		CUiColor m_BackgroundColor;
		//! Window renderer
		IWindowRenderer<T,U> *m_pRenderer;
	};
}

#endif//__UI_WINDOW_H__
