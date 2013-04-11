/**************************************************************************
	created		:	2013-04-10
	filename	: 	UiFont.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	UI Font
**************************************************************************/
#ifndef __UI_FONT_H__
#define __UI_FONT_H__

#ifndef _UIAL_SDK_INCLUDE_
	#error Please include the global header file 'UIAL.h'
#endif

namespace UIAL
{
	#define DEFAULT_FONT_SIZE 10U

	class CUiFont
	{
	public:
		CUiFont(const string_t &FontName_in, unsigned short FontSize_in = DEFAULT_FONT_SIZE,
				bool bBold_in = false, bool bItalic_in = false);
		CUiFont();

		void SetFont(const string_t &FontName_in, unsigned short FontSize_in = DEFAULT_FONT_SIZE,
					 bool bBold_in = false, bool bItalic_in = false);

		void SetFontName(const string_t &FontName_in);
		const string_t& GetFontName() const;
		void SetFontSize(unsigned short FontSize_in);
		unsigned short GetFontSize() const;
		void SetItalic(bool bItalic_in);
		bool IsItalic() const;
		void SetBold(bool bBold_in);
		bool IsBold() const;

	private:
		//! font name
		string_t m_FontName;
		//! title text size
		unsigned short m_FontSize;
		//! flag specifying if the text is bold
		bool m_Bold;
		//! flag specifying if the text is italic
		bool m_Italic;
	};
}

#endif//__UI_FONT_H__
