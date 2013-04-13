/**************************************************************************
	created		:	2013-04-10
	filename	: 	UiTextElement.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	UI text element
**************************************************************************/
#include "stdafx.h"

#include "UiColor.h"
#include "UiFont.h"
#include "UiTextElement.h"

namespace UIAL
{
	//! \brief CUiTextElement default constructor
	CUiTextElement::CUiTextElement() : m_Orientation(0UL) {}

	/*! \brief CUiTextElement constructor
		\param[in] Text_in : the text of the element
		\param[in] FontName_in : the name of the font
		\param[in] FontSize_in : the size of the font
		\param[in] TextColor_in : color of the text
		\param[in] bBold_in : flag specifying if the font is bold
		\param[in] bItalic_in : flag specifying if the font is italic
		\param[in] Orientation_in : orientation of the text
	*/
	CUiTextElement::CUiTextElement(const string_t &Text_in, const string_t &FontName_in, unsigned short FontSize_in,
								   const CUiColor &TextColor_in, bool bBold_in, bool bItalic_in, long Orientation_in)
		: m_Orientation(Orientation_in), m_TextColor(TextColor_in), m_Text(Text_in),
		  m_TextFont(FontName_in, FontSize_in, bBold_in, bItalic_in) {}

	/*! \brief CUiTextElement constructor
		\param[in] Text_in : the text of the element
		\param[in] Font_in : the font of the text
		\param[in] TextColor_in : color of the text
		\param[in] Orientation_in : orientation of the text
	*/
	CUiTextElement::CUiTextElement(const string_t &Text_in, const CUiFont &Font_in,
								   const CUiColor &TextColor_in, long Orientation_in)
		: m_Orientation(Orientation_in), m_TextColor(TextColor_in), 
		  m_Text(Text_in), m_TextFont(Font_in) {}

	/*! \brief Sets the text of the element
		\param[in] Text_in : the new text
	*/
	void CUiTextElement::SetText(const string_t &Text_in)
	{	m_Text = Text_in;	}

	/*! \brief Retrieves the text of the element
		\return the text of the element
	*/
	const string_t& CUiTextElement::GetText() const
	{	return m_Text;	}

	/*! \brief Sets the orientation of the text
		\param[in] Orientation_in : the new orientation
	*/
	void CUiTextElement::SetOrientation(long Orientation_in)
	{
		m_Orientation = Orientation_in;
	}

	/*! \brief Retrieves the orientation of the text
		\return the orientation of the text
	*/
	long CUiTextElement::GetOrientation() const
	{
		return m_Orientation;
	}

	/*! \brief Sets the color of the text
		\param[in] Red_in : the red component of the color
		\param[in] Green_in : the green component of the color
		\param[in] Blue_in : the blue component of the color
		\param[in] Alpha_in : the alpha component of the color
	*/
	void CUiTextElement::SetTextColor(unsigned char Red_in, unsigned char Green_in,
									  unsigned char Blue_in, unsigned char Alpha_in)
	{
		m_TextColor.SetColor(Red_in, Green_in, Blue_in, Alpha_in);
	}

	/*! \brief Sets the color of the text
		\param[in] Color_in : the new color of the text
	*/
	void CUiTextElement::SetTextColor(const CUiColor &Color_in)
	{
		m_TextColor = Color_in;
	}

	/*! \brief Sets the color of the text
		\param[in] ARGB_in : 32-bit color packed ARGB values
	*/
	void CUiTextElement::SetTextColor(unsigned long ARGB_in)
	{
		m_TextColor.SetARGB(ARGB_in);
	}

	/*! \brief Retrieves the color of the text
		\return the color of the text
	*/
	const CUiColor& CUiTextElement::GetTextColor() const
	{
		return m_TextColor;
	}

	/*! \brief Sets the font of the text element
		\param[in] FontName_in : the name of the font e.g. Arial
		\param[in] FontSize_in : the size of the font
		\param[in] bBold_in : flag specifying if the font is bold
		\param[in] bItalic_in : flag specifying if the font is italic
	*/
	void CUiTextElement::SetTextFont(const string_t &FontName_in, unsigned short FontSize_in,
									 bool bBold_in, bool bItalic_in)
	{
		m_TextFont.SetFont(FontName_in, FontSize_in, bBold_in, bItalic_in);
	}

	/*! \brief Sets the font of the text element
		\param[in] Font_in : the new font
	*/
	void CUiTextElement::SetTextFont(const CUiFont &Font_in)
	{
		m_TextFont = Font_in;
	}

	/*! \brief Retrieves the font of the text element
		\return the font of the text element
	*/
	const CUiFont& CUiTextElement::GetTextFont() const
	{
		return m_TextFont;
	}
}