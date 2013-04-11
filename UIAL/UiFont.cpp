/**************************************************************************
	created		:	2013-04-10
	filename	: 	UiFont.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	UI Font
**************************************************************************/
#include "stdafx.h"

#include "UiFont.h"

namespace UIAL
{
	//! \brief CUiFont default constructor
	CUiFont::CUiFont() : m_FontSize(DEFAULT_FONT_SIZE), m_Bold(false), m_Italic(false) {}

	/*! \brief CUiFont constructor
		\param[in] FontName_in : the name of the font e.g. Arial
		\param[in] FontSize_in : the size of the font
		\param[in] bBold_in : flag specifying if the font is bold
		\param[in] bItalic_in : flag specifying if the font is italic
	*/
	CUiFont::CUiFont(const string_t &FontName_in, unsigned short FontSize_in, bool bBold_in, bool bItalic_in)
		: m_FontName(FontName_in), m_FontSize(FontSize_in), m_Bold(bBold_in), m_Italic(bItalic_in) {}

	/*! \brief Sets the font parameters
		\param[in] FontName_in : the name of the font e.g. Arial
		\param[in] FontSize_in : the size of the font
		\param[in] bBold_in : flag specifying if the font is bold
		\param[in] bItalic_in : flag specifying if the font is italic
	*/
	void CUiFont::SetFont(const string_t &FontName_in, unsigned short FontSize_in, bool bBold_in, bool bItalic_in)
	{
		m_FontName = FontName_in;
		m_FontSize = FontSize_in;
		m_Italic = bItalic_in;
		m_Bold = bBold_in;
	}

	/*! \brief Retrieves the name of the font
		\return the name of the font
	*/
	const string_t& CUiFont::GetFontName() const
	{
		return m_FontName;
	}

	/*! \brief Retrieves the size of the font
		\return the size of the font
	*/
	unsigned short CUiFont::GetFontSize() const
	{
		return m_FontSize;
	}

	/*! \brief Checks if the font is italic
		\return true if the font is italic; false otherwise
	*/
	bool CUiFont::IsItalic() const
	{
		return m_Italic;
	}

	/*! \brief Checks if the font is bold
		\return true if the font is bold; false otherwise
	*/
	bool CUiFont::IsBold() const
	{
		return m_Bold;
	}

	/*! \brief Sets the font name
		\param[in] FontName_in : the new name of the font
	*/
	void CUiFont::SetFontName(const string_t &FontName_in)
	{
		m_FontName = FontName_in;
	}

	/*! \brief Sets the font size
		\param[in] FontSize_in : the new size of the font
	*/
	void CUiFont::SetFontSize(unsigned short FontSize_in)
	{
		m_FontSize = FontSize_in;
	}

	/*! \brief Sets the font to bold
		\param[in] bBold_in : flag specifying if the font is bold
	*/
	void CUiFont::SetBold(bool bBold_in)
	{
		m_Bold = bBold_in;
	}

	/*! \brief Sets the font to italic
		\param[in] bItalic_in : flag specifying if the font is italic
	*/
	void CUiFont::SetItalic(bool bItalic_in)
	{
		m_Italic = bItalic_in;
	}
}