/**************************************************************************
	created		:	2013-04-10
	filename	: 	UiColor.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	UI Color
**************************************************************************/
#include "stdafx.h"

#include "UiColor.h"

namespace UIAL
{
	//! \brief CUiColor default constructor
	CUiColor::CUiColor() : m_ARGB(0xFFFFFFFF) {}
	
	/*! \brief CUiColor constructor
		\param[in] ARGB_in : 32-bit color packed ARGB values
	*/
	CUiColor::CUiColor(unsigned long ARGB_in) : m_ARGB(ARGB_in) {}

	/*! \brief CUiColor constructor
		\param[in] Red_in : red color component (ALPHA_SHIFT-255)
		\param[in] Green_in : green color component (ALPHA_SHIFT-255)
		\param[in] Blue_in : blue color component (ALPHA_SHIFT-255)
		\param[in] Alpha_in : alpha color component (ALPHA_SHIFT-255)
	*/
	CUiColor::CUiColor(unsigned char Red_in, unsigned char Green_in, unsigned char Blue_in, unsigned char Alpha_in)
		: m_ARGB(0xFFFFFFFF)
	{
		SetColor(Red_in, Green_in, Blue_in, Alpha_in);
	}

	/*! \brief Retrieves the color as a 32-bit color packed ARGB values
		\return the color as a 32-bit color packed ARGB values
	*/
	unsigned long CUiColor::GetARGB() const
	{ return m_ARGB; }

	/*! \brief Sets the color components from a 32-bit color packed ARGB values
		\param[in] ARGB_in : 32-bit color packed ARGB values
	*/
	void CUiColor::SetARGB(unsigned long ARGB_in)
	{ m_ARGB = ARGB_in; }

	/*! \brief Sets the color of the text
		\param[in] Red_in : the red component of the color
		\param[in] Green_in : the green component of the color
		\param[in] Blue_in : the blue component of the color
		\param[in] Alpha_in : the alpha component of the color
	*/
	void CUiColor::SetColor(unsigned char Red_in, unsigned char Green_in, unsigned char Blue_in, unsigned char Alpha_in)
	{
		SetComponent(Alpha_in, ALPHA_SHIFT);
		SetComponent(Red_in, RED_SHIFT);
		SetComponent(Green_in, GREEN_SHIFT);
		SetComponent(Blue_in, BLUE_SHIFT);
	}

	/*! \brief Retrieves the alpha component of the color
		\return the alpha component of the color
	*/
	unsigned char CUiColor::GetAlpha() const
	{
		return GetComponent(ALPHA_SHIFT);
	}

	/*! \brief Retrieves the green component of the color
		\return the green component of the color
	*/
	unsigned char CUiColor::GetGreen() const
	{
		return GetComponent(GREEN_SHIFT);
	}

	/*! \brief Retrieves the blue component of the color
		\return the blue component of the color
	*/
	unsigned char CUiColor::GetBlue() const
	{
		return GetComponent(BLUE_SHIFT);
	}

	/*! \brief Retrieves the red component of the color
		\return the red component of the color
	*/
	unsigned char CUiColor::GetRed() const
	{
		return GetComponent(RED_SHIFT);
	}

	/*! \brief Sets the alpha component of the color
		\param[in] Alpha_in : the alpha component of the color
	*/
	void CUiColor::SetAlpha(unsigned char Alpha_in)
	{
		return SetComponent(Alpha_in, ALPHA_SHIFT);
	}

	/*! \brief Sets the green component of the color
		\param[in] Green_in : the green component of the color
	*/
	void CUiColor::SetGreen(unsigned char Green_in)
	{
		return SetComponent(Green_in, GREEN_SHIFT);
	}

	/*! \brief Sets the blue component of the color
		\param[in] Blue_in : the blue component of the color
	*/
	void CUiColor::SetBlue(unsigned char Blue_in)
	{
		return SetComponent(Blue_in, BLUE_SHIFT);
	}

	/*! \brief Sets the red component of the color
		\param[in] Red_in : the red component of the color
	*/
	void CUiColor::SetRed(unsigned char Red_in)
	{
		return SetComponent(Red_in, RED_SHIFT);
	}

	/*! \brief Sets a color component given its place in the 32-bit value
		\param[in] Value_in : the new value of the component
		\param[in] Shift_in : the bit shift of the component
	*/
	void CUiColor::SetComponent(unsigned char Value_in, unsigned char Shift_in)
	{
		unsigned long ShiftedValue = Value_in << (BLUE_SHIFT - Shift_in);
		unsigned long Mask = 0x000000FF << (BLUE_SHIFT - Shift_in);

		// combine the value to the color
		m_ARGB = ((m_ARGB & ~Mask) | ShiftedValue);
	}

	/*! \brief Retrieves a color component given its place in the 32-bit value
		\param[in] Shift_in : the bit shift of the component
	*/
	unsigned char CUiColor::GetComponent(unsigned char Shift_in) const
	{
		unsigned long ShiftedARGB = m_ARGB >> (BLUE_SHIFT - Shift_in);

		return (unsigned char)(ShiftedARGB & 0x000000FF);
	}
}