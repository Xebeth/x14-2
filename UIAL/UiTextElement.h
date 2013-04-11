/**************************************************************************
	created		:	2013-04-10
	filename	: 	CUiTextElement.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	UI text element
**************************************************************************/
#ifndef __UI_TEXT_ELEMENT_H__
#define __UI_TEXT_ELEMENT_H__

namespace UIAL
{
	#define DEFAULT_TEXT_ORIENTATION 0L

	class CUiTextElement
	{
	public:
		CUiTextElement(const string_t &Text_in, const string_t &FontName_in,
					   unsigned short FontSize_in, const CUiColor &TextColor_in,
					   bool bBold_in = false, bool bItalic_in = false,
					   long Orientation_in = DEFAULT_TEXT_ORIENTATION);
		CUiTextElement(const string_t &Text_in, const CUiFont &Font_in, const CUiColor &TextColor_in, 
					   long Orientation_in = DEFAULT_TEXT_ORIENTATION);
		CUiTextElement();

		void SetText(const string_t &Text_in);
		const string_t& GetText() const;

		void SetOrientation(long Orientation_in);
		long GetOrientation() const;
		
		void SetTextColor(unsigned char Red_in, unsigned char Green_in,
						  unsigned char Blue_in, unsigned char Alpha_in = 255U);
		void SetTextColor(const CUiColor &Color_in);
		void SetTextColor(unsigned long ARGB_in);
		const CUiColor& GetTextColor() const;

		void SetTextFont(const string_t &FontName_in, unsigned short FontSize_in = DEFAULT_FONT_SIZE,
						 bool bBold_in = false, bool bItalic_in = false);
		void SetTextFont(const CUiFont &Font_in);
		const CUiFont& GetTextFont() const;

	private:
		//! text of the element
		string_t m_Text;
		//! text font
		CUiFont m_TextFont;
		//! text orientation
		long m_Orientation;
		//! color of the text
		CUiColor m_TextColor;
	};
}

#endif//__UI_TEXT_ELEMENT_H__
